#include "mmap_writer.h"

MMapWriter::MMapWriter(const std::string& mmap_base_dir) : mmap_base_dir_()mmap_base_dir {
}

MMapWriter::~MMapWriter() {
}

bool MMapWriter::Init() {
  int fd = open(MMAP_FILE, O_RDWR, 0777);
  if (fd < 0) {
    perror("open error");
    return false;
  }

  mmap_ptr_ = (struct mmap_struct*) mmap(NULL, sizeof(struct mmap_struct), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
  if (mmap_ptr_ == MAP_FAILED) {
    perror("mmap error");
    return false;
  }
}

void MMapWriter::Prefault() {

}

void MMapWriter::WriteData(const DataType& data) {

}
