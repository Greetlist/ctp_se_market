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
  double ask[5];
  double bid[5];
  int hold_volume;
  int ask_volume[5];
  int bid_volume[5];
  char exchange_id[8];
  char uid[8];
};

#endif
