#include "fast-hashmap.hpp"

#include <iostream>

FastHashMap::FastHashMap(size_t capacity) : capacity_(capacity) {
    // slots_.resize(capacity);  // To reduce reallocations
    buckets_.resize(capacity);
}

void FastHashMap::evict_() {
    // int evictIndex = evictionQueue_.front();
    // Find the oldest slot based on timestamp
    size_t oldestIndex = 0;
    size_t oldestTimestamp = slots_[0].timestamp;

    for (size_t i = 1; i < slots_.size(); ++i) {
        if (slots_[i].timestamp < oldestTimestamp) {
            oldestIndex = i;
            oldestTimestamp = slots_[i].timestamp;
        }
    }
    Slot& evictionSlot = slots_[oldestIndex];
    evictionSlot.isSentinel = true;
    if (evictionSlot.prev != -1) {
        slots_[evictionSlot.prev].next = slots_[oldestIndex].next;
    } else {
        for (Bucket& bucket : buckets_) {
            if (bucket.firstSlotIndex == oldestIndex) {
                bucket.firstSlotIndex = evictionSlot.next;
            }
        }
    }
}

void FastHashMap::put(const std::string& key, const std::string& value) {
    size_t hashedKey = hasher_(key);
    size_t bucketIndex = hashedKey % capacity_;
    Bucket& bucket = buckets_[bucketIndex];
    std::unique_lock<std::shared_mutex> lock(bucket.mutex);

    if (size_ == capacity_) {
        evict_();
    }

    if (bucket.firstSlotIndex == -1) {  // No element in the bucket yet
        slots_.emplace_back(Slot(hashedKey, value));
        bucket.firstSlotIndex = slots_.size() - 1;
    } else {  // 1 or more element already in the bucket
        bool exists = false;
        Slot& currSlot = slots_[bucket.firstSlotIndex];
        int prevSlotIndex = bucket.firstSlotIndex;
        Slot& prevSlot = currSlot;
        while (currSlot.next != -1) {
            if (hashedKey == currSlot.hashedKey) {  // already exists - update
                exists = true;
                break;
            }
            prevSlot = currSlot;
            currSlot = slots_[currSlot.next];
        }

        if (exists || hashedKey == currSlot.hashedKey) {
            // update
            currSlot.value = value;
        } else {
            slots_.emplace_back(Slot(hashedKey, value));
            currSlot.next = slots_.size() - 1;
            int prevIndex = prevSlot.prev;
            if (prevIndex == -1) {
                prevIndex = bucket.firstSlotIndex;
            }
            currSlot.prev = slots_[prevIndex].next;
        }
    }
    size_++;
}

void FastHashMap::del(const std::string& key) {
    size_t hashedKey = hasher_(key);
    size_t bucketIndex = hashedKey % capacity_;
    Bucket& bucket = buckets_[bucketIndex];
    std::unique_lock<std::shared_mutex> lock(bucket.mutex);

    Slot& currSlot = slots_[bucket.firstSlotIndex];
    bool exists;
    while (currSlot.next != -1) {
        if (hashedKey == currSlot.hashedKey) {
            exists = true;
            break;
        }
        currSlot = slots_[currSlot.next];
    }

    if (exists || hashedKey == currSlot.hashedKey) {
        slots_[currSlot.prev].next = currSlot.next;
        currSlot.isSentinel = true;
        size_--;
    } else {
        std::cout << "Invalid Key";  // TODO:Error handling
    }
}

std::string FastHashMap::get(const std::string& key) {
    size_t hashedKey = hasher_(key);
    size_t bucketIndex = hashedKey % capacity_;
    Bucket& bucket = buckets_[bucketIndex];
    std::shared_lock<std::shared_mutex> lock(bucket.mutex);
    Slot& currSlot = slots_[bucket.firstSlotIndex];
    bool exists;
    while (currSlot.next != -1) {
        if (hashedKey == currSlot.hashedKey) {
            exists = true;
            break;
        }
    }
    if (exists || hashedKey == currSlot.hashedKey) {
        return currSlot.value;
    } else {
        std::cout << "Invalid Key";  // TODO:Error handling
    }
    return "";
}
