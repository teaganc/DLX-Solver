#pragma once

#include "../dlx.h"

#include <string>
#include <unordered_map>

class SudokuDriverTest;

class SudokuDriver : public Dlx::Driver {
public:
  std::vector<Dlx::HNode> hnodes_owner;
  std::vector<Dlx::VNode> vnodes_owner;
  std::unordered_map<Dlx::VNode *, std::tuple<int, int, int>> option_map;
  std::string puzzle;

  int generatePuzzle(const std::string &puzzle);
  std::vector<bool> generatePrecover(const std::string &puzzle);
  std::unordered_map<int, int>
  generateHeaders(const std::vector<bool> &precover,
                  std::vector<Dlx::HNode> &hnodes,
                  std::vector<Dlx::VNode> &vnodes);
  bool shouldSkipOption(const std::unordered_map<int, int> &header_map, int i,
                        int j, int k) const;
  void generateOptions(std::vector<Dlx::VNode> &vnodes,
                       const std::unordered_map<int, int> &header_map);
  int getEmptyTopIndex(int i, int j, int k, int l) const;
  int getEmptyTopIndex(int a, int b, int l) const;

  std::string translateSolution(const std::vector<Dlx::VNode *> &solution);
  void prettyPrintSolution(const std::vector<Dlx::VNode *> &solution);

  friend class SudokuDriverTest;
};
