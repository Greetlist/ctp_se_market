#ifndef MARKET_DATA_H_
#define MARKET_DATA_H_

// POD struct
struct FutureMarketData {
  double last_price;
  double settlement_price;
  double pre_close_price;
  double avg_price;
  double high;
  double open;
  double low;
  double close;
  double ask_price[5];
  double bid_price[5];
  double hold_volume;
  uint64_t vendor_time;
  uint64_t local_time;
  int ask_volume[5];
  int bid_volume[5];
  char exchange_id[8];
  char uid[8];
  char vendor_update_time[16];
};

#endif
