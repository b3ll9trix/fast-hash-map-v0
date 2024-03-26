#ifndef FAST_HASHMAP_HPP
#define FAST_HASH_HPP

#include <cstddef>
#include <shared_mutex>
#include <string>
#include <vector>

class FastHashMap {
   public:
    FastHashMap(size_t capacity);
    void put(const std::string& key, const std::string& value);

    void del(const std::string& key);

    std::string get(const std::string& key);

   private:
    size_t capacity_;
    size_t size_;
    struct Slot {
        size_t hashedKey;
        std::string value;
        int next;
        int prev;
        bool isSentinel;
        size_t timestamp;
        Slot() = default;

        Slot(const size_t& hk, const std::string& v) : hashedKey(hk), value(v), next(-1), prev(-1), isSentinel(false) {
            timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        }
    };
    struct Bucket {
        std::shared_mutex mutex;
        int firstSlotIndex;
        // Bucket() = default;
        Bucket() : firstSlotIndex(-1) {}
    };
    std::vector<Slot> slots_;
    std::vector<Bucket> buckets_;
    std::hash<std::string> hasher_;
    void evict_();
};

#endif  // FAST_HASHMAP_HPP
