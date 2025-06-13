#include "ctp_se_market_receiver.h"

CtpSeMarketReceiver::CtpSeMarketReceiver(const std::string& config_file, const std::string& secinfo_file) : config_file_(config_file), secinfo_file_(secinfo_file) {
}

CtpSeMarketReceiver::~CtpSeMarketReceiver() {
  ctp_api_->Release();
  delete market_writer_;
  delete csv_reader_;
}

bool CtpSeMarketReceiver::Init() {
  InitConfig();
  csv_reader_ = new CsvReader(secinfo_file_);
  market_writer_ = new MMapWriter<FutureMarketData>(config_map_["mmap_base_dir"]);
  ctp_api_ = CThostFtdcMdApi::CreateFtdcMdApi("/home/greetlist/github_project/ctp_se_market/flow", false, false);
  ctp_api_->RegisterSpi(this);
  ctp_api_->RegisterFront(const_cast<char*>(config_map_["front_addr"].c_str()));
  ctp_api_->Init();
  if (!CheckAction(LOGIN)) {
    LOG(ERROR) << "Login Timeout";
    return false;
  }
  return login_.load();
}

void CtpSeMarketReceiver::OnFrontConnected() {
  LOG(INFO) << "OnFrontConnected, Start to Login";
  CThostFtdcReqUserLoginField login_req;
  strncpy(login_req.BrokerID, config_map_["broker_id"].c_str(), config_map_["broker_id"].size());
  strncpy(login_req.UserID, config_map_["user_id"].c_str(), config_map_["user_id"].size());
  strncpy(login_req.Password, config_map_["password"].c_str(), config_map_["password"].size());
  //strncpy(login_req.MacAddress, config_map_["mac"].c_str(), config_map_["mac"].size());
  //login_req.ClientIPPort = std::stoi(config_map_["port"]);
  //strncpy(login_req.ClientIPAddress, config_map_["ip_addr"].c_str(), config_map_["ip_addr"].size());
  ctp_api_->ReqUserLogin(&login_req, req_id_++);
}

void CtpSeMarketReceiver::OnFrontDisconnected(int reason) {
  LOG(ERROR)
    << "OnFrontDisconnected, Reason code: ["
    << reason << "]";
}

void CtpSeMarketReceiver::OnRspUserLogin(CThostFtdcRspUserLoginField *login_field, CThostFtdcRspInfoField *info, int req_id, bool is_last) {
  if (login_field == nullptr || info == nullptr) {
    LOG(ERROR) << "Login field is nullptr";
    return;
  }
  std::lock_guard<std::mutex> l(mutex_vec_[LOGIN]);
  LOG(INFO) << "Login Success.";
  login_ = true;
  cv_vec_[LOGIN].notify_one();
}

void CtpSeMarketReceiver::OnRspUserLogout(CThostFtdcUserLogoutField * logout_field, CThostFtdcRspInfoField *info, int req_id, bool is_last) {
  if (logout_field == nullptr || info == nullptr) {
    LOG(ERROR) << "Login field is nullptr";
    return;
  }
  LOG(INFO) << "Logout Success.";
  login_ = false;
}

std::vector<std::string> CtpSeMarketReceiver::GetInstVec() {
  return csv_reader_->ReadColumnByIndex(0);
}

void CtpSeMarketReceiver::Subscribe(const std::vector<std::string>&& inst_vec) {
  int inst_size = inst_vec.size();
  char* subscribe_inst_arr[inst_size];
  int i = 0;
  for (const std::string& uid : inst_vec) {
    LOG(INFO) << "Subscribe: " << uid;
    subscribe_inst_arr[i++] = const_cast<char*>(uid.c_str());
  }
  ctp_api_->SubscribeMarketData(subscribe_inst_arr, inst_size);
  if (!CheckAction(SUBSCRIBE_INST)) {
    LOG(ERROR) << "Subscribe Timeout.";
  }
  LOG(INFO) << "Subscribe All inst code Success.";
}

void CtpSeMarketReceiver::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *inst_field, CThostFtdcRspInfoField *info, int req_id, bool is_last) {
  if (inst_field == nullptr || info == nullptr) {
    LOG(ERROR) << "Subscribe Error";
  }
  LOG(INFO) << "Subscribe " << inst_field->InstrumentID << " Success.";
  if (info != nullptr && info->ErrorID != 0) {
    LOG(ERROR) << "Error id: " << info->ErrorID << ", Error Info: " << info->ErrorMsg;
  }
  if (is_last) {
    cv_vec_[SUBSCRIBE_INST].notify_one();
  }
}

void CtpSeMarketReceiver::OnRspError(CThostFtdcRspInfoField *info, int req_id, bool is_last) {
  LOG(ERROR) << "OnRspError";
}

void CtpSeMarketReceiver::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *market_data) {
  LOG(INFO) << "CtpSeMarketReceiver::OnRtnDepthMarketData";
  FutureMarketData data;
  memset(&data, 0, sizeof(FutureMarketData));
  data.last_price = market_data->LastPrice;
  data.settlement_price = market_data->SettlementPrice;
  data.pre_close_price = market_data->PreClosePrice;
  data.avg_price = market_data->AveragePrice;
  data.high = market_data->HighestPrice;
  data.open = market_data->OpenPrice;
  data.low = market_data->LowestPrice;
  data.close = market_data->ClosePrice;

  data.ask_price[0] = market_data->AskPrice1;
  data.ask_price[1] = market_data->AskPrice2;
  data.ask_price[2] = market_data->AskPrice3;
  data.ask_price[3] = market_data->AskPrice4;
  data.ask_price[4] = market_data->AskPrice5;
  data.ask_volume[0] = market_data->AskVolume1;
  data.ask_volume[1] = market_data->AskVolume2;
  data.ask_volume[2] = market_data->AskVolume3;
  data.ask_volume[3] = market_data->AskVolume4;
  data.ask_volume[4] = market_data->AskVolume5;

  data.hold_volume = market_data->OpenInterest;

  data.bid_price[0] = market_data->BidPrice1;
  data.bid_price[1] = market_data->BidPrice2;
  data.bid_price[2] = market_data->BidPrice3;
  data.bid_price[3] = market_data->BidPrice4;
  data.bid_price[4] = market_data->BidPrice5;
  data.bid_volume[0] = market_data->BidVolume1;
  data.bid_volume[1] = market_data->BidVolume2;
  data.bid_volume[2] = market_data->BidVolume3;
  data.bid_volume[3] = market_data->BidVolume4;
  data.bid_volume[4] = market_data->BidVolume5;

  data.vendor_time = static_cast<uint64_t>(market_data->UpdateMillisec);
  auto ts = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  data.local_time = static_cast<uint64_t>(ts);

  strcpy(data.exchange_id, market_data->ExchangeID);
  strcpy(data.uid, market_data->InstrumentID);

  market_writer_->Prefault(std::move(data));
}

void CtpSeMarketReceiver::InitConfig() {
  config_map_["mmap_base_dir"] = "/home/greetlist/github_project/ctp_se_market/mmap/";
  config_map_["front_addr"] = "tcp://180.168.146.187:10211";
  config_map_["broker_id"] = "9999";
  config_map_["user_id"] = "242911";
  config_map_["password"] = "";
  config_map_["mac"] = "48210b34fdec";
  config_map_["port"] = "40200";
  config_map_["ip_addr"] = "192.168.18.123";
}

bool CtpSeMarketReceiver::CheckAction(int action) {
  std::unique_lock<std::mutex> l(mutex_vec_[action]);
  auto res = cv_vec_[action].wait_for(l, std::chrono::seconds(timeout_));
  if (res == std::cv_status::timeout) {
    return false;
  }
  return true;
}
