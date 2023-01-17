#ifndef LRU_CACHE_H_
#define LRU_CACHE_H_

#include <list>
#include <unordered_map>

namespace PennCloud::Utils {

template <typename K, typename V>
class LRUCache {
  public:
    LRUCache() : capacity_(10) {}
    LRUCache(size_t capacity) : capacity_(capacity) {}
    ~LRUCache() {}
    int Put(const K& key, const V& val) {
      auto it = map_.find(key);
      if (it == map_.end()) { // New element
        while (queue_.size() >= capacity_) {
          Evict();
        }
        queue_.push_back(key);
        auto last = queue_.end();
        last--;
        map_.insert({key, last});
        kv_.insert({key, val});
        return 0;
      } else { // Existing element
        kv_[it->first] = val;
        queue_.splice(queue_.end(), queue_, it->second);
        return 1;
      }
    }
    int Get(const K& key, V* val) {
      auto it = map_.find(key);
      if (it == map_.end()) { // Does not exist
        return -1;
      }
      *val = kv_.at(it->first);
      // LRU cache tracks Get access.
      queue_.splice(queue_.end(), queue_, it->second);
      return 0;
    }
  private:
    void Evict() {
      auto first_it = queue_.begin();
      auto map_it = map_.find(*first_it);
      map_.erase(map_it);
      queue_.pop_front();
    }
    size_t capacity_;
    typename std::list<K> queue_;
    std::unordered_map<K, typename std::list<K>::iterator> map_;
    typename std::unordered_map<K, V> kv_;
};

} // namespace PennCloud::Utils

#endif
