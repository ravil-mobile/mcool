#pragma once

#include "ast.h"
#include <unordered_map>
#include <memory>
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

  template<typename Type>
  std::enable_if_t<std::is_same_v<Type, ast::String> || std::is_same_v<Type, ast::ObjectId>, Type*>
  getStringNode(const std::string& str) {
    std::unordered_map<std::string, std::unique_ptr<Type>>* table;
    if constexpr(std::is_same_v<Type, ast::String>) {
      table = &stringTable;
    }
    else {
      table = &objectIdTable;
    }
    if (table->find(str) == table->end()) {
      table->insert({str, std::make_unique<Type>(str)});
    }
    return (table->operator[](str)).get();
  }

  ast::Int* getIntNode(const int& integer);
  ast::Bool* getBoolNode(const bool& boolean);

private:
  MemoryManagement() = default;

  std::unordered_map<std::string, std::unique_ptr<ast::String>> stringTable{};
  std::unordered_map<std::string, std::unique_ptr<ast::ObjectId>> objectIdTable{};
  std::unordered_map<int, std::unique_ptr<ast::Int>> integerTable{};
  std::unordered_map<bool, std::unique_ptr<ast::Bool>> booleanTable{};
  std::vector<void*> memory{};
};

template<typename Type, typename ...Args>
Type* make(Args... args) {
  auto& mm = MemoryManagement::getInstance();
  if constexpr(std::is_same_v<Type, ast::String> || std::is_same_v<Type, ast::ObjectId>) {
    return mm.getStringNode<Type>(args...);
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
