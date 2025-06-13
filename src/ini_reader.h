#ifndef INI_READER_H_
#define INI_READER_H_

#include <string>
#include <unordered_map>

class INIReader {
public:
  INIReader(const std::string& config_file);
  ~INIReader() = default;
  std::unordered_map<std::string, std::string> GetConfig();
private:
  std::string config_file_;
};


#endif
