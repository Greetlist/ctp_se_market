#ifndef CTP_SE_MARKET_RECEIVER_H_
#define CTP_SE_MARKET_RECEIVER_H_

#include "ThostFtdcMdApi.h"

#include "mmap_writer.h"

class CtpSeMarketReceiver : public CThostFtdcMdSpi {
public:
  CtpSeMarketReceiver();
  ~CtpSeMarketReceiver();
  
  void OnFrontConnected() override;
  void OnFrontDisconnected(int nReason) override;
  void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  void OnRspQryMulticastInstrument(CThostFtdcMulticastInstrumentField *pMulticastInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) override;
private:
  MMapWriter<
};

#endif
