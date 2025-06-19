#include <iostream>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <string>
#include <chrono>
#include <thread>

#include "ctp_se_market_receiver.h"
#include "mmap_reader.h"
#include "ini_reader.h"
#include "market_spliter.h"

DEFINE_string(mmap_base_dir, "", "mmap base dir");
DEFINE_string(output_base_dir, "./output", "csv output dir");
DEFINE_string(config_file_path, "", "config file");
DEFINE_string(secinfo_path, "", "instrument csv path");
DEFINE_string(mode, "", "recorder or reader");

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  if (FLAGS_mode == std::string{"recorder"}) {
    CtpSeMarketReceiver* r = new CtpSeMarketReceiver(FLAGS_mmap_base_dir, FLAGS_config_file_path, FLAGS_secinfo_path);
    if (!r->Init()) {
      return -1;
    }
    std::vector<std::string> inst_vec = r->GetInstVec();
    r->Subscribe(std::move(inst_vec));

    while (true) {
      std::time_t now = std::time({});
      char time_str[std::size("HH:MM:SS")];
      std::strftime(std::data(time_str), std::size(time_str), "%T", std::localtime(&now));
      LOG(INFO) << time_str;
      if (std::string{time_str} > std::string{"15:30:00"}) {
        LOG(INFO) << "End Of Trading Day, Exit.";
        break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
  } else if (FLAGS_mode == std::string{"reader"}) {
    MarketSpliter* ms = new MarketSpliter(FLAGS_secinfo_path, FLAGS_mmap_base_dir, FLAGS_output_base_dir);
    if (!ms->Init()) {
      LOG(ERROR) << "Init market spliter error";
      return -1;
    }
    ms->Split();
  } else if (FLAGS_mode == std::string{"test"}) {
    INIReader reader{FLAGS_config_file_path};
    auto m = reader.GetConfig();
    for (auto [section, config] : m) {
      std::cout << "Section: [" << section << "]" << std::endl;
      for (auto [key, value] : config) {
        std::cout << "Key: [" << key << "], Value: [" << value << "]" << std::endl;
      }
    }
  }
  return 0;
}
