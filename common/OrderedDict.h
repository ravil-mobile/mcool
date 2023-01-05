#pragma once

// Taken from: https://caffe2.ai/doxygen-c/html/ordered__dict_8h_source.html

#include <cstdint>
#include <functional>
#include <initializer_list>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <cassert>


namespace mcool {
  template <typename Key, typename Value>
  class OrderedDict {
  public:
    class Item;

    using Iterator = typename std::vector<Item>::iterator;
    using ConstIterator = typename std::vector<Item>::const_iterator;

    explicit OrderedDict();

    OrderedDict(const OrderedDict& other);
    OrderedDict& operator=(const OrderedDict& other);

    OrderedDict(OrderedDict&& other) = default;
    OrderedDict& operator=(OrderedDict&& other) = default;

    OrderedDict(std::initializer_list<Item> initializer_list);

    ~OrderedDict() = default;

    // Element Access
    Item& front();
    const Item& front() const;

    Item& back();
    const Item& back() const;

    Item& operator[](size_t index);
    const Item& operator[](size_t index) const;
    Value& operator[](const Key& key);
    const Value& operator[](const Key& key) const;

    // Lookup
    Value* find(const Key& key) noexcept;
    const Value* find(const Key& key) const noexcept;
    bool contains(const Key& key) const noexcept;

    // Iterators
    Iterator begin();
    ConstIterator begin() const;
    Iterator end();
    ConstIterator end() const;

    // Capacity
    size_t size() const noexcept;
    bool is_empty() const noexcept;
    void reserve(size_t requested_capacity);

    // Modifiers
    template <typename K, typename V>
    Value& insert(K&& key, V&& value);
    Value& insert(Key key, Value&& value);
    void update(OrderedDict&& other);
    void update(const OrderedDict& other);
    void clear();

    // Observers
    const std::vector<Item>& items() const noexcept;

    std::vector<Key> keys() const;
    std::vector<Value> values() const;
    std::vector<std::pair<Key, Value>> pairs() const;

  private:
    std::unordered_map<Key, size_t> index_;
    std::vector<Item> items_;
  };

  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ OrderedDict::Item ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  template <typename Key, typename Value>
  class OrderedDict<Key, Value>::Item {
  public:
    Item(Key key, Value value) : pair_(std::move(key), std::move(value)) {}

    Value& operator*() { return value(); }
    const Value& operator*() const { return value(); }
    Value* operator->() { return &value(); }
    const Value* operator->() const { return &value(); }
    const Key& key() const noexcept { return pair_.first; }
    Value& value() noexcept { return pair_.second; }
    const Value& value() const noexcept { return pair_.second; }
    const std::pair<const Key, Value>& pair() const noexcept { return pair_; }

  private:
    std::pair<const Key, Value> pair_;
  };

  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ OrderedDict ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  template <typename Key, typename Value>
  OrderedDict<Key, Value>::OrderedDict() {}

  template <typename Key, typename Value>
  OrderedDict<Key, Value>::OrderedDict(const OrderedDict& other)
      : index_(other.index_) {
    // Copy we have to do ourselves, because items' keys are const, so we have to
    // re-insert the items.
    for (const auto& item : other.items_) {
      items_.push_back(item);
    }
  }

  template <typename Key, typename Value>
  OrderedDict<Key, Value>& OrderedDict<Key, Value>::operator=(
      const OrderedDict& other) {
    index_ = other.index_;
    items_.clear();
    for (auto& item : other.items_) {
      items_.push_back(item);
    }

    return *this;
  }

  template <typename Key, typename Value>
  OrderedDict<Key, Value>::OrderedDict(
      std::initializer_list<Item> initializer_list) {
    items_.reserve(initializer_list.size());
    for (auto& item : initializer_list) {
      // Copy the key here and move it into the index.
      items_.emplace_back(item.key(), std::move(item.value()));
      index_.emplace(std::move(item.key()), size() - 1);
    }
  }

  template <typename Key, typename Value>
  typename OrderedDict<Key, Value>::Iterator OrderedDict<Key, Value>::begin() {
    return items_.begin();
  }

  template <typename Key, typename Value>
  typename OrderedDict<Key, Value>::ConstIterator OrderedDict<Key, Value>::begin()
  const {
    return items_.begin();
  }

  template <typename Key, typename Value>
  typename OrderedDict<Key, Value>::Iterator OrderedDict<Key, Value>::end() {
    return items_.end();
  }

  template <typename Key, typename Value>
  typename OrderedDict<Key, Value>::ConstIterator OrderedDict<Key, Value>::end()
  const {
    return items_.end();
  }

  template <typename Key, typename Value>
  typename OrderedDict<Key, Value>::Item& OrderedDict<Key, Value>::front() {
    assert(!items_.empty() && "Called front() on an empty OrderedDict");
    return items_.front();
  }

  template <typename Key, typename Value>
  const typename OrderedDict<Key, Value>::Item& OrderedDict<Key, Value>::front()
  const {
    assert(!items_.empty() && "Called front() on an empty OrderedDict");
    return items_.front();
  }

  template <typename Key, typename Value>
  typename OrderedDict<Key, Value>::Item& OrderedDict<Key, Value>::back() {
    assert(!items_.empty() && "Called back() on an empty OrderedDict");
    return items_.back();
  }

  template <typename Key, typename Value>
  const typename OrderedDict<Key, Value>::Item& OrderedDict<Key, Value>::back()
  const {
    assert(!items_.empty() && "Called back() on an empty OrderedDict");
    return items_.back();
  }

  template <typename Key, typename Value>
  typename OrderedDict<Key, Value>::Item& OrderedDict<Key, Value>::operator[](
      size_t index) {
    assert(index < items_.size() && "Index is out of bounds");
    return items_[index];
  }

  template <typename Key, typename Value>
  const typename OrderedDict<Key, Value>::
  Item& OrderedDict<Key, Value>::operator[](size_t index) const {
    assert(index < items_.size() && "Index is out of bounds");
    return items_[index];
  }

  template <typename Key, typename Value>
  Value& OrderedDict<Key, Value>::operator[](const Key& key) {
    if (auto* value = find(key)) {
      return *value;
    }
    assert("key is not defined");
  }

  template <typename Key, typename Value>
  const Value& OrderedDict<Key, Value>::operator[](const Key& key) const {
    if (auto* value = find(key)) {
      return *value;
    }
    assert("key is not defined");
  }

  template <typename Key, typename Value>
  template <typename K, typename V>
  Value& OrderedDict<Key, Value>::insert(K&& key, V&& value) {
    assert(index_.count(key) == 0 && "key is already defined");

    // Copy `key` here and move it into the index.
    items_.emplace_back(key, std::forward<V>(value));
    index_.emplace(std::forward<K>(key), size() - 1);
    return items_.back().value();
  }

  template <typename Key, typename Value>
  Value& OrderedDict<Key, Value>::insert(Key key, Value&& value) {
    return insert<Key, Value>(std::move(key), std::move(value));
  }

  template <typename Key, typename Value>
  void OrderedDict<Key, Value>::update(OrderedDict&& other) {
    reserve(size() + other.size());
    for (auto& item : other) {
      // We want to call `insert()` to prevent duplicate keys.
      insert(std::move(item.key()), std::move(item.value()));
    }
  }

  template <typename Key, typename Value>
  void OrderedDict<Key, Value>::update(const OrderedDict& other) {
    reserve(size() + other.size());
    for (auto& item : other) {
      // We want to call `insert()` to prevent duplicate keys.
      insert(item.key(), item.value());
    }
  }

  template <typename Key, typename Value>
  Value* OrderedDict<Key, Value>::find(const Key& key) noexcept {
    auto iterator = index_.find(key);
    if (iterator == index_.end()) {
      return nullptr;
    }
    return &items_[iterator->second].value();
  }

  template <typename Key, typename Value>
  const Value* OrderedDict<Key, Value>::find(const Key& key) const noexcept {
    auto iterator = index_.find(key);
    if (iterator == index_.end()) {
      return nullptr;
    }
    return &items_[iterator->second].value();
  }

  template <typename Key, typename Value>
  bool OrderedDict<Key, Value>::contains(const Key& key) const noexcept {
    return find(key) != nullptr;
  }

  template <typename Key, typename Value>
  void OrderedDict<Key, Value>::clear() {
    index_.clear();
    items_.clear();
  }

  template <typename Key, typename Value>
  size_t OrderedDict<Key, Value>::size() const noexcept {
    return items_.size();
  }

  template <typename Key, typename Value>
  bool OrderedDict<Key, Value>::is_empty() const noexcept {
    return items_.empty();
  }

  template <typename Key, typename Value>
  const std::vector<typename OrderedDict<Key, Value>::Item>& OrderedDict<
      Key,
      Value>::items() const noexcept {
    return items_;
  }

  template <typename Key, typename Value>
  std::vector<Key> OrderedDict<Key, Value>::keys() const {
    std::vector<Key> keys;
    keys.reserve(size());
    for (const auto& item : items_) {
      keys.push_back(item.key());
    }
    return keys;
  }

  template <typename Key, typename Value>
  std::vector<Value> OrderedDict<Key, Value>::values() const {
    std::vector<Value> values;
    values.reserve(size());
    for (const auto& item : items_) {
      values.push_back(item.value());
    }
    return values;
  }

  template <typename Key, typename Value>
  std::vector<std::pair<Key, Value>> OrderedDict<Key, Value>::pairs() const {
    std::vector<std::pair<Key, Value>> values;
    values.reserve(size());
    for (const auto& item : items_) {
      values.push_back(item.pair());
    }
    return values;
  }

  template <typename Key, typename Value>
  void OrderedDict<Key, Value>::reserve(size_t requested_capacity) {
    index_.reserve(requested_capacity);
    items_.reserve(requested_capacity);
  }

} // namespace mcool