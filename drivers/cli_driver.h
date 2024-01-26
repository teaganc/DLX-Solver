#pragma once 

#include "../dlx.h"

#include <string>
#include <unordered_map>

class CliDriver : public Dlx::Driver {
public:
  std::vector<Dlx::HNode> hnodes_safe;
  std::vector<Dlx::VNode> vnodes_safe;

  std::unordered_map<Dlx::VNode*, std::string> options;

  std::string generateNodes(std::istream& in);
  std::string generate(int argc, char** argv);

  void prettyPrintSolution(const std::vector<Dlx::VNode*> solution);
};
