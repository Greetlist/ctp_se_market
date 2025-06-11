#ifndef CSV_READER_H_
#define CSV_READER_H_
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

enum class Exchange {
  SH = 1,
  SZ,
  BJ,
  Unknown,
};

using ExchangeMap = std::unordered_multimap<Exchange, std::string>;

class CsvReader {
public:
  CsvReader(const std::string&&);
  ~CsvReader() = default;
  std::vector<std::string> ReadColumnByIndex(int index);
private:
  std::string csv_path_;
};

#endif
