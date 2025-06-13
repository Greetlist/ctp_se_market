#ifndef CTP_SE_MARKET_RECEIVER_H_
#define CTP_SE_MARKET_RECEIVER_H_

#include <string>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <chrono>
#include <vector>
#include <glog/logging.h>

#include "ThostFtdcMdApi.h"
#include "ThostFtdcTraderApi.h"
#include "mmap_writer.h"
#include "csv_reader.h"
#include "market_data.h"

enum Action {
  LOGIN = 0,
  SUBSCRIBE_INST = 1,
  ACTION_NUM = 2
};

class CtpSeMarketReceiver : public CThostFtdcMdSpi {
public:
  CtpSeMarketReceiver(const std::string& config_file, const std::string& secinfo_file);
  ~CtpSeMarketReceiver();

  bool Init();
  std::vector<std::string> GetInstVec();
  void Subscribe(const std::vector<std::string>&& inst_vec);

  void OnFrontConnected() override;
  void OnFrontDisconnected(int nReason) override;
  void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) override;

  static constexpr int timeout_ = 10; //seconds;
private:
  void InitConfig();
  bool CheckAction(int action);
  //bool filter_inst_code(const std::string& inst_code); //if needed
  std::string config_file_;
  std::string secinfo_file_;
  std::unordered_map<std::string, std::string> config_map_;
  std::atomic<bool> login_ = false;
  std::atomic<int> req_id_ = 0;
  MMapWriter<FutureMarketData>* market_writer_ = nullptr;
  CsvReader* csv_reader_;
  CThostFtdcMdApi* ctp_api_ = nullptr;

  std::condition_variable cv_vec_[ACTION_NUM];
  std::mutex mutex_vec_[ACTION_NUM];
  std::vector<std::string> exchange_vec_{"CFFEX", "CZCE", "SHFE", "INE", "DCE", "GFEX"};
  std::vector<std::string> uid_vec_;
};

#endif
