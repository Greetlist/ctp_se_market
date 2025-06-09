#include "src/ctp_se_market_receiver.h"

CtpSeMarketReceiver::CtpSeMarketReceiver(const std::string& config_file) : config_file_(config_file) {
}

CtpSeMarketReceiver::~CtpSeMarketReceiver() {
  ctp_api_->Release();
  delete market_writer_;
}

bool CtpSeMarketReceiver::Init() {
  ctp_api_->CThostFtdcMdApi::CreateFtdcMdApi("", false, false);
  ctp_api_->RegisterSpi(this);
  ctp_api_>RegisterFront(config_map_["front_addr"]);
  ctp_api_->Init();
  CThostFtdcReqUserLoginField login_req;
  login_req.BrokerID = config_map_["broker_id"];
  login_req.UserID = config_map_["user_id"];
  login_req.Password = config_map_["password"];
  login_req.MacAddress = config_map_["mac"];
  login_req.ClientIPPort = config_map_["port"];
  login_req.ClientIPAddress = config_map_["ip_addr"];
  ctp_api_->ReqUserLogin(&login_req, req_id_++);
  std::unique_lock<std::mutex> l(login_mutex_);
  auto res = login_cv_.wait_for(l, std::chrono::system_clock::seconds(login_timeout_));
  if (res == std::cv_status::timeout) {
    LOG(ERROR) << "Login Timeout";
    login_ = false;
    return false;
  }
  return login_.load();
}

void CtpSeMarketReceiver::QueryInstCode() {
  ctp_api_->ReqQryMulticastInstrument(CThostFtdcQryMulticastInstrumentField *pQryMulticastInstrument, req_id_++);
}

void CtpSeMarketReceiver::Subscribe() {

}

void CtpSeMarketReceiver::OnFrontConnected() {
  LOG(INFO) << "OnFrontConnected";
}

void CtpSeMarketReceiver::OnFrontDisconnected(int reason) {
  LOG(ERROR)
    << "OnFrontDisconnected, Reason code: [";
    << reason << "]";
}

void CtpSeMarketReceiver::OnRspUserLogin(CThostFtdcRspUserLoginField *login_filed, CThostFtdcRspInfoField *info, int req_id, bool is_last) {
  std::lock_guard<std::mutex> l(login_mutex_);
  LOG(INFO) << "Login Success.";
  login_ = true;
  login_cv_.notify_one();
}

void CtpSeMarketReceiver::OnRspUserLogout(CThostFtdcUserLogoutField * logout_feild, CThostFtdcRspInfoField *info, int req_id, bool is_last) {
  LOG(INFO) << "Login Success.";
  login_ = false;
}

void CtpSeMarketReceiver::OnRspQryMulticastInstrument(CThostFtdcMulticastInstrumentField *inst_field, CThostFtdcRspInfoField *info, int req_id, bool is_last) {
}

void CtpSeMarketReceiver::OnRspError(CThostFtdcRspInfoField *info, int req_id, bool is_last) {
}

void CtpSeMarketReceiver::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *inst_field, CThostFtdcRspInfoField *info, int req_id, bool is_last) {
}

void CtpSeMarketReceiver::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *market_data) {
}
