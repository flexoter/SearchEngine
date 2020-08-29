#include "concurrent_map.h"

using namespace std;

template <typename K, typename V>
typename ConcurrentMap<K, V>::Access ConcurrentMap<K, V>::operator[](const K& key) {
    auto idx = whichBucket(key);
    auto& map_part = _pmap[idx];
return {lock_guard<mutex>(map_part.mtx), map_part.data[key]};
}

template <typename K, typename V>
pair<bool, typename map<K, V>::const_iterator>
ConcurrentMap<K, V>::Find(const K& key) const {
    auto r_it = _pmap.back().data.end();
    bool r = false;
    for (const auto& [mtx, data, update] : _pmap) {
        if (const auto it = data.find(key); it != data.end()) {
        r = true;
        r_it = it;
        }
    }
    return make_pair(r, r_it);
}

template <typename K, typename V>
map<K, V> ConcurrentMap<K, V>::BuildOrdinaryMap() {
    map<K, V> r;
    for (auto& bucket : _pmap) {
      lock_guard part_guard(bucket.mtx);
      r.merge(bucket.data);
    } 
    return r;
  }