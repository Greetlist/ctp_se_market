#ifndef MMAP_READER_H_
#define MMAP_READER_H_

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string>
#include <glog/logging.h>

template<class DataType>
class MMapReader {
public:
  MMapReader(const std::string& mmap_base_dir) : mmap_base_dir_(mmap_base_dir), data_ptr_(nullptr), meta_ptr_(nullptr), data_fd_(0), meta_fd_(0), read_index_(0) {}
  ~MMapReader() {
    close(data_fd_);
    close(meta_fd_);
  }

  bool Init() {
    // for data ptr
    data_fd_ = open((mmap_base_dir_ + std::string{"market.data"}).c_str(), O_RDONLY);
    if (data_fd_ < 0) {
      perror("open data error");
      return false;
    }
    data_ptr_ = mmap(NULL, TOTAL_BYTES, PROT_READ, MAP_SHARED, data_fd_, 0);
    if (data_ptr_ == MAP_FAILED) {
      perror("data mmap error");
      return false;
    }

    // for meta ptr
    meta_fd_ = open((mmap_base_dir_ + std::string{"market.meta"}).c_str(), O_RDONLY);
    if (meta_fd_ < 0) {
      perror("open meta error");
      return false;
    }
    meta_ptr_ = mmap(NULL, META_BYTES, PROT_READ, MAP_SHARED, meta_fd_, 0);
    if (meta_ptr_ == MAP_FAILED) {
      perror("meta mmap error");
      return false;
    }
    return true;
  }

  uint64_t GetStructSize() {
    uint64_t* ptr = (uint64_t*)meta_ptr_;
    return *ptr;
  }

  uint64_t GetDataCount() {
    uint64_t* ptr = (uint64_t*)((char*)meta_ptr_ + sizeof(uint64_t));
    return *ptr;
  }

  bool ReadData() {
    if (read_index_ >= GetDataCount()) {
      LOG(INFO) << "Read Finish, Total Record Count is: " << read_index_;
      return false;
    }
    DataType* data = (DataType*)((char*)data_ptr_ + (sizeof(DataType) * read_index_));
    LOG(INFO) << "Uid: " << data->uid << ", Local Time: " << data->local_time << ", Vendor Time: " << data->vendor_time;
    read_index_++;
    return true;
  }
private:
  std::string mmap_base_dir_;
  void* data_ptr_ = nullptr;
  void* meta_ptr_ = nullptr;
  int data_fd_;
  int meta_fd_;
  uint64_t read_index_;

  static constexpr int MAX_MSG_LEN = 1024 * 1024 * 4;
  static constexpr int TOTAL_BYTES = sizeof(DataType) * MAX_MSG_LEN;
  static constexpr int META_BYTES = sizeof(uint64_t) * 2;
};

#endif
