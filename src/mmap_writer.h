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
  MMapWriter(const std::string& mmap_base_dir);
  ~MMapWriter();
  bool Init();
  void Prefault();
  void WriteData(const DataType& data);
private:
  std::string mmap_base_dir_;
  struct mmap_struct* mmap_ptr_;
  static constexpr int MAX_MSG_LEN = 20000000;
};

#endif
