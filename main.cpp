#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "BinderLayer/Binder.h"
#include "CatalogLayer/Catalog.h"
#include "DiskLayer/DiskManager.h"
#include "ExecutionLayer/InsertExecutor.h"
#include "ExecutionLayer/SelectExecutor.h"
#include "MemoryManagementLayer/BufferPoolManager.h"
#include "SyntaxLayer/Parser.h"
#include "SyntaxLayer/Tokenizer.h"

namespace {

constexpr std::uint32_t REPL_METADATA_MAGIC = 0x4D534551;

struct ReplMetadata {
    std::uint32_t magic;
    int users_first_page_id;
};

void printHelp() {
    std::cout << "Commands:\n";
    std::cout << "  INSERT INTO users VALUES (1, 'mark', 'mark@example.com');\n";
    std::cout << "  SELECT * FROM users;\n";
    std::cout << "  SELECT id, name FROM users;\n";
    std::cout << "  .help\n";
    std::cout << "  .exit\n";
}

void printRows(const std::vector<ResultRow>& rows) {
    if (rows.empty()) {
        std::cout << "(0 rows)\n";
        return;
    }

    for (const ResultRow& row : rows) {
        const std::vector<std::string>& values = row.getValues();
        for (std::size_t i = 0; i < values.size(); i++) {
            if (i > 0) {
                std::cout << " | ";
            }
            std::cout << values[i];
        }
        std::cout << '\n';
    }

    std::cout << "(" << rows.size() << " row";
    if (rows.size() != 1) {
        std::cout << "s";
    }
    std::cout << ")\n";
}

bool handleSelect(const std::vector<Token>& tokens, const Parser& parser, const Binder& binder,
                  const SelectExecutor& select_executor) {
    ParseResult parse_result = parser.parseSelect(tokens);
    if (!parse_result.succeeded()) {
        std::cout << "Parse error: " << parse_result.getErrorMessage() << '\n';
        return true;
    }

    std::optional<BoundTableStatement> bound = binder.bindSelect(parse_result.getStatement().value());
    if (!bound.has_value()) {
        std::cout << "Bind error: could not resolve SELECT statement against the catalog\n";
        return true;
    }

    std::optional<std::vector<ResultRow>> rows = select_executor.execute(bound.value());
    if (!rows.has_value()) {
        std::cout << "Execution error: SELECT failed\n";
        return true;
    }

    printRows(rows.value());
    return true;
}

bool handleInsert(const std::vector<Token>& tokens, const Parser& parser, const Binder& binder,
                  const InsertExecutor& insert_executor) {
    InsertParseResult parse_result = parser.parseInsert(tokens);
    if (!parse_result.succeeded()) {
        std::cout << "Parse error: " << parse_result.getErrorMessage() << '\n';
        return true;
    }

    std::optional<BoundInsertStatement> bound = binder.bindInsert(parse_result.getStatement().value());
    if (!bound.has_value()) {
        std::cout << "Bind error: INSERT values do not match the target table schema\n";
        return true;
    }

    std::optional<RID> rid = insert_executor.execute(bound.value());
    if (!rid.has_value()) {
        std::cout << "Execution error: INSERT failed\n";
        return true;
    }

    std::cout << "Inserted row at page " << rid->page_id << ", slot " << rid->slot_id << '\n';
    return true;
}

ReplMetadata readMetadata(DiskManager& disk_manager) {
    char page_buffer[PAGE_SIZE]{0};
    disk_manager.readPage(0, page_buffer);

    ReplMetadata metadata{};
    std::memcpy(&metadata, page_buffer, sizeof(ReplMetadata));
    return metadata;
}

void writeMetadata(DiskManager& disk_manager, const ReplMetadata& metadata) {
    char page_buffer[PAGE_SIZE]{0};
    std::memcpy(page_buffer, &metadata, sizeof(ReplMetadata));
    disk_manager.writePage(0, page_buffer);
}

} // namespace

int main() {
    DiskManager disk_manager("Storage/temp.db");
    BufferPoolManager buffer_pool_manager(&disk_manager);
    Catalog catalog(&disk_manager, &buffer_pool_manager);
    Binder binder(&catalog);
    Tokenizer tokenizer;
    Parser parser;
    InsertExecutor insert_executor;
    SelectExecutor select_executor;

    Schema users_schema({
        Column("id", TypeId::INTEGER, false),
        Column("name", TypeId::TEXT),
        Column("email", TypeId::TEXT)
    });

    ReplMetadata metadata = readMetadata(disk_manager);
    if (metadata.magic != REPL_METADATA_MAGIC) {
        const int metadata_page_id = disk_manager.allocatePage();
        if (metadata_page_id != 0) {
            std::cerr << "Failed to initialize REPL metadata page\n";
            return 1;
        }

        TableInfo* users_table = catalog.createTable("users", users_schema);
        if (users_table == nullptr) {
            std::cerr << "Failed to bootstrap users table for REPL\n";
            return 1;
        }

        metadata.magic = REPL_METADATA_MAGIC;
        metadata.users_first_page_id = users_table->getTableHeap()->getFirstPageId();
        writeMetadata(disk_manager, metadata);
    } else {
        if (catalog.loadTable("users", users_schema, metadata.users_first_page_id) == nullptr) {
            std::cerr << "Failed to reopen persisted users table for REPL\n";
            return 1;
        }
    }

    std::cout << "marksql minimal REPL\n";
    std::cout << "Bootstrapped table: users(id, name, email)\n";
    printHelp();

    std::string line;
    while (true) {
        std::cout << "marksql> ";
        if (!std::getline(std::cin, line)) {
            std::cout << '\n';
            break;
        }

        if(line == "exit" || line == "EXIT" || line == "QUIT" || line == "quit") {
            std::cout << "Bye..." << std::endl;
            break;
        }

        if (line.empty()) {
            continue;
        }

        if (line == ".exit") {
            break;
        }

        if (line == ".help") {
            printHelp();
            continue;
        }

        std::optional<std::vector<Token>> tokens = tokenizer.tokenize(line);
        if (!tokens.has_value()) {
            std::cout << "Tokenizer error: unsupported input\n";
            continue;
        }

        if (tokens->empty()) {
            continue;
        }

        TokenType first_token = tokens->front().getType();
        if (first_token == TokenType::SELECT) {
            handleSelect(tokens.value(), parser, binder, select_executor);
            continue;
        }

        if (first_token == TokenType::INSERT) {
            handleInsert(tokens.value(), parser, binder, insert_executor);
            continue;
        }

        std::cout << "Unsupported command. Only SELECT and INSERT are implemented right now.\n";
    }

    return 0;
}
