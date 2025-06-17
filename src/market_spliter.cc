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
  MMapReader<FutureMarketData>* reader = new MMapReader<FutureMarketData>(mmap_base_dir_);
  if (!reader->Init()) {
    LOG(ERROR) << "Init Reader Error";
    return false;
  }
  LOG(INFO) << "Struct Size: " << reader->GetStructSize() << ", data len: " << reader->GetDataCount();

  std::vector<std::string> inst_code_vec = GetInstVec();
  for (auto& inst : inst_code_vec) {
    std::time_t now = std::time({});
    char time_str[std::size("yyyy-mm-dd")];
    std::strftime(std::data(time_str), std::size(time_str), "%F", std::gmtime(&now));
    std::string output_file_name = output_base_dir_ + "/" + inst + "." + std::string{time_str} + ".csv";
    writers_[inst] = new CsvWriter<FutureMarketData>(output_file_name);
    writers_[inst]->SetHeader();
  }
  reader_ = new MMapReader<FutureMarketData>(mmap_base_dir_);
  return reader_->Init();
}

void MarketSpliter::Split() {
  FutureMarketData* data = nullptr;
  while ((data = reader_->ReadData()) != nullptr) {
    std::string uid{data->uid};
    writers_[uid]->WriteData(*data);
  }
}

std::vector<std::string> MarketSpliter::GetInstVec() {
  CsvReader reader(secinfo_file_);
  return reader.ReadColumnByIndex(0);
}
