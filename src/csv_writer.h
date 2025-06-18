#ifndef CSV_WRITER_H_
#define CSV_WRITER_H_

#include <string>
#include <fstream>
#include <type_traits>

#include "market_data.h"

template <class DataType>
class CsvWriter {
public:
  CsvWriter(const std::string& output_file) : output_file_(output_file), ofs_(output_file, std::ios::out | std::ios::ate) {}
  ~CsvWriter() {
    ofs_.flush();
  }
  void SetHeader() {
    if constexpr (std::is_same_v<DataType, FutureMarketData>) {
      std::string header = "Uid,ExchangeId,LastPrice,High,Open,Low,Close,HoldVolume,LocalTime,VendorUpdateTime,VendorTimeMilliSeconds";
      ofs_ << header << '\n' << std::flush;
    }
  }

  void WriteData(const DataType& data) {
    if constexpr (std::is_same_v<DataType, FutureMarketData>) {
      ofs_ 
        << data.uid << "," << data.exchange_id << "," << data.last_price << ","
        << data.high << "," << data.open << "," << data.low << "," << data.close << ","
        << data.hold_volume << ","
        << data.local_time << "," << data.vendor_update_time << "," << data.vendor_time << '\n'
        << std::flush;
    }
  }
private:
  std::string output_file_;
  std::ofstream ofs_;
};

#endif
