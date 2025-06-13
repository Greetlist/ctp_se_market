#include "ini_reader.h"

INIReader::INIReader(const std::string& config_file) : config_file_(config_file) {}

std::unordered_map<std::string, std::string> INIReader::GetConfig() {
  std::unordered_map<std::string, std::string> res;
  return res;
}
