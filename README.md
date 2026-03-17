# marksql

`marksql` is a DBMS prototype in C++20. The codebase is already structured around the classic pieces of a database storage stack, even though several behaviors are still incomplete. This README is meant to be a fast architectural refresher so you can come back later and remember how the pieces are intended to fit together.

## What exists today

The current implementation is centered on a paged storage model:

- `DiskLayer/` handles page-sized reads, writes, and page-id allocation against a database file.
- `MemoryManagementLayer/` provides an in-memory buffer pool plus an LRU replacement policy.
- `RecordLayer/` interprets raw page bytes as a slotted heap page that can store variable-length tuples.
- `TableLayer/` provides a table-heap abstraction over linked heap pages.
- `Models/` contains the low-level structs and page wrappers used across the stack.
- `Tests/` exercises each layer independently.

The project is not yet a full database. There is no SQL layer, parser, planner, executor, schema system, WAL, or transaction manager. Right now the code is mostly about learning and validating the storage-engine path from disk page -> buffer frame -> heap page -> tuple.

## High-level architecture

The intended flow is:

1. `DiskManager` owns a database file and treats it as an array of fixed-size pages.
2. `BufferPoolManager` caches those pages in RAM as `Page` objects.
3. `LRUReplacer` decides which unpinned frame can be evicted when the buffer pool is full.
4. `HeapPage` interprets a page's raw `char[4096]` payload as a slotted page storing tuples.
5. `TableHeap` walks one or more heap pages and exposes table-style operations such as insert, read, delete, and edit by `RID`.

If you want one mental model for the codebase, it is this:

`TableHeap` is the public storage abstraction, `HeapPage` is the record format, `BufferPoolManager` is the RAM cache, and `DiskManager` is the persistence boundary.

## Directory map

### `Models/`

Shared data structures used by multiple layers.

- `Page`: an in-memory page frame with metadata:
  - `page_id_`
  - `data_[PAGE_SIZE]`
  - dirty bit
  - pin count
- `RID`: `(page_id, slot_id)` address of a tuple.
- `Slot`: slot-directory entry `(offset, length)` for one tuple.
- `HeapPageHeader`: compact header placed at the start of a heap page:
  - number of slots
  - start of slot directory / free-space metadata
  - end of free space
- `Tuple.h`: currently not the tuple type used by the heap-page code. The real tuple payload type in `HeapPage` is currently `using Tuple = std::vector<char>;`.

### `DiskLayer/`

Contains `DiskManager`, the lowest layer that talks to the filesystem.

Responsibilities:

- Open or create the backing file.
- Read exactly one page with `pread`.
- Write exactly one page with `pwrite`.
- Hand out monotonically increasing page ids via `allocatePage()`.

Important design point:

- `DiskManager` does not know anything about records, slots, or tables.
- It only understands page ids and byte offsets.

### `MemoryManagementLayer/`

Contains the in-memory cache and replacement policy.

#### `BufferPoolManager`

Owns:

- A fixed-size array of `Page` frames (`BUFFER_POOL_CAPACITY = 1000`).
- A page table mapping `page_id -> frame index`.
- A free-list of never-used frames.
- An `LRUReplacer` for choosing eviction victims among unpinned frames.

Main behaviors:

- `fetchPage(page_id)`
  - Returns a cached page if already resident.
  - Otherwise loads it from disk into a free frame or an evicted frame.
  - Pins the page while it is in active use.
- `unpinPage(page_id, is_dirty)`
  - Decrements pin count.
  - Marks the page dirty if requested.
  - Makes it eligible for eviction once the pin count reaches zero.
- `newPage(...)`
  - Allocates a fresh page id through `DiskManager`.
  - Reserves a frame for it.
  - Returns a pinned, dirty page ready to be initialized by upper layers.
- `flushPage(page_id)`
  - Forces the current in-memory bytes to disk.
- `deletePage(page_id)`
  - Removes an unpinned page from the buffer pool and frees its frame for reuse.

#### `Replacer/`

Defines the replacement-policy interface and the current implementation:

- `Replacer`: abstract base class.
- `LRUReplacer`: linked-list + hash-map implementation of least-recently-used eviction among unpinned frames.

Conceptually:

- `pin(frame_id)` removes a frame from the eviction set.
- `unpin(frame_id)` adds or moves a frame to the back of the LRU list.
- `victim(...)` evicts the least recently used unpinned frame.

### `RecordLayer/`

Contains `HeapPage`, which overlays structure onto a raw page buffer.

The page layout is a slotted-page design:

- The header lives at the beginning of the page.
- The slot directory grows forward from the front.
- Tuple bytes are copied into the page from the back downward.
- Free space is the gap between those two regions.

That means inserts work like this:

1. Check whether there is enough free space for tuple bytes plus one `Slot`.
2. Copy the tuple bytes near the end of free space.
3. Append a slot entry containing offset and length.
4. Advance `free_space_start`.
5. Move `free_space_end` backward.

Current `HeapPage` operations:

- `insertTuple`
- `getTuple`
- `applyDelete`
- `changeTuple` exists but is still effectively unfinished in the implementation
- `setNextPage` / `setPrevPage` and getters exist at the object level

Important caveat:

The current next/prev page ids are stored as member fields on `HeapPage`, not serialized into the underlying page bytes. So page links behave like transient wrapper state rather than durable page metadata right now.

### `TableLayer/`

Contains `TableHeap`, the highest-level storage abstraction currently present.

`TableHeap` owns:

- a `DiskManager*`
- a `BufferPoolManager*`
- `first_page_id_`

The constructor allocates the first table page immediately.

Main behaviors:

- `insertTuple`
  - Starts from `first_page_id_`
  - Fetches a page through the buffer pool
  - Wraps it as a `HeapPage`
  - Tries to insert the tuple
  - Returns an `RID` with `(page_id, slot_id)`
- `getTuple`
  - Walks pages until it finds the target page id and reads the slot
- `applyDelete`
  - Walks pages until it finds the target page id and marks the slot deleted
- `editTuple`
  - Intended to update in place when possible, or delete-and-reinsert when not

Conceptually this is the start of a heap-file implementation: a table is a linked sequence of slotted pages, and each tuple is addressed by `RID`.

## How the layers interact

### Insert path

When a tuple is inserted through `TableHeap`:

1. `TableHeap` chooses a target page, starting at `first_page_id_`.
2. It asks `BufferPoolManager` for that page.
3. `BufferPoolManager` either:
   - returns the cached frame, or
   - loads the page from disk through `DiskManager`, or
   - evicts an unpinned frame and reuses it.
4. `TableHeap` constructs a `HeapPage` view over the page's raw bytes.
5. `HeapPage::insertTuple` copies the tuple bytes and appends a slot entry.
6. `TableHeap` returns the resulting `RID`.

### Read path

When reading a tuple by `RID`:

1. `TableHeap` fetches candidate pages through the buffer pool.
2. Each page is interpreted as a `HeapPage`.
3. Once the target `page_id` is found, the slot directory is used to reconstruct the tuple bytes.

### Eviction path

When the buffer pool is full:

1. `BufferPoolManager` asks `LRUReplacer` for a victim frame.
2. If the victim page is dirty, it is written back via `DiskManager`.
3. The old page id is removed from the page table.
4. The frame is reused for the requested page.

## Current implementation status and rough edges

This is the part worth rereading before making changes later, because it explains what is architectural intent versus what is already solid.

### Solid enough to build on

- Fixed-size page abstraction.
- Disk-backed page read/write.
- Buffer pool with pin/unpin semantics.
- LRU-based eviction of unpinned frames.
- Slotted heap-page insert and fetch.
- Basic table-heap wrapper and tests for the main happy paths.

### Incomplete or inconsistent pieces

- `HeapPage::changeTuple` is a stub, but tests already describe the intended behavior.
- `TableHeap::editTuple` is written around the idea of in-place update or reinsertion, but it currently depends on the unfinished `HeapPage::changeTuple`.
- Heap-page linked-list metadata is not persisted inside the page bytes yet.
- `TableHeap::insertTuple` allocates a new page id if `fetchPage` returns `nullptr`, but it does not fully initialize and link a new heap page into a durable page chain.
- Pin/unpin discipline is not consistently enforced across all table-layer operations, so long-running usage could keep pages pinned longer than intended.
- Deletion currently zeroes the slot length, but there is no space reclamation or compaction.
- The `Tuple` class in `Models/Tuple.h` does not match the `Tuple` alias used by `HeapPage` and `TableHeap`.
- `main.cpp` is not yet a real entry point for the storage engine; most meaningful validation lives in `Tests/`.

## How to read this code quickly next time

If you want the fastest re-entry path into the project, read files in this order:

1. `TableLayer/TableHeap.h` and `.cpp`
2. `RecordLayer/HeapPage.h` and `.cpp`
3. `MemoryManagementLayer/BufferPoolManager.h` and `.cpp`
4. `MemoryManagementLayer/Replacer/LRUReplacer.h` and `.cpp`
5. `DiskLayer/DiskManager.h` and `.cpp`
6. `Models/Page.h`
7. `Tests/HeapPageTests.cpp`, `Tests/BpmTests.cpp`, and `Tests/TableHeapTests.cpp`

That order mirrors the logical abstraction stack from highest-level storage API down to disk.

## Build and test

The project uses CMake and GoogleTest.

Typical flow:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

Defined test executables include:

- `testDM`
- `testPage`
- `testReplacer`
- `testBPM`
- `testHeapPage`
- `testTableHeap`

Sanitizers are enabled in the CMake configuration for the main library, which is useful while iterating on low-level page and memory logic.

## Short mental summary

`marksql` is currently a storage-engine skeleton:

- disk pages at the bottom
- a buffer pool in the middle
- slotted heap pages on top of raw page bytes
- a table-heap API on top of heap pages

The architecture is already recognizable as a miniature database storage stack. The remaining work is mostly about making the table/page linkage durable, finishing tuple update behavior, tightening lifecycle rules around pinning and flushing, and then building more database functionality above this foundation.
