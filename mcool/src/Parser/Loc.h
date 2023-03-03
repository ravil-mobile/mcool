#pragma once

#include <cstddef>
#include <string>
#include <cassert>

namespace mcool::ast {
class StringPtr;
} // namespace mcool::ast

namespace mcool {
struct Position {
  explicit Position() : line(0), column(0) {}
  explicit Position(int line, int column)
      : line(static_cast<size_t>(line)), column(static_cast<size_t>(column)) {}
  Position& operator=(const Position& other) = default;

  bool operator<(const Position& other) const {
    return line <= other.line && column < other.column;
  }

  bool operator>(const Position& other) const {
    return !(*this < other);
  }

  bool operator==(const Position& other) const {
    return line == other.line && column == other.column;
  }

  size_t line;
  size_t column;
};

struct Loc {
  explicit Loc() : begin(Position()), end(Position()), filename(nullptr) {}
  explicit Loc(Position begin, Position end, ast::StringPtr* filename)
      : begin(begin), end(end), filename(filename) {}
  explicit Loc(const Loc& first, const Loc& second);
  Loc& operator=(const Loc& other) = default;

  mcool::Position begin{};
  mcool::Position end{};
  mcool::ast::StringPtr* filename{nullptr};
};
} // namespace mcool
