//
// Created by Eli Michaud on 6/4/2025.
//

#ifndef SPARSE_SET_H
#define SPARSE_SET_H
#include <ranges>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

/// Abstract sparse set interface for runtime polymorphism.
class ISparseSet {
public:
  virtual ~ISparseSet() = default;
  /** Remove an element by its id. */
  virtual bool Remove(std::size_t id) = 0;

  /** Remove all keys and elements */
  virtual void Clear() noexcept = 0;

  /** True if the list contains the specified element */
  [[nodiscard]] virtual bool Contains(std::size_t id) const = 0;

  /** Returns the number of elements in the list */
  [[nodiscard]] virtual std::size_t Size() const = 0;

  /// True if the list is empty
  [[nodiscard]] virtual bool Empty() const = 0;
};

/**
 * A sparsely populated set. Efficiently stores elements where the indexes may be far apart.
 * Insertion, removal, and retrieval have average O(1) complexity.
 * @tparam KeyType The "key" or index type
 * @tparam ValueType The type of stored objects
 */
template <std::integral KeyType, typename ValueType>
class SparseSet : public ISparseSet {
private:
  using DenseMap = std::vector<ValueType>;
  using SparseMap = std::unordered_map<KeyType, std::size_t>;
  using SparseMapRvs = std::unordered_map<std::size_t, KeyType>;

  DenseMap dense_;
  SparseMap sparse_; // Map entity id to index in dense list
  SparseMapRvs sparse_reverse_; // Map index in dense list to entity id

  /* TODO: Pagination instead of unordered map for better time complexity?
   * Would make it actually O(1) instead of "average" O(1),
   * but would it actually be significantly faster for the amount of elements we have? */

public:
  using KeyView = decltype(std::views::keys(std::declval<const SparseMap &>()));
  using ValueView = decltype(std::views::all(std::declval<const DenseMap &>()));
  using EntryView = decltype(std::views::zip(std::declval<KeyView>(), std::declval<ValueView>()));

  /// Returns a const view of all the keys in the set.
  KeyView GetKeyView() const { return std::views::keys(sparse_); }

  ValueView GetValueView() const { return std::views::all(dense_); }

  EntryView GetEntryView() const {
    KeyView key_view =
        std::views::iota(0u, dense_.size()) |
        std::views::transform([&](std::size_t i) -> KeyType { return sparse_reverse_.at(i); });

    ValueView val_view = std::views::all(dense_);

    return std::views::zip(key_view, val_view);
  }

  /** Add or override an element at the specified index. */
  void Insert(const KeyType id, ValueType element) {
    if (sparse_.contains(id)) {
      dense_[sparse_[id]] = element; // Override
      return;
    }

    const std::size_t index = dense_.size();
    sparse_[id] = index;
    sparse_reverse_[index] = id;
    dense_.push_back(element);
  }

  /**
   * Get an element from the list
   * @param id The index of the element
   * @return A reference to the element
   * @throws std::out_of_range if there is no element with that ID.
   */
  ValueType &Get(const KeyType id) {
    if (!sparse_.contains(id)) {
      throw std::out_of_range("No such element with id:" + std::to_string(id));
    }

    std::size_t index = sparse_.at(id);
    return dense_[index];
  }

  /**
   * Get an element from the list
   * @param id The index of the element
   * @return A const reference to the element
   * @throws std::out_of_range if there is no element with that ID.
   */
  const ValueType &Get(const KeyType id) const {
    if (!sparse_.contains(id)) {
      throw std::out_of_range("No such element with id: " + std::to_string(id));
    }

    std::size_t index = sparse_.at(id);
    return dense_[index];
  }

  ValueType &operator[](const KeyType id) { return Get(id); }

  const ValueType &operator[](const KeyType id) const { return Get(id); }

  /** Remove an element by its id. */
  bool Remove(const std::size_t id) override {
    if (!sparse_.contains(id)) {
      return false;
    }

    std::size_t index = sparse_[id];
    std::size_t back_index = dense_.size() - 1;
    KeyType back_entity = sparse_reverse_[back_index];

    std::swap(dense_[index], dense_[back_index]);

    sparse_[back_entity] = index;
    sparse_reverse_[index] = back_entity;

    sparse_.erase(id);
    sparse_reverse_.erase(back_index);
    dense_.pop_back();

    return true;
  }

  /**
   * Remove all keys and elements
   */
  void Clear() noexcept override {
    dense_.clear();
    sparse_.clear();
    sparse_reverse_.clear();
  }

  /** True if the list contains the specified id. */
  [[nodiscard]] bool Contains(const std::size_t id) const override { return sparse_.contains(id); }

  /** Returns the number of elements in the list. */
  [[nodiscard]] std::size_t Size() const override { return dense_.size(); }

  /** True if the list is empty */
  [[nodiscard]] bool Empty() const override { return dense_.empty(); }

  // -- Iterators -- //
  using Iterator = typename std::vector<ValueType>::iterator;
  using ConstIterator = typename std::vector<ValueType>::const_iterator;

  Iterator begin() noexcept { return dense_.begin(); }
  Iterator end() noexcept { return dense_.end(); }

  ConstIterator begin() const noexcept { return dense_.begin(); }
  ConstIterator end() const noexcept { return dense_.end(); }
  ConstIterator cbegin() const noexcept { return dense_.cbegin(); }
  ConstIterator cend() const noexcept { return dense_.cend(); }
};

#endif // SPARSE_SET_H
