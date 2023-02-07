#pragma once

#include <cstddef>
#include <string>

namespace mcool::ast {
class StringPtr;
} // namespace mcool::ast

namespace mcool {
struct Position {
  explicit Position() : line(0), column(0) {}
  explicit Position(int line, int column)
      : line(static_cast<size_t>(line)), column(static_cast<size_t>(column)) {}
  Position& operator=(const Position& other) = default;

  size_t line;
  size_t column;
};

struct Loc {
  explicit Loc() : begin(Position()), end(Position()), filename(nullptr) {}
  explicit Loc(Position begin, Position end, ast::StringPtr* filename)
      : begin(begin), end(end), filename(filename) {}
  Loc& operator=(const Loc& other) = default;

  mcool::Position begin{};
  mcool::Position end{};
  mcool::ast::StringPtr* filename{nullptr};
};
} // namespace mcool
