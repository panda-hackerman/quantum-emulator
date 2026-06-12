//
// Created by Eli Michaud on 12/10/2025.
//

#ifndef CONST_MAP_H
#define CONST_MAP_H
#include <stdexcept>

namespace sdw::core {

/// Entry for a compile-time map
template <typename KeyType, typename ValType>
struct CTEntry {
  KeyType key;
  ValType val;
};

/// A map whose elements are known at compile-time.
template <typename KeyType, typename ValType, std::size_t N>
class CompileTimeMap {
private:
  using Entry = CTEntry<KeyType, ValType>;

  const Entry entries_[N];

  static ValType InvalidKey() { throw std::out_of_range("Key doesn't exist in the map"); }

public:
  using Iterator = const Entry *;
  using ConstIterator = const Entry *;

  template <std::convertible_to<Entry>... Entries>
    requires(sizeof...(Entries) == N)
  constexpr explicit(false) CompileTimeMap(Entries &&...entries) : entries_{entries...} {}

  constexpr bool Contains(KeyType key) const noexcept {
    for (const Entry entry : entries_) {
      if (entry.key == key) return true;
    }

    return false;
  }

  [[nodiscard]] constexpr ValType Get(KeyType key) const {
    if (!Contains(key)) {
      return InvalidKey();
    }

    for (Entry entry : entries_) {
      if (entry.key == key) {
        return entry.val;
      }
    }

    std::unreachable();
  }

  [[nodiscard]] constexpr std::size_t Size() const noexcept {
    return std::extent_v<decltype(entries_)>; // return N;
  }

  [[nodiscard]] constexpr ValType operator[](KeyType key) const { return Get(key); }

  constexpr Iterator begin() const noexcept { return entries_; }
  constexpr Iterator end() const noexcept { return entries_ + N; }
  constexpr ConstIterator cbegin() const noexcept { return entries_; }
  constexpr ConstIterator cend() const noexcept { return entries_ + N; }
};

template <typename Key, typename Val>
constexpr auto CTMapBuilder(std::convertible_to<CTEntry<Key, Val>> auto... elements)
    -> CompileTimeMap<Key, Val, sizeof...(elements)> {
  return {elements...};
}

} // namespace sdw::core

#endif // CONST_MAP_H
