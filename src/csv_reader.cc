#include "csv_reader.h"

CsvReader::CsvReader(const std::string& csv_path) : csv_path_(csv_path) {}

std::vector<std::string> CsvReader::ReadColumnByIndex(int index) {
  std::vector<std::string> res;
  std::ifstream input(csv_path_);
  if (!input.is_open()) {
    std::cout << "Open [" << csv_path_ << "] Error, Exit" << std::endl;
    std::exit(1);
  }

  int line_index = 0;
  while (true) {
    std::string line;
    std::getline(input, line);
    std::stringstream line_stream(line);
    std::string cell;
    if (line_index++ == 0) continue;

    int cell_index = 0;
    std::string target;
    while (std::getline(line_stream, cell, ',')) {
      if (cell_index == index) {
        res.push_back(cell);
        break;
      }
      cell_index++;
    }
    if (input.eof()) {
      break;
    }
  }
  return res;
}
