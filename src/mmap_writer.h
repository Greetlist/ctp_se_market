#ifndef MMAP_WRITER_H_
#define MMAP_WRITER_H_

#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string>
#include <filesystem>
#include <iostream>

template<class DataType>
class MMapWriter {
public:
  MMapWriter(const std::string& mmap_base_dir) : mmap_base_dir_(mmap_base_dir), write_index_(0) {}
  ~MMapWriter() {
    msync(data_ptr_, TOTAL_BYTES, MS_SYNC);
    munmap(meta_ptr_, TOTAL_BYTES);
    close(data_fd_);
    close(meta_fd_);
  }

  bool Init() {
    std::string data_file = mmap_base_dir_ + std::string{"market.data"};
    std::string meta_file = mmap_base_dir_ + std::string{"market.meta"};
    if (!std::filesystem::exists(data_file) || !std::filesystem::exists(meta_file)) {
      std::filesystem::remove(data_file);
      std::filesystem::remove(meta_file);
      std::cout << "Start to Init mmap." << std::endl;
      return InitDataMMap(data_file) && InitMetaMMap(meta_file);
    }

    //read meta first ==> reset data offset ==> manual trigger rest page_fault(keep history data)
    meta_fd_ = open(meta_file.c_str(), O_RDWR, 0777);
    meta_ptr_ = mmap(NULL, META_BYTES, PROT_READ|PROT_WRITE, MAP_SHARED, meta_fd_, 0);

    data_fd_ = open(data_file.c_str(), O_RDWR, 0777);
    data_ptr_ = mmap(NULL, TOTAL_BYTES, PROT_READ|PROT_WRITE, MAP_SHARED, data_fd_, 0);
    write_index_ = *(uint64_t*)((char*)meta_ptr_ + sizeof(uint64_t));
    manual_trigger_page_fault();
    return true;
  }

  bool InitDataMMap(const std::string& data_file) {
    data_fd_ = open(data_file.c_str(), O_RDWR|O_CREAT|O_TRUNC, 0777);
    if (data_fd_ < 0) {
      perror("open mmap error");
      return false;
    }
    //Important !!!
    if (ftruncate(data_fd_, TOTAL_BYTES) < 0) {
      perror("ftruncate mmap error");
      return false;
    }
    data_ptr_ = mmap(NULL, TOTAL_BYTES, PROT_READ|PROT_WRITE, MAP_SHARED, data_fd_, 0);
    if (data_ptr_ == MAP_FAILED) {
      perror("open data mmap error");
      return false;
    }

    std::cout << "Start trigger page_fault" << std::endl;
    manual_trigger_page_fault();
    if (madvise(data_ptr_, TOTAL_BYTES, MADV_WILLNEED) < 0) {
      perror("madvise error");
      return false;
    }
    std::cout << "End of Init data mmap" << std::endl;
    return true;
  }

  bool InitMetaMMap(const std::string& meta_file) {
    meta_fd_ = open(meta_file.c_str(), O_RDWR|O_CREAT, 0777);
    if (meta_fd_ < 0) {
      perror("open meta error");
      return false;
    }

    //Important !!!
    if (ftruncate(meta_fd_, META_BYTES) < 0) {
      perror("ftruncate mmap error");
      return false;
    }

    meta_ptr_ = mmap(NULL, META_BYTES, PROT_READ|PROT_WRITE, MAP_SHARED, meta_fd_, 0);
    if (meta_ptr_ == MAP_FAILED) {
      perror("open meta mmap error");
      return false;
    }
    //write data struct size first
    uint64_t* write_ptr = (uint64_t*)meta_ptr_;
    memset(write_ptr, 0, META_BYTES);
    *write_ptr = sizeof(DataType);
    std::cout << "End of Init meta mmap" << std::endl;
    return true;
  }

  //write data to mmap
  //increase data counter in memory first
  //then increase data counter in meta file
  //notice memory order
  void Prefault(const DataType& data) {
    char* data_write_ptr = static_cast<char*>(data_ptr_) + (write_index_ * sizeof(DataType));
    memcpy(data_write_ptr, &data, sizeof(DataType));
    write_index_++;
    asm volatile("" ::: "memory"); //prevent compiler reorder
    asm volatile("mfence" ::: "memory"); //prevent cpu reorder
    uint64_t* write_ptr = (uint64_t*)((char*)meta_ptr_ + sizeof(uint64_t));
    (*write_ptr)++;
  }
private:
  void manual_trigger_page_fault() {
    char cur_buff[1];
    int total_page_count = TOTAL_BYTES / PAGE_SIZE;
    std::cout << "total_page: " << total_page_count << std::endl;
    for (int i = write_index_; i < total_page_count; ++i) {
      memcpy((char*)data_ptr_ + i * PAGE_SIZE, cur_buff, 1);
      memset((char*)data_ptr_ + i * PAGE_SIZE, 0, PAGE_SIZE);
    }
    std::cout << "end manual page fault" << std::endl;
  }

  std::string mmap_base_dir_;
  uint64_t write_index_;
  void* data_ptr_;
  void* meta_ptr_;
  int data_fd_;
  int meta_fd_;
  //size_t PAGE_SIZE = sysconf(_SC_PAGESIZE);
  static constexpr size_t PAGE_SIZE = 4096;
  static constexpr int MAX_MSG_LEN = 1024 * 1024 * 4;
  static constexpr int TOTAL_BYTES = ((sizeof(DataType) * MAX_MSG_LEN) / PAGE_SIZE + 1) * PAGE_SIZE;
  static constexpr int META_BYTES = sizeof(uint64_t) * 2;
};

#endif
