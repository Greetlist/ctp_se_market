#include "mmap_writer.h"

MMapWriter<DataType>::MMapWriter(const std::string& mmap_base_dir) : mmap_base_dir_(mmap_base_dir) {
}

MMapWriter<DataType>::~MMapWriter() {
  close(data_fd_);
  close(meta_fd_);
}

bool MMapWriter<DataType>::Init() {
  data_fd = open(mmap_base_dir + "market.data", O_RDWR, 0777);
  if (data_fd_ < 0) {
    perror("open mmap error");
    return false;
  }

  meta_fd = open(mmap_base_dir + "market.meta", O_RDWR, 0777);
  if (data_fd_ < 0) {
    perror("open meta error");
    return false;
  }

  mmap_ptr_ = (struct mmap_struct*) mmap(NULL, sizeof(DataType) * MAX_MSG_LEN, PROT_READ|PROT_WRITE, MAP_SHARED, data_fd_, 0);
  constexpr int total_mmap_size = sizeof(DataType) * MAX_MSG_LEN;
  constexpr int total_page_count = toal_mmap_size / PAGE_SIZE + 1;
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

void MMapWriter<DataType>::Prefault(const DataType&& data) {
  memcpy(mmap_ptr_, &data, sizeof(DataType));
  mmap_ptr_ += sizeof(DataType);
}

void MMapWriter<DataType>::manual_trigger_page_fault(char* page_start_addr) {
  char* c = page_start_addr;
  memcpy(page_start_addr, c, 1);
}
