#include "cli_parser.h"

void CliParser::addOption(std::string flags, std::string* out) {
  int start = 0;
  for (int end = 1; end <= flags.size(); end++) {
    if (flags[end] == '\0' || flags[end] == ',') {
      options.emplace(std::string({&flags[start], &flags[end]}), out);
      start = end + 1;
      end = end + 2;
    }
  }
}

std::string CliParser::parse(int argc, char** argv) {
  int i = 1;
  while(i < argc) {
    const std::string flag = std::string(argv[i]);
    auto iter = options.find(flag);
    if (iter == options.cend()) {
      return "Invalid parameter '" + flag + "'\n";
    }

    if (std::holds_alternative<bool*>(iter->second)) {
      *std::get<bool*>(iter->second) = true;
      ++i;
    }
    else {
      if (++i >= argc) {
        return "Missing argument to flag "  + iter->first + "\n";
      }
      *std::get<std::string*>(iter->second) = std::string(argv[i]); 
      ++i;
    }
  }
  return {};
}
