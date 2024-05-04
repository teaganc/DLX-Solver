#include "sudoku_driver_test.h"

#include <format>
#include <iostream>
#include <vector>

void SudokuDriverTest::validateGetEmptyTopIndex() {
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      for (int k = 0; k < 9; k++) {
        if (sudoku_driver->getEmptyTopIndex(i, j, k, 0) !=
            sudoku_driver->getEmptyTopIndex(i, j, 0)) {
          std::cout << std::format(
              "Failed top index: i->{} j->{} k->{} l->{}\n", i, j, k, 0);
        }
      }
    }
  }
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      for (int k = 0; k < 9; k++) {
        if (sudoku_driver->getEmptyTopIndex(i, j, k, 1) !=
            sudoku_driver->getEmptyTopIndex(i, k, 1)) {
          std::cout << std::format(
              "Failed top index: i->{} j->{} k->{} l->{}\n", i, j, k, 1);
        }
      }
    }
  }

  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      for (int k = 0; k < 9; k++) {
        if (sudoku_driver->getEmptyTopIndex(i, j, k, 2) !=
            sudoku_driver->getEmptyTopIndex(j, k, 2)) {
          std::cout << std::format(
              "Failed top index: i->{} j->{} k->{} l->{}\n", i, j, k, 2);
        }
      }
    }
  }
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      for (int k = 0; k < 9; k++) {
        if (sudoku_driver->getEmptyTopIndex(i, j, k, 3) !=
            sudoku_driver->getEmptyTopIndex(3 * (i / 3) + j / 3, k, 3)) {
          std::cout << std::format(
              "Failed top index: i->{} j->{} k->{} l->{}\n", i, j, k, 3);
        }
      }
    }
  }
}

void SudokuDriverTest::printVNodes() {
  for (int i = 0; i < sudoku_driver->vnodes_owner.size(); i++) {
    if (sudoku_driver->vnodes_owner[i].top == nullptr) {
      std::cout << "\n\n";
      auto x = sudoku_driver->option_map[&sudoku_driver->vnodes_owner[i + 1]];
      std::cout << "i: " << std::get<0>(x) << " j: " << std::get<1>(x)
                << " k: " << std::get<2>(x) << "\n";
    }
    std::cout << "i: " << i << " t: "
              << sudoku_driver->vnodes_owner[i].top - sudoku_driver->vnodes
              << " u: "
              << sudoku_driver->vnodes_owner[i].up - sudoku_driver->vnodes
              << " d: "
              << sudoku_driver->vnodes_owner[i].down - sudoku_driver->vnodes
              << "    ";
  }
  std::cout << "\n";
}

void SudokuDriverTest::validateNodes() {
  Dlx dlx;
  dlx.hnodes = sudoku_driver->hnodes;
  dlx.vnodes = sudoku_driver->vnodes;
  for (int i = 1; i < sudoku_driver->hnodes_owner.size(); i++) {
    int index = 0;
    Dlx::VNode *top =
        &sudoku_driver
             ->vnodes[&sudoku_driver->hnodes[i] - sudoku_driver->hnodes];
    Dlx::VNode *current = top->up;
    std::vector<Dlx::VNode *> l;
    while (current != top) {
      l.push_back(current);
      current = current->up;
      index++;
      if (index > 100) {
        std::cout << "FAILED to up validate node: " << &sudoku_driver->vnodes[i]
                  << "\n";
        for (auto j : l) {
          std::cout << j << " ";
        }
        std::cout << "\n";
        break;
      }
    }
  }

  for (int i = 1; i < sudoku_driver->hnodes_owner.size(); i++) {
    int index = 0;
    Dlx::VNode *top =
        &sudoku_driver
             ->vnodes[&sudoku_driver->hnodes[i] - sudoku_driver->hnodes];
    Dlx::VNode *current = top->down;
    std::vector<Dlx::VNode *> l;
    while (current != top) {
      l.push_back(current);
      current = current->down;
      index++;
      if (index > 100) {
        std::cout << "FAILED to down validate node: " << i << "\n";
        for (auto j : l) {
          std::cout << j - sudoku_driver->vnodes << " ";
        }
        std::cout << "\n";
        return;
      }
    }
  }
  std::cout << "Validated nodes\n";
}
