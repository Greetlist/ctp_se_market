#include "market_spliter.h"

MarketSpliter::MarketSpliter(const std::string& secinfo_file, const std::string& mmap_base_dir, const std::string& output_base_dir) : secinfo_file_(secinfo_file), mmap_base_dir_(mmap_base_dir), output_base_dir_(output_base_dir), reader_(nullptr) {}

MarketSpliter::~MarketSpliter() {
  if (reader_) {
    delete reader_;
    reader_ = nullptr;
  }

  for (auto& [inst, ptr] : writers_) {
    delete ptr;
  }
}

bool MarketSpliter::Init() {
  reader_ = new MMapReader<FutureMarketData>(mmap_base_dir_);
  if (!reader_->Init()) {
    LOG(ERROR) << "Init Reader Error";
    return false;
  }
  LOG(INFO) << "Struct Size: " << reader_->GetStructSize() << ", data len: " << reader_->GetDataCount();

  std::vector<std::string> inst_code_vec = GetInstVec();
  for (auto& inst : inst_code_vec) {
    std::time_t now = std::time({});
    char time_str[std::size("yyyy-mm-dd")];
    std::strftime(std::data(time_str), std::size(time_str), "%F", std::gmtime(&now));
    std::string output_file_name = output_base_dir_ + "/" + inst + "." + std::string{time_str} + ".csv";
    writers_[inst] = new CsvWriter<FutureMarketData>(output_file_name);
    writers_[inst]->SetHeader();
  }
  return true;
}

void MarketSpliter::Split() {
  std::pair<FutureMarketData, bool> p;
  while (true) {
    p = reader_->ReadData();
    if (!p.second) {
      break;
    }
    FutureMarketData& data = p.first;
    std::string uid{data.uid};
    writers_[uid]->WriteData(data);
  }
}

std::vector<std::string> MarketSpliter::GetInstVec() {
  CsvReader reader(secinfo_file_);
  return reader.ReadColumnByIndex(0);
}
