//
// Created by Eli Michaud on 7/4/2025.
//

#ifndef BIMAP_H
#define BIMAP_H
#include <functional>
#include <list>
#include <set>

namespace sdw::core {

/**
* An unordered bidirectional map. Inserted elements are stored only once unlike maintaining two
* separate maps.
* @tparam KeyType The key type when forward
* @tparam ValType The value type when forward
*/
template <typename KeyType, typename ValType>
class BiMap {
private:
  struct Entry {
    KeyType key;
    ValType val;

    template <typename K, typename V>
    Entry(K &&k, V &&v): key(std::forward<K>(k)), val(std::forward<V>(v)) {}
  };

  struct ForwardCompare {
    using is_transparent = void;
    bool operator()(const Entry *a, const Entry *b) const noexcept { return a->key < b->key; }
    bool operator()(const Entry *e, const KeyType &k) const noexcept { return e->key < k; }
    bool operator()(const KeyType &k, const Entry *e) const noexcept { return k < e->key; }
  };

  struct ReverseCompare {
    using is_transparent = void;
    bool operator()(const Entry *a, const Entry *b) const noexcept { return a->val < b->val; }
    bool operator()(const Entry *e, const ValType &v) const noexcept { return e->val < v; }
    bool operator()(const ValType &v, const Entry *e) const noexcept { return v < e->val; }
  };

  std::set<const Entry*, ForwardCompare> forward_index_{ForwardCompare()};
  std::set<const Entry*, ReverseCompare> reverse_index_{ReverseCompare()};

  using ForwardIterator = typename decltype(forward_index_)::iterator;
  using ReverseIterator = typename decltype(reverse_index_)::iterator;
  using ConstForwardIterator = typename decltype(forward_index_)::const_iterator;
  using ConstReverseIterator = typename decltype(reverse_index_)::const_iterator;

  std::list<Entry> storage_;

public:
  /**
   * Attempt to insert a new entry into the map.
   * @param key The key to insert
   * @param val The value to associate with the given key.
   * @tparam K The class of the key param (that is convertable to KeyType).
   * @tparam V The class of the value param (that is convertable to ValType).
   * @return True if the element was inserted. False if an element with this key or value already
   * exists.
   * @note This is <b>not</b> exception safe. If the insertions into any of the three underlying
   * containers throw, the bimap will likely become inconsistent or malformed. Fixing this is
   * non-trivial.
   */
  template <typename K, typename V>
    requires requires(K, V) {
      requires std::is_convertible_v<K, KeyType>;
      requires std::is_convertible_v<V, ValType>;
    }
  bool Insert(K &&key, V &&val) {
    if (HasKey(key) || HasValue(val)) {
      return false;
    }

    Entry *entry_ptr = &storage_.emplace_back(std::forward<K>(key), std::forward<V>(val));

    forward_index_.insert(entry_ptr);
    reverse_index_.insert(entry_ptr);

    return true;
  }

  const ValType *Find(const KeyType &key) {
    ForwardIterator it = forward_index_.find(key);
    return it != forward_index_.end() ? &(*it)->val : nullptr;
  }

  const KeyType *FindReverse(const ValType &val) {
    ReverseIterator it = reverse_index_.find(val);
    return it != reverse_index_.end() ? &(*it)->key : nullptr;
  }

  bool HasKey(const KeyType &key) { return forward_index_.contains(key); }

  bool HasValue(const ValType &val) { return reverse_index_.contains(val); }

  /**
   * Removes the entry with this key from the map.
   * @param key The key to search for.
   * @return True if the entry was removed, false if no such entry exists.
   */
  bool RemoveByKey(const KeyType &key) {
    ConstForwardIterator fwd_itr = forward_index_.find(key);

    if (fwd_itr == forward_index_.end()) {
      return false; // Doesn't contain the key element
    }

    const Entry &entry = **fwd_itr;

    ConstReverseIterator rvs_itr = reverse_index_.find(entry.val);

    forward_index_.erase(fwd_itr);
    reverse_index_.erase(rvs_itr);

    std::erase_if(storage_, [&entry](Entry &el) {
      return el.key == entry.key && el.val == entry.val;
    });

    return true;
  }

  /**
   * Removes the entry with this value from the map.
   * @param val The value to search for.
   * @return True if the entry was removed, false if no such entry exists.
   */
  bool RemoveByValue(const ValType &val) {
    ConstReverseIterator rvs_itr = reverse_index_.find(val);

    if (rvs_itr == reverse_index_.end()) {
      return false; // Doesn't contain the key element
    }

    const Entry &entry = **rvs_itr;

    ConstForwardIterator fwd_itr = forward_index_.find(entry.key);

    forward_index_.erase(fwd_itr);
    reverse_index_.erase(rvs_itr);

    std::erase_if(storage_, [&entry](Entry &item) {
      return item.key == entry.key && item.val == entry.val;
    });

    return true;
  }

  /**
   * Get the size of the map.
   * @return The number of elements in this map.
   */
  [[nodiscard]] std::size_t Size() const noexcept { return storage_.size(); }

  /**
   * Check if empty.
   * @return True if the map is empty
   */
  [[nodiscard]] bool Empty() const noexcept { return storage_.empty(); }

};
}

#endif //BIMAP_H
