//
// Created by Eli Michaud on 12/10/2025.
//

#ifndef CONST_MAP_H
#define CONST_MAP_H
#include <array>
#include <stdexcept>

/// Entry for a compile-time map
template <typename KeyType, typename ValType>
struct CTEntry {
  using Key_T = KeyType;
  using Val_T = ValType;

  KeyType key;
  ValType val;
};

/// A map whose elements are known at compile-time.
template <typename KeyType, typename ValType, std::size_t N>
class CompileTimeMap {
private:
  using Entry = CTEntry<KeyType, ValType>;

  std::array<Entry, N> entries_;

  [[noreturn]] static constexpr ValType XInvalidKey() {
    throw std::out_of_range("Key doesn't exist in the map");
  }

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

  [[nodiscard]] constexpr const ValType &Get(KeyType key) const {
    if (!Contains(key)) {
      return XInvalidKey();
    }

    for (std::size_t i = 0; i < N; ++i) {
      if (const Entry &entry = entries_[i]; entry.key == key) {
        return entry.val;
      }
    }

    std::unreachable();
  }

  [[nodiscard]] constexpr std::size_t Size() const noexcept {
    return std::extent_v<decltype(entries_)>; // return N;
  }

  [[nodiscard]] constexpr const ValType &operator[](KeyType key) const { return Get(key); }

  constexpr Iterator begin() const noexcept { return entries_; }
  constexpr Iterator end() const noexcept { return entries_ + N; }
  constexpr ConstIterator cbegin() const noexcept { return entries_; }
  constexpr ConstIterator cend() const noexcept { return entries_ + N; }
};

namespace ct_map::internal {

template <typename Key, typename Val, std::size_t Size, std::size_t... Is>
static constexpr CompileTimeMap<Key, Val, Size> CTBuilderImpl(CTEntry<Key, Val> (&arr)[Size],
                                                              std::index_sequence<Is...>) {
  return {arr[Is]...};
}

template <typename Key, typename Val, std::size_t Size, std::size_t... Is>
static constexpr CompileTimeMap<Key, Val, Size> CTBuilderImpl(CTEntry<Key, Val> (&&arr)[Size],
                                                              std::index_sequence<Is...>) {
  return {std::move(arr[Is])...};
}

} // namespace ct_map::internal

template <typename Key, typename Val, std::size_t Size>
constexpr CompileTimeMap<Key, Val, Size> CTMapBuilder(CTEntry<Key, Val> (&arr)[Size]) {
  return ct_map::internal::CTBuilderImpl(arr, std::make_index_sequence<Size>{});
}

template <typename Key, typename Val, std::size_t Size>
constexpr CompileTimeMap<Key, Val, Size> CTMapBuilder(CTEntry<Key, Val> (&&arr)[Size]) {
  return ct_map::internal::CTBuilderImpl(std::move(arr), std::make_index_sequence<Size>{});
}

#endif // CONST_MAP_H
