#ifndef MMAP_WRITER_H_
#define MMAP_WRITER_H_

#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string>

template<class DataType>
class MMapWriter {
public:
  MMapWriter(const std::string& mmap_base_dir) : mmap_base_dir_(mmap_base_dir) {}
  ~MMapWriter() {
    close(data_fd_);
    close(meta_fd_);
  }

  bool Init() {
    data_fd_ = open((mmap_base_dir_ + std::string{"market.data"}).c_str(), O_RDWR, 0777);
    if (data_fd_ < 0) {
      perror("open mmap error");
      return false;
    }

    meta_fd_ = open((mmap_base_dir_ + std::string{"market.meta"}).c_str(), O_RDWR, 0777);
    if (data_fd_ < 0) {
      perror("open meta error");
      return false;
    }

    mmap_ptr_ = static_cast<void*>(mmap(NULL, sizeof(DataType) * MAX_MSG_LEN, PROT_READ|PROT_WRITE, MAP_SHARED, data_fd_, 0));
    int total_mmap_size = sizeof(DataType) * MAX_MSG_LEN;
    int total_page_count = total_mmap_size / PAGE_SIZE + 1;
    char* start_addr = mmap_ptr_;
    for (int i = 0; i < total_page_count; ++i) {
      manual_trigger_page_fault(start_addr);
      start_addr += PAGE_SIZE;
    }
    if (mmap_ptr_ == MAP_FAILED) {
      perror("mmap error");
      return false;
    }
    if (madvise(mmap_ptr_, total_mmap_size, MADV_WILLNEED) < 0) {
      perror("madvise error");
      return false;
    }
    return true;
  }

  void Prefault(const DataType& data) {
    memcpy(mmap_ptr_, &data, sizeof(DataType));
    mmap_ptr_ = static_cast<char*>(mmap_ptr_) + sizeof(DataType);
  }
private:
  void manual_trigger_page_fault(char* page_start_addr) {
    char* c = page_start_addr;
    memcpy(page_start_addr, c, 1);
  }

  std::string mmap_base_dir_;
  void* mmap_ptr_;
  int data_fd_;
  int meta_fd_;
  static constexpr int MAX_MSG_LEN = 20000000;
  size_t PAGE_SIZE = sysconf(_SC_PAGESIZE);
};

#endif
