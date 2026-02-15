//
// Created by Mark on 2/11/26.
//

#include "DiskManager.h"

#include <utility>


DiskManager::DiskManager() : DiskManager(filepath) {

}



DiskManager::DiskManager(std::string customFilePath) : filepath_(std::move(customFilePath)){

    fd_ = open(filepath_.c_str(), O_RDWR | O_CREAT, filePermissionCode);
    //get byte size of file
    //next pointer set to 1 + (bytes / page_size)
    if (fd_ < 0) {
        perror("open");
        throw std::runtime_error("File opening failed.");
    }
    off_t fileSize = lseek(fd_, 0, SEEK_END);

    if (fileSize == static_cast<off_t>(-1)) {
        throw std::runtime_error("File size reading error.");
    }
    next_page_id_ = fileSize / PAGE_SIZE;

}



DiskManager::~DiskManager() {
    if (fd_ >= 0) {
        close(fd_);
    }
}



void DiskManager::writePage(int page_id, char *buf) {
    off_t start = DiskManager::getByteOffset(page_id);
    ssize_t status = pwrite(fd_, buf, PAGE_SIZE, start);

    if (status != PAGE_SIZE) {
        throw std::runtime_error("Error in writePage()");
    }

}


char* DiskManager::readPage(int page_id, char* buf) {

    off_t start = DiskManager::getByteOffset(page_id);

    //read page from offset, read of size PAGE_SIZE;
    int status = pread(fd_, buf, PAGE_SIZE, start);

    if (status < 0) {
        throw std::runtime_error("Error reading page");
    }

    return buf;
}

size_t DiskManager::allocatePage() {
    return next_page_id_++;
}




 off_t DiskManager::getByteOffset(size_t page_id) {

    return static_cast<off_t>(page_id) * PAGE_SIZE;
}
