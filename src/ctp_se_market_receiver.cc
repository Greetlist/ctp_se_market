#include "ctp_se_market_receiver.h"

CtpSeMarketReceiver::CtpSeMarketReceiver(const std::string& config_file) : config_file_(config_file) {
}

CtpSeMarketReceiver::~CtpSeMarketReceiver() {
  ctp_api_->Release();
  delete market_writer_;
}

bool CtpSeMarketReceiver::Init() {
  market_writer_ = new MMapWriter<FutureMarketData>(config_map_["mmap_base_dir"]);
  ctp_api_->CThostFtdcMdApi::CreateFtdcMdApi("", false, false);
  ctp_api_->RegisterSpi(this);
  ctp_api_>RegisterFront(config_map_["front_addr"]);
  ctp_api_->Init();
  if (!check_action(LOGIN)) {
    LOG(ERROR) << "Login Timeout";
    return false;
  }
  return login_.load();
}

void CtpSeMarketReceiver::OnFrontConnected() {
  LOG(INFO) << "OnFrontConnected, Start to Login";
  CThostFtdcReqUserLoginField login_req;
  login_req.BrokerID = config_map_["broker_id"];
  login_req.UserID = config_map_["user_id"];
  login_req.Password = config_map_["password"];
  login_req.MacAddress = config_map_["mac"];
  login_req.ClientIPPort = config_map_["port"];
  login_req.ClientIPAddress = config_map_["ip_addr"];
  ctp_api_->ReqUserLogin(&login_req, req_id_++);
}

void CtpSeMarketReceiver::OnFrontDisconnected(int reason) {
  LOG(ERROR)
    << "OnFrontDisconnected, Reason code: [";
    << reason << "]";
}

void CtpSeMarketReceiver::OnRspUserLogin(CThostFtdcRspUserLoginField *login_field, CThostFtdcRspInfoField *info, int req_id, bool is_last) {
  if (login_field == nullptr || info == nullptr) {
    LOG(ERROR) << "Login field is nullptr";
    return;
  }
  std::lock_guard<std::mutex> l(login_mutex_);
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

void CtpSeMarketReceiver::QueryInstCode() {
  for (std::string& exchange : exchange_vec_) {
    LOG(INFO) << "Start to query Exchange Code: [" << exchange << "]";
    CThostFtdcQryInstrumentField req;
    memset(&req, 0, sizeof(CThostFtdcQryInstrumentField));
    strcpy(&req.ExchangeID, exchange.c_str(), exchange.size());
    ctp_api_->ReqQryInstrument(&req, req_id_++);

    if (!check_action(QUERY_INST)) {
      LOG(ERROR) << "Query [" << exchange << "], Timeout.";
      continue;
    }
  }
}

void CtpSeMarketReceiver::OnRspQryInstrument(CThostFtdcInstrumentField *inst_field, CThostFtdcRspInfoField *info, int req_id, bool is_last) {
  if (inst_field == nullptr || info == nullptr) {
    LOG(ERROR) << "Instrument Info is nullptr";
    return;
  }
  std::string uid = std::string{inst_field->InstrumentID} + "-" + std::string{inst_field->ExchangeInstID} + "-" + std::string{inst_field->ProductID};
  uid_vec_.push_back(std::move(uid));
  cv_vec_[QUERY_INST].notify_one();
}

void CtpSeMarketReceiver::Subscribe() {
  int inst_size = uid_vec_.size();
  char* subscribe_inst_arr[inst_size];
  int i = 0;
  for (const std::string& uid : uid_vec_) {
    subscribe_inst_arr[i++] = uid.c_str();
  }
  ctp_api_->SubscribeMarketData(subscribe_inst_arr, inst_size);
  if (!check_action(SUBSCRIBE_INST)) {
    LOG(ERROR) << "Subscribe Timeout.";
  }
}

void CtpSeMarketReceiver::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *inst_field, CThostFtdcRspInfoField *info, int req_id, bool is_last) {
  if (inst_field == nullptr || info == nullptr) {
    LOG(ERROR) << "Subscribe Error";
  }
  if (is_last) {
    cv_vec_[SUBSCRIBE_INST].notify_one();
  }
}

void CtpSeMarketReceiver::OnRspError(CThostFtdcRspInfoField *info, int req_id, bool is_last) {
  LOG(ERROR) << "OnRspError"
}

void CtpSeMarketReceiver::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *market_data) {
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

  strcpy(&data.exchange_id, market_data->ExchangeID);
  strcpy(&data.uid, market_data->InstrumentID);

  market_writer_->Prefault(std::move(data));
}

bool CtpSeMarketReceiver::check_action(int action) {
  std::unique_lock<std::mutex> l(mutex_vec_[action]);
  auto res = cv_vec_[action].wait_for(l, std::chrono::system_clock::seconds(timeout_));
  if (res == std::cv_status::timeout) {
    return false;
  }
}
