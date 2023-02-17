#ifndef LRU_CACHE_H_
#define LRU_CACHE_H_

#include <list>
#include <unordered_map>

template <typename K, typename V>
class LRUCache {
  public:
    LRUCache(size_t capacity) : capacity_(capacity), size_(0) {
      pthread_mutex_init(&lock_, nullptr);
    }
    virtual ~LRUCache() {
      pthread_mutex_destroy(&lock_);
    }
    int Put(const K& key, const V& val) {
      if (sizeof(val) > capacity_) {
        return -1;
      }
      pthread_mutex_lock(&lock_);
      auto it = map_.find(key);
      if (it == map_.end()) { // New element
        while (size_ + sizeof(val) >= capacity_) {
          Evict();
        }
        queue_.push_back(key);
        auto last = queue_.end();
        last--;
        map_.insert({key, last});
        kv_.insert({key, val});
        size_ += sizeof(val);
        pthread_mutex_unlock(&lock_);
        return 0;
      } else { // Existing element
        size_ -= sizeof(it->second);
        kv_[it->first] = val;
        size_ += sizeof(val);
        queue_.splice(queue_.end(), queue_, it->second);
        pthread_mutex_unlock(&lock_);
        return 1;
      }
    }
    int Get(const K& key, V* val) {
      pthread_mutex_lock(&lock_);
      auto it = map_.find(key);
      if (it == map_.end()) { // Does not exist
        pthread_mutex_unlock(&lock_);
        return -1;
      }
      *val = kv_.at(it->first);
      // LRU cache tracks Get access.
      queue_.splice(queue_.end(), queue_, it->second);
      pthread_mutex_unlock(&lock_);
      return 0;
    }
  private:
    void Evict() {
      auto first_it = queue_.begin();
      auto map_it = map_.find(*first_it);
      size_ -= sizeof(kv_.at(*first_it));
      kv_.erase(*first_it);
      map_.erase(map_it);
      queue_.pop_front();
    }
    size_t capacity_;
    size_t size_;
    typename std::list<K> queue_;
    std::unordered_map<K, typename std::list<K>::iterator> map_;
    typename std::unordered_map<K, V> kv_;
    pthread_mutex_t lock_;
};

#endif
