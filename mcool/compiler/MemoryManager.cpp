#include "MemoryManager.h"

namespace mcool {
  MemoryManager::~MemoryManager() {
  for (auto ptr : memory) {
    std::free(ptr);
  }
}

ast::Int* MemoryManager::getIntNode(const int& integer) {
  if (integerTable.find(integer) == integerTable.end()) {
    integerTable.insert({integer, std::make_unique<ast::Int>(integer)});
  }
  return integerTable[integer].get();
}

ast::Bool* MemoryManager::getBoolNode(const bool& boolean) {
  if (booleanTable.find(boolean) == booleanTable.end()) {
    booleanTable.insert({boolean, std::make_unique<ast::Bool>(boolean)});
  }
  return booleanTable[boolean].get();
}
} // namespace mcool
