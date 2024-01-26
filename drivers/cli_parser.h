#pragma once

#include <string>
#include <variant>
#include <unordered_map>

struct CliParser {
  std::unordered_map<std::string, std::variant<std::string*, bool*>> options;

  void addOption(std::string flags, std::string* out);

  std::string parse(int argc, char** argv);
};
