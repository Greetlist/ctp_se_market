#include "ini_reader.h"

INIReader::INIReader(const std::string& config_file) : config_file_(config_file) {}

std::unordered_map<std::string, std::unordered_map<std::string, std::string>> INIReader::GetConfig() {
  std::unordered_map<std::string, std::unordered_map<std::string, std::string>> res;
  std::ifstream input(config_file_);
  if (!input.is_open()) {
    LOG(ERROR) << "Open [" << config_file_ << "] Error, Exit";
    std::exit(1);
  }
  while (true) {
    std::string line;
    std::getline(input, line);
    if (input.eof()) {
      break;
    }

    line.erase(
        std::remove_if(
            line.begin(), line.end(),
            [](unsigned char x) { return std::isspace(x); }
        ),
        line.end()
    );

    if (line.size() == 0 || line[0] == ';') { //is empty line or line is comment
      continue;
    }
    if (line[0] == '[') {
      auto last_it = line.find_last_of(']');
      if (last_it == std::string::npos) {
        LOG(ERROR) << "INI file [" << config_file_ << "] is invalid! Please check file. Exit";
        exit(-1);
      }
      line.erase(
          std::remove_if(
              line.begin(), line.end(),
              [](unsigned char x) { return x == '[' || x == ']'; }
          ),
          line.end()
      );
      if (res.find(line) == res.end()) {
        res[line] = std::unordered_map<std::string, std::string>();
        last_section_ = line;
      }
      continue;
    }

    std::stringstream line_stream(line);
    std::string cell, key, value;
    int cell_index = 0;
    std::string target;
    while (std::getline(line_stream, cell, '=')) {
      if (cell_index == 0) {
        key = cell;
      } else {
        value = cell;
      }
      if (cell_index++ >= 1) {
        break;
      }
    }
    res[last_section_][key] = value;
  }
  return res;
}
