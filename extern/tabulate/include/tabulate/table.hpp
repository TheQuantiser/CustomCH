#pragma once
#include <algorithm>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace tabulate {

class Table {
 public:
  using Row = std::vector<std::string>;

  void add_row(const std::initializer_list<std::string> &row) {
    rows_.emplace_back(row);
  }

  void add_row(const Row &row) { rows_.push_back(row); }

  friend std::ostream &operator<<(std::ostream &os, const Table &table) {
    if (table.rows_.empty()) return os;
    std::size_t cols = 0;
    for (const auto &row : table.rows_) cols = std::max(cols, row.size());
    std::vector<std::size_t> widths(cols, 0);
    for (const auto &row : table.rows_) {
      for (std::size_t c = 0; c < row.size(); ++c) {
        widths[c] = std::max(widths[c], row[c].size());
      }
    }
    for (std::size_t r = 0; r < table.rows_.size(); ++r) {
      const auto &row = table.rows_[r];
      for (std::size_t c = 0; c < cols; ++c) {
        const std::string &cell = c < row.size() ? row[c] : std::string();
        os << std::left << std::setw(static_cast<int>(widths[c] + 2)) << cell;
      }
      if (r + 1 != table.rows_.size()) os << '\n';
    }
    return os;
  }

 private:
  std::vector<Row> rows_;
};

}  // namespace tabulate

