#include <iostream>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <string>
#include <chrono>
#include <thread>

#include "ctp_se_market_receiver.h"

DEFINE_string(config_file_path, "", "config file");
DEFINE_string(secinfo_path, "", "instrument csv path");

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  CtpSeMarketReceiver* r = new CtpSeMarketReceiver(FLAGS_config_file_path, FLAGS_secinfo_path);
  if (!r->Init()) {
    return -1;
  }
  std::vector<std::string> inst_vec = r->GetInstVec();
  r->Subscribe(std::move(inst_vec));

  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  return 0;
}
