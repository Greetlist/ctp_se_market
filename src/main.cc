#include <iostream>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <string>
#include <chrono>
#include <thread>

#include "ctp_se_market_receiver.h"
#include "mmap_reader.h"

DEFINE_string(mmap_base_dir, "", "mmap base dir");
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
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
  } else if (FLAGS_mode == std::string{"reader"}) {
    MMapReader<FutureMarketData>* reader = new MMapReader<FutureMarketData>(FLAGS_mmap_base_dir);
    while (reader->ReadData()) {}
  }
  return 0;
}
