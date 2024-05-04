#pragma once
#include "sudoku_driver.h"

class SudokuDriverTest {
public:
  SudokuDriver *sudoku_driver;
  SudokuDriverTest(SudokuDriver *sudoku_driver_)
      : sudoku_driver(sudoku_driver_) {}

  void validateGetEmptyTopIndex();
  void printHNodes();
  void printVNodes();
  void printHeaderMap(const std::unordered_map<int, int> &header_map);
  void validateNodes();
};
