#include "ini_reader.h"

INIReader::INIReader(const std::string& config_file) : config_file_(config_file) {}

std::unordered_map<std::string, std::unordered_map<std::string, std::string>> INIReader::GetConfig() {
  std::unordered_map<std::string, std::string> res;
  std::ifstream input(config_file_);
  if (!input.is_open()) {
    std::cout << "Open [" << config_file_ << "] Error, Exit" << std::endl;
    std::exit(1);
  }
  while (true) {
    std::string line;
    std::getline(input, line);

    line.erase(
        std::remove_if(
            line.begin(), line.end(),
            [](unsigned char x) { return std::isspace(x); }
        ),
        str.end()
    );

    if (line.size() == 0 || line[0] == ';') { //is empty line or line is comment
      continue;
    }
    if (line[0] == '[') {
      auto last_it = line.find_last_of(']');
      if (last_it != std::npos) {
        LOG(ERROR) << "INI file [" << config_file_ << "] is invalid! Please check file. Exit";
        exit(-1);
      }
      std::string map_key(line.begin()+1, last_it);
      if (res.find(map_key) == res.end()) {
        res[map_key] = std::unordered_map<std::string, std::string>();
      }
      continue;
    }

    std::stringstream line_stream(line);
    std::string key, value;
    int cell_index = 0;
    std::string target;
    while (std::getline(line_stream, cell, '=')) {
      if (cell_index == 0) {
        key = cell;
      } else {
        value = cell;
      }
      cell_index++;
    }
    res[] = ;
    if (input.eof()) {
      break;
    }
  }
  return res;
}
