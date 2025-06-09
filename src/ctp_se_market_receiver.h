#ifndef CTP_SE_MARKET_RECEIVER_H_
#define CTP_SE_MARKET_RECEIVER_H_

#include <string>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <chrono>

#include "ThostFtdcMdApi.h"
#include "mmap_writer.h"
#include "market_data.h"

class CtpSeMarketReceiver : public CThostFtdcMdSpi {
public:
  CtpSeMarketReceiver(const std::string& config_file);
  ~CtpSeMarketReceiver();

  bool Init();
  void QueryInstCode();
  void Subscribe();

  void OnFrontConnected() override;
  void OnFrontDisconnected(int nReason) override;
  void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  void OnRspQryMulticastInstrument(CThostFtdcMulticastInstrumentField *pMulticastInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) override;

  static constexpr int login_timeout_ = 10; //seconds;
private:
  std::string config_file_;
  std::unordered_map<std::string, std::string> config_map_;
  std::condition_variable login_cv_;
  std::mutex login_mutex_;
  std::atomic<bool> login_ = false;
  std::atomic<int> req_id_ = 0;
  MMapWriter<FutureMarketData>* market_writer_ = nullptr;
  CThostFtdcMdApi* ctp_api_ = nullptr;
};

#endif
