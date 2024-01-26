#include "cli_driver.h"
#include "cli_parser.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::string CliDriver::generateNodes(std::istream& in) {
  std::unordered_map<std::string, Dlx::VNode*> items; 
  {
    std::string s;
    std::getline(in, s);

    hnodes_safe.emplace_back(nullptr, &hnodes_safe[1]);
    vnodes_safe.emplace_back(nullptr, nullptr, nullptr);

    int index = 1;
    auto i = s.cbegin();
    while (i < s.cend()) {
      auto j = std::find_if(i, s.cend(), [] (auto k) { return std::isspace(k); });
      hnodes_safe.emplace_back(&hnodes_safe[index - 1], &hnodes_safe[index + 1]);
      vnodes_safe.emplace_back(0, &vnodes_safe[index], &vnodes_safe[index]);
      items[{i, j}] = &vnodes_safe[index];
      i = j + 1;
      index++;
    }

    hnodes_safe.back().right = &hnodes_safe[0];
    hnodes_safe.front().left = &hnodes_safe[index - 1];
  }

  // repeatedly get line and create the spacer and options for each line
  int index = vnodes_safe.size();
  Dlx::VNode* prev_spacer = nullptr;
  std::string s;
  while (std::getline(in, s)) {
    if (s.size() == 0) {
      break;
    }

    vnodes_safe.emplace_back(nullptr, prev_spacer + 1, nullptr);
    prev_spacer = &vnodes_safe.back();
    index++;

    auto i = s.cbegin();
    while (i < s.cend()) {
      auto j = std::find_if(i, s.cend(), [](auto k) { return std::isspace(k); });
      std::string t(i, j);
      i = j + 1;
      
      auto item = items.find(t);
      if (item == items.end()) {
        return "Error item: " + t + " not in input\n";
      }

      Dlx::VNode* top = item->second;
      Dlx::VNode* bottom = top->up;
      vnodes_safe.emplace_back(top, bottom, top);

      Dlx::VNode* current = &vnodes_safe[index];
      top->up = current;
      bottom->down = current;

      options[current] = s;

      index++;
    }
    prev_spacer->down = &vnodes_safe[index - 1];
  }
  
  vnodes_safe.emplace_back(nullptr, prev_spacer + 1, nullptr);

  return {};
}

std::string CliDriver::generate(int argc, char** argv) {
  CliParser parser;
  std::string items_count;
  std::string options_count;
  std::string in_filename;

  if (argc == 1) {
    return "Usage: -nh <int> -nv <int> [-f <input-filename>]\n";
  }

  parser.addOption("-nh,--item-count", &items_count);
  parser.addOption("-nv,--option-count", &options_count);
  parser.addOption("-f,--input-file", &in_filename);
  
  std::string error = parser.parse(argc, argv);

  if (!error.empty()) {
    return error;
  }
  
  try {
    std::size_t nh, nv;
    nh = std::stoi(items_count, &nh);
    nv = std::stoi(options_count, &nv);

    hnodes_safe.reserve(nh);
    vnodes_safe.reserve(nv);
  }
  catch(std::exception e) {
    return "Failed to parse item count(-nh) and option count(-nv)\n";
  }

  if (in_filename.empty()) {
    error = generateNodes(std::cin);
  }
  else {
    // open file to istream
    std::filebuf fb;
    try {
      fb.open(in_filename, std::ios::in);
      std::istream in(&fb);
      error = generateNodes(in);
      fb.close();
    }
    catch (std::exception e) {
      return "Failed to open file: "  + in_filename + "\n";
    }
  }

  hnodes = hnodes_safe.data();
  vnodes = vnodes_safe.data();
  solution_size = vnodes_safe.size() - hnodes_safe.size();

  return error;
}

void CliDriver::prettyPrintSolution(const std::vector<Dlx::VNode*> solution) {
  for (auto i : solution) {
    std::cout << options.at(i) << "\n";
  }
}

#include <iostream>

int main(int argc, char** argv) {
  CliDriver driver;
  std::string s = driver.generate(argc, argv);

  if (s.size() != 0) {
    std::cerr << s;
    exit(1);
  }
  Dlx dlx;
  auto solution = dlx.solve(&driver);

  driver.prettyPrintSolution(solution);

  return 0;
}
