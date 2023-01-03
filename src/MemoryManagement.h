#pragma once

#include "ast.h"
#include <unordered_map>
#include <string>
#include <tuple>
#include <vector>
#include <type_traits>


namespace mcool {
class MemoryManagement {
public:
  ~MemoryManagement();
  static MemoryManagement& getInstance();

  void obtain(void* ptr) {
    memory.push_back(ptr);
  }

  ast::String* getStringNode(const std::string& str);
  ast::Int* getIntNode(const int& integer);
  ast::Bool* getBoolNode(const bool& boolean);

private:
  MemoryManagement() = default;

  std::unordered_map<std::string, ast::String*> stringTable{};
  std::unordered_map<int, ast::Int*> integerTable{};
  std::unordered_map<bool, ast::Bool*> booleanTable{};
  std::vector<void*> memory{};
};

template<typename Type, typename ...Args>
Type* make(Args... args) {
  auto& mm = MemoryManagement::getInstance();
  if constexpr(std::is_same_v<Type, ast::String>) {
    return mm.getStringNode(args...);
  }
  else if constexpr(std::is_same_v<Type, ast::Int>) {
    return mm.getIntNode(args...);
  }
  else if constexpr(std::is_same_v<Type, ast::Bool>) {
    return mm.getBoolNode(args...);
  }
  else if constexpr(sizeof...(args)) {
    Type *ptr = new Type(args...);
    mm.obtain(static_cast<void *>(ptr));
    return ptr;
  }
  else {
    Type *ptr = new Type();
    mm.obtain(static_cast<void *>(ptr));
    return ptr;
  }
}
} // namespace mcool
