#include <cassert>
#include <iostream>

#include "fast-hashmap.hpp"

// Simple function to test FastHashMap
void testFastHashMap() {
    // Create FastHashMap instance with capacity 5
    FastHashMap map(5);

    // Test put and get
    map.put("key1", "value1");
    map.put("key2", "value2");
    assert(map.get("key1") == "value1");
    assert(map.get("key2") == "value2");

    // Test delete
    map.del("key1");
    assert(map.get("key1") == "");
    assert(map.get("key2") == "value2");

    // Test eviction (if implemented)
    map.put("key3", "value3");
    map.put("key4", "value4");
    map.put("key5", "value5");
    map.put("key6", "value6");            // This should trigger eviction if implemented
    assert(map.get("key1") == "");        // Check if key1 is evicted
    assert(map.get("key6") == "value6");  // Check if key6 is added successfully
}

int main() {
    testFastHashMap();
    // Add more test cases as needed

    // If all tests pass, print success message
    std::cout << "All tests passed." << std::endl;

    return 0;
}
