#include "MemoryManagement.h"

namespace mcool {
MemoryManagement::~MemoryManagement() {
  for (auto ptr : memory) {
    std::free(ptr);
  }
}

MemoryManagement& MemoryManagement::getInstance() {
  static MemoryManagement instance;
  return instance;
}

ast::Int* MemoryManagement::getIntNode(const int& integer) {
  if (integerTable.find(integer) == integerTable.end()) {
    integerTable.insert({integer, std::make_unique<ast::Int>(integer)});
  }
  return integerTable[integer].get();
}

ast::Bool* MemoryManagement::getBoolNode(const bool& boolean) {
  if (booleanTable.find(boolean) == booleanTable.end()) {
    booleanTable.insert({boolean, std::make_unique<ast::Bool>(boolean)});
  }
  return booleanTable[boolean].get();
}

} // namespace mcool