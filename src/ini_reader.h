#ifndef INI_READER_H_
#define INI_READER_H_

#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <ffstream>
#include <algorithm>
#include <glog/logging.h>

class INIReader {
public:
  INIReader(const std::string& config_file);
  ~INIReader() = default;
  std::unordered_map<std::string, std::unordered_map<std::string, std::string>> GetConfig();
private:
  std::string config_file_;
};

#endif
