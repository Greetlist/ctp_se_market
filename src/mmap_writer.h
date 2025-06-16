#ifndef MMAP_WRITER_H_
#define MMAP_WRITER_H_

#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string>

template<class DataType>
class MMapWriter {
public:
  MMapWriter(const std::string& mmap_base_dir) : mmap_base_dir_(mmap_base_dir) {}
  ~MMapWriter() {
    msync(data_ptr_, TOTAL_BYTES, MS_SYNC);
    munmap(meta_ptr_, TOTAL_BYTES);
    close(data_fd_);
    close(meta_fd_);
  }

  bool Init() {
    data_fd_ = open((mmap_base_dir_ + std::string{"market.data"}).c_str(), O_RDWR|O_CREAT|O_TRUNC, 0777);
    if (data_fd_ < 0) {
      perror("open mmap error");
      return false;
    }

    meta_fd_ = open((mmap_base_dir_ + std::string{"market.meta"}).c_str(), O_RDWR|O_CREAT, 0777);
    if (meta_fd_ < 0) {
      perror("open meta error");
      return false;
    }

    //Important !!!
    ftruncate(data_fd_, TOTAL_BYTES);
    ftruncate(meta_fd_, META_BYTES);

    // for data ptr
    data_ptr_ = mmap(NULL, TOTAL_BYTES, PROT_READ|PROT_WRITE, MAP_SHARED, data_fd_, 0);
    if (data_ptr_ == MAP_FAILED) {
      perror("open data mmap error");
      return false;
    }

    int total_page_count = TOTAL_BYTES / PAGE_SIZE + 1;
    for (int i = 0; i < total_page_count; ++i) {
      manual_trigger_page_fault(i);
    }
    if (madvise(data_ptr_, TOTAL_BYTES, MADV_WILLNEED) < 0) {
      perror("madvise error");
      return false;
    }

    // for meta ptr
    meta_ptr_ = mmap(NULL, META_BYTES, PROT_READ|PROT_WRITE, MAP_SHARED, meta_fd_, 0);
    if (meta_ptr_ == MAP_FAILED) {
      perror("open meta mmap error");
      return false;
    }
    //write data struct size first
    uint64_t* write_ptr = (uint64_t*)meta_ptr_;
    memset(write_ptr, 0, META_BYTES);
    *write_ptr = sizeof(DataType);
    return true;
  }

  void Prefault(const DataType& data) {
    memcpy(data_ptr_, &data, sizeof(DataType));
    data_ptr_ = static_cast<char*>(data_ptr_) + sizeof(DataType);
    asm volatile("" ::: "memory"); //prevent compiler reorder
    asm volatile("mfence" ::: "memory"); //prevent cpu reorder
    uint64_t* write_ptr = (uint64_t*)((char*)meta_ptr_ + sizeof(uint64_t));
    (*write_ptr)++;
  }
private:
  void manual_trigger_page_fault(int index) {
    char cur_buff[1];
    memcpy((char*)data_ptr_ + index * PAGE_SIZE, cur_buff, 1);
    memset((char*)data_ptr_ + index * PAGE_SIZE, 0, PAGE_SIZE);
  }

  std::string mmap_base_dir_;
  void* data_ptr_;
  void* meta_ptr_;
  int data_fd_;
  int meta_fd_;
  static constexpr int MAX_MSG_LEN = 1024 * 1024 * 4;
  static constexpr int TOTAL_BYTES = sizeof(DataType) * MAX_MSG_LEN;
  static constexpr int META_BYTES = sizeof(uint64_t) * 2;
  size_t PAGE_SIZE = sysconf(_SC_PAGESIZE);
};

#endif
