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

ast::String* MemoryManagement::getStringNode(const std::string& str) {
  if (stringTable.find(str) == stringTable.end()) {
    stringTable[str] = new ast::String(str);
  }
  return dynamic_cast<ast::String*>(stringTable[str]);
}

ast::ObjectId* MemoryManagement::getObjectIdNode(const std::string& str) {
  if (stringTable.find(str) == stringTable.end()) {
    stringTable[str] = new ast::ObjectId(str);
  }
  return dynamic_cast<ast::ObjectId*>(stringTable[str]);
}


ast::Int* MemoryManagement::getIntNode(const int& integer) {
  if (integerTable.find(integer) == integerTable.end()) {
    integerTable[integer] = new ast::Int(integer);
  }
  return integerTable[integer];
}

ast::Bool* MemoryManagement::getBoolNode(const bool& boolean) {
  if (booleanTable.find(boolean) == booleanTable.end()) {
    booleanTable[boolean] = new ast::Bool(boolean);
  }
  return booleanTable[boolean];
}

} // namespace mcool