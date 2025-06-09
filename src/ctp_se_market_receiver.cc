#include "src/ctp_se_market_receiver.h"

CtpSeMarketReceiver::CtpSeMarketReceiver() {
}

CtpSeMarketReceiver::~CtpSeMarketReceiver() {
}

void CtpSeMarketReceiver::OnFrontConnected() {}

void CtpSeMarketReceiver::OnFrontDisconnected(int nReason) {
}

void CtpSeMarketReceiver::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
}

void CtpSeMarketReceiver::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
}

void CtpSeMarketReceiver::OnRspQryMulticastInstrument(CThostFtdcMulticastInstrumentField *pMulticastInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
}

void CtpSeMarketReceiver::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
}

void CtpSeMarketReceiver::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
}

void CtpSeMarketReceiver::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) {
}
