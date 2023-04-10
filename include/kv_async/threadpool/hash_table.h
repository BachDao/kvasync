//
// Created by Bach Dao.
//

#ifndef KV_ASYNC_HASHTABLE_H
#define KV_ASYNC_HASHTABLE_H
#include <memory>
#include <thread>
#include <vector>
namespace kv_async {

template <typename K, typename V> struct node {
  K key_;
  V val_;
  uint64_t hashVal_;

  template <typename Key, typename Val>
    requires std::is_convertible_v<Key, K> && std::is_convertible_v<Val, V>
  node(Key &&key, Val &&val, size_t hashVal)
      : key_(std::forward<Key>(key)), val_(std::forward<Val>(val)),
        hashVal_(hashVal) {}
};

template <typename K, typename V, typename HashFn = std::hash<K>,
          typename KeyEqual = std::equal_to<K>>
class hash_table {
  using node_t = node<K, V>;
  using slot_t = std::atomic<node_t *>;
  static constexpr size_t InvalidIndex = -1;
  std::atomic<slot_t *> ptrStorage_ = nullptr;
  size_t sizeMask_;
  slot_t &get_slot(size_t idx) {
    auto ptrStorage = ptrStorage_.load(std::memory_order_relaxed);
    return ptrStorage[idx];
  }
  node_t *get_node(size_t idx) {
    return get_slot(idx).load(std::memory_order_relaxed);
  }
  size_t probe_limit() { return (sizeMask_ + 1) / 4; }
  node_t *allocate_note_storage() {
    auto requestSize = sizeof(node_t);
    auto alignRequire = std::max({alignof(K), alignof(V), alignof(uint64_t)});
    requestSize += alignRequire - 1;
    return reinterpret_cast<node_t *>(new (std::align_val_t(alignRequire))
                                          std::byte[requestSize]);
  }
  /*
   * Find slot match with "key" or return index of slot that key can
   * inserted
   * */
  template <typename Key> std::tuple<bool, size_t> prepare_insert(Key &&key) {
    auto hashVal = HashFn{}(key);
    auto idx = hashVal & sizeMask_;
    size_t probeCount = 0;
    while (true) {
      auto &curSlot = get_slot(idx);
      auto ptrNode = curSlot.load(std::memory_order_relaxed);
      auto ptrNewNode = allocate_note_storage();
      if (!ptrNode) {
        if (curSlot.compare_exchange_weak(ptrNode, ptrNewNode,
                                          std::memory_order_release))
          return {false, idx};
      } else if (KeyEqual{}(ptrNode->key_, key)) {
        new (&ptrNode->key_) K(std::forward<Key>(key));
        return {true, idx};
      }
      idx = (++idx) & sizeMask_;
      probeCount++;
      if (probeCount > probe_limit()) {
        return {false, InvalidIndex};
      }
    }
  }

public:
  hash_table(size_t size) : sizeMask_(size - 1) {
    auto ptrStorage = new slot_t[size];
    for (int i = 0; i < size; ++i) {
      ptrStorage[i].store(nullptr, std::memory_order_relaxed);
    }
    ptrStorage_.store(ptrStorage, std::memory_order_seq_cst);
  }

  template <typename Key, typename Val> V *put(Key &&key, Val &&val) {
    auto [success, idx] = prepare_insert(std::forward<Key>(key));
    auto ptrNode = get_node(idx);
    if (success) {
      return &ptrNode->val_;
    }
    if (!success && idx != InvalidIndex) {
      ptrNode->val_ = std::forward<Val>(val);
      return &ptrNode->val_;
    }
    return nullptr;
  }
  template <typename Key, typename... Args>
  V *emplace(Key &&key, Args... args) {
    auto [success, idx] = prepare_insert(std::forward<Key>(key));
    auto ptrNode = get_node(idx);
    if (success) {
      return &ptrNode->val_;
    }
    if (!success && idx != InvalidIndex) {
      new (&ptrNode->val_) V(std::forward<Args>(args)...);
      return &ptrNode->val_;
    }
    return nullptr;
  }
};

} // namespace kv_async
#endif // KV_ASYNC_HASHTABLE_H
