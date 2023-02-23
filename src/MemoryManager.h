#pragma once

#include "ast.h"
#include <unordered_map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>
#include <type_traits>

namespace mcool {
class MemoryManager {
  public:
  MemoryManager() = default;
  MemoryManager(MemoryManager&& other) = default;
  ~MemoryManager();

  template <typename Type, typename... Args>
  Type* make(Args... args);

  void obtain(void* ptr) { memory.push_back(ptr); }

  template <typename Type>
  std::enable_if_t<std::is_same_v<Type, ast::String> || std::is_same_v<Type, ast::TypeId> ||
                       std::is_same_v<Type, ast::ObjectId> || std::is_same_v<Type, ast::StringPtr>,
                   ast::StringPtr*>
      getStringPtr(std::string& str) {
    std::unordered_map<std::string, std::unique_ptr<ast::StringPtr>>* table;
    if constexpr (std::is_same_v<Type, ast::String> || std::is_same_v<Type, ast::TypeId>) {
      table = &staticStringTable;
    } else if constexpr (std::is_same_v<Type, ast::StringPtr>) {
      table = &rawStringTable;
    } else {
      table = &objectStringTable;
    }
    if (table->find(str) == table->end()) {
      table->insert({str, std::make_unique<ast::StringPtr>(str)});
    }
    return (table->operator[](str)).get();
  }

  ast::Int* getIntNode(const int& integer);
  ast::Bool* getBoolNode(const bool& boolean);

  auto getStaticStrings() {
    std::vector<std::string> strings{};
    for (auto& item : staticStringTable) {
      strings.push_back(item.first);
    }
    return strings;
  }

  private:
  std::unordered_map<std::string, std::unique_ptr<ast::StringPtr>> staticStringTable{};
  std::unordered_map<std::string, std::unique_ptr<ast::StringPtr>> rawStringTable{};
  std::unordered_map<std::string, std::unique_ptr<ast::StringPtr>> objectStringTable{};
  std::unordered_map<int, std::unique_ptr<ast::Int>> integerTable{};
  std::unordered_map<bool, std::unique_ptr<ast::Bool>> booleanTable{};
  std::vector<void*> memory{};
  int classTagCounter{0};
};

template <typename Type, typename... Args>
Type* MemoryManager::make(Args... args) {
  Type* ptr{nullptr};
  if constexpr (std::is_same_v<Type, ast::String> || std::is_same_v<Type, ast::TypeId> ||
                std::is_same_v<Type, ast::ObjectId>) {
    using FirstParamType = std::tuple_element_t<0, std::tuple<Args...>>;
    static_assert(sizeof...(args) == 1 && std::is_same_v<FirstParamType, std::string>,
                  "expected std::string as parameter");
    auto param = std::get<0>(std::make_tuple(args...));
    auto stringPtr = this->getStringPtr<Type>(param);
    ptr = new Type(stringPtr);
  } else if constexpr (std::is_same_v<Type, ast::Int>) {
    ptr = this->getIntNode(args...);
  } else if constexpr (std::is_same_v<Type, ast::Bool>) {
    ptr = this->getBoolNode(args...);
  } else if constexpr(std::is_same_v<Type, ast::CoolClass>) {
    ptr = new Type(args..., classTagCounter++);
    this->obtain(static_cast<void*>(ptr));
  } else if constexpr (sizeof...(args)) {
    ptr = new Type(args...);
    this->obtain(static_cast<void*>(ptr));
  } else {
    ptr = new Type();
    this->obtain(static_cast<void*>(ptr));
  }
  return ptr;
}
} // namespace mcool
