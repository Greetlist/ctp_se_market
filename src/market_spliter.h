#ifndef MARKET_SPLITER_H_
#define MARKET_SPLITER_H_

#include <ctime>
#include <string>
#include <vector>
#include <unordered_map>

#include "market_data.h"
#include "mmap_reader.h"
#include "csv_writer.h"
#include "csv_reader.h"

class MarketSpliter {
public:
  MarketSpliter(const std::string& secinfo_file, const std::string& mmap_base_dir, const std::string& output_base_dir);
  ~MarketSpliter();
  bool Init();
  void Split();
private:
  std::vector<std::string> GetInstVec();
  std::string secinfo_file_;
  std::string mmap_base_dir_;
  std::string output_base_dir_;
  std::unordered_map<std::string, CsvWriter<FutureMarketData>*> writers_;
  MMapReader<FutureMarketData>* reader_;
};

#endif
