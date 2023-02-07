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
    assert(scopes.empty());
    scopes.pop_back();
  }

  ScopeType& getTopScope() { return scopes.back(); }
  auto getNumScopes() { return scopes.size(); }

  std::optional<Value> find(Key& key) {
    for (auto scope = scopes.rbegin(); scope != scopes.rend(); ++scope) {
      if (auto* value = scope->find(key)) {
        return std::optional<Value>(&value);
      }
    }
    return std::optional<Value>{};
  };

  void add(Key& key, Value& value) {
    auto& scope = scopes.back();
    scope.insert(key, value);
  }

  private:
  std::list<ScopeType> scopes{};
};
} // namespace mcool::type