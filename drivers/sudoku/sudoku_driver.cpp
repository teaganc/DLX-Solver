#include "sudoku_driver.h"

#include <algorithm>
#include <iostream>
#include <vector>

int SudokuDriver::generatePuzzle(const std::string &puzzle_) {
  if (puzzle_.size() != 81) {
    return -1;
  }

  const std::vector<bool> precover = generatePrecover(puzzle_);

  hnodes_owner.reserve(9 * 9 * 4 + 1);
  vnodes_owner.reserve(9 * 9 * 4 + 1 + 9 * 9 * 9 * 5 + 1);

  const std::unordered_map<int, int> header_map =
      generateHeaders(precover, hnodes_owner, vnodes_owner);
  generateOptions(vnodes_owner, header_map);

  hnodes = hnodes_owner.data();
  vnodes = vnodes_owner.data();
  solution_size = hnodes_owner.size();
  puzzle = puzzle_;

  return 0;
}

std::vector<bool> SudokuDriver::generatePrecover(const std::string &puzzle) {
  std::vector<bool> precover;
  precover.resize(9 * 9 * 4 + 1);

  const char char_map[9] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
  const int char_map_size = sizeof(char_map) / sizeof(char_map[0]);
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      auto iter =
          std::find(char_map, char_map + char_map_size, puzzle[i * 9 + j]);
      int k = iter - char_map;
      if (k == char_map_size) {
        continue;
      }

      for (int l = 0; l < 4; l++) {
        precover[getEmptyTopIndex(i, j, k, l)] = true;
      }
    }
  }
  return precover;
}

std::unordered_map<int, int>
SudokuDriver::generateHeaders(const std::vector<bool> &precover,
                              std::vector<Dlx::HNode> &hnodes,
                              std::vector<Dlx::VNode> &vnodes) {
  std::unordered_map<int, int> header_map;

  hnodes.emplace_back(nullptr, &hnodes[1]);
  vnodes.emplace_back(nullptr, nullptr, nullptr);
  int index = 1;
  for (int l = 0; l < 4; l++) {
    for (int i = 0; i < 9; i++) {
      for (int j = 0; j < 9; j++) {
        if (!precover[getEmptyTopIndex(i, j, l)]) {
          hnodes.emplace_back(&hnodes[index - 1], &hnodes[index + 1]);
          vnodes.emplace_back(0, &vnodes[index], &vnodes[index]);
          header_map[getEmptyTopIndex(i, j, l)] = index;
          index++;
        }
      }
    }
  }
  hnodes.back().right = &hnodes[0];
  hnodes.front().left = &hnodes[index - 1];

  return header_map;
}

bool SudokuDriver::shouldSkipOption(
    const std::unordered_map<int, int> &header_map, int i, int j, int k) const {
  for (int l = 0; l < 4; l++) {
    if (!header_map.contains(getEmptyTopIndex(i, j, k, l))) {
      return true;
    }
  }
  return false;
}

void SudokuDriver::generateOptions(
    std::vector<Dlx::VNode> &vnodes,
    const std::unordered_map<int, int> &header_map) {
  int index = vnodes.size();
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      for (int k = 0; k < 9; k++) {
        if (shouldSkipOption(header_map, i, j, k)) {
          continue;
        }

        vnodes.emplace_back(nullptr, &vnodes[index - 4], &vnodes[index + 4]);
        index++;
        for (int l = 0; l < 4; l++) {
          int top_index = header_map.at(getEmptyTopIndex(i, j, k, l));
          Dlx::VNode *top = &vnodes[top_index];
          Dlx::VNode *bottom = top->up;
          vnodes.emplace_back(top, bottom, top);
          top->up = &vnodes[index];
          bottom->down = &vnodes[index];
          option_map[&vnodes[index]] = {i, j, k};
          index++;
        }
      }
    }
  }
  vnodes.emplace_back(nullptr, &vnodes[index - 4], &vnodes[index + 4]);
}

int SudokuDriver::getEmptyTopIndex(int i, int j, int k, int l) const {
  switch (l) {
  case 0:
    return i * 9 + j + 1;
  case 1:
    return 81 + i * 9 + k + 1;
  case 2:
    return 2 * 81 + j * 9 + k + 1;
  case 3:
    int x = 3 * (i / 3) + (j / 3);
    return 3 * 81 + x * 9 + k + 1;
  }
  return -1;
}

int SudokuDriver::getEmptyTopIndex(int a, int b, int l) const {
  switch (l) {
  case 0:
    return getEmptyTopIndex(a, b, 0, l);
  case 1:
    return getEmptyTopIndex(a, 0, b, l);
  case 2:
    return getEmptyTopIndex(0, a, b, l);
  case 3:
    return getEmptyTopIndex(0, a * 3, b, l);
  }
  return -1;
}

std::string
SudokuDriver::translateSolution(const std::vector<Dlx::VNode *> &solution) {
  for (auto i : solution) {
    auto x = option_map.at(i);
    puzzle[std::get<0>(x) * 9 + std::get<1>(x)] = '1' + std::get<2>(x);
  }
  return puzzle;
}

void SudokuDriver::prettyPrintSolution(
    const std::vector<Dlx::VNode *> &solution) {
  std::string s = translateSolution(solution);
  for (int i = 0; i < 9; i++) {
    if ((i % 3) == 0) {
      std::cout << "\n";
    }
    std::cout << "\n";
    for (int j = 0; j < 9; j++) {
      if ((j % 3) == 0) {
        std::cout << " ";
      }
      std::cout << s[i * 9 + j] << " ";
    }
  }
  std::cout << "\n\n\n";
}

#ifdef SUDOKU_MAIN_IMPL

#include "sudoku_driver_test.h"
#include <iostream>

inline void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char c) {
            return !std::isspace(c);
          }));
}

inline void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [](unsigned char c) { return !std::isspace(c); })
              .base(),
          s.end());
}

inline void trim(std::string &s) {
  rtrim(s);
  ltrim(s);
}

int main() {
  std::string s;
  std::cin >> s;
  trim(s);

  Dlx dlx;
  SudokuDriver sudoku_driver;

  if (sudoku_driver.generatePuzzle(s) != 0) {
    std::cerr << "Failed to generate driver\n";
    return -1;
  }

  auto solution = dlx.solve(&sudoku_driver);

  if (solution.size() == 0) {
    std::cerr << "Failed to find a solution\n";
    return -1;
  }

  sudoku_driver.prettyPrintSolution(solution);

  return 0;
}

#endif
