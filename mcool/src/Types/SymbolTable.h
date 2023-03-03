#pragma once

#include "OrderedDict.h"
#include <list>
#include <optional>
#include <cassert>

namespace mcool::type {
template <typename Key, typename Value>
class SymbolTable {
  using ScopeType = mcool::OrderedDict<Key, Value>;

  public:
  SymbolTable() { scopes.push_back(ScopeType()); }

  void pushScope() { scopes.push_back(ScopeType()); }
  void popScope() {
    assert(not scopes.empty());
    scopes.pop_back();
  }

  ScopeType& getTopScope() { return scopes.back(); }
  auto getNumScopes() { return scopes.size(); }

  std::optional<Value> lookup(const Key& key) {
    for (auto scope = scopes.rbegin(); scope != scopes.rend(); ++scope) {
      if (auto* value = scope->find(key)) {
        return *value;
      }
    }
    return std::optional<Value>{};
  };

  void add(Key& key, Value& value) {
    auto& scope = scopes.back();
    scope.insert(key, value);
  }

  void add(const Key& key, Value& value) {
    auto& scope = scopes.back();
    scope.insert(key, value);
  }

  std::optional<Value> print(std::ostream& stream) {
    size_t level{0};
    for (auto scope = scopes.begin(); scope != scopes.end(); ++scope, ++level) {
      stream << "level: " << level << '\n';
      auto keys = scope->keys();
      for (auto key : keys) {
        if (std::is_same_v<Key, std::string>) {
          stream << "key: " << key;
        }
        if (std::is_same_v<Value, Type*> || std::is_same_v<Value, MethodType*>) {
          auto* value = (*scope)[key];
          stream << "; value: " << value->getAsString();
        }
        stream << '\n';
      }
    }
  }

  auto begin() { return scopes.begin(); }
  auto end() { return scopes.end(); }

  private:
  std::list<ScopeType> scopes{};
};
} // namespace mcool::type