#ifndef H_CONCURRENT_MAP
#define H_CONCURRENT_MAP

#include <mutex>
#include <map>
#include <vector>

using namespace std;

const size_t WORD_LENGTH = 100u;

template <typename K, typename V>
class ConcurrentMap {
public:

  struct Access {
    lock_guard<mutex> ref_guard;
    V& ref_to_value;
  };

  explicit ConcurrentMap(size_t bucket_count)
    : _pmap(bucket_count)
    {
    }

  Access operator[](const K& key);

  pair<bool, typename map<K, V>::const_iterator>
  Find(const K& key) const;

  map<K, V> BuildOrdinaryMap();

private:
  struct Bucket {
    mutex mtx;
    map<K, V> data;
    bool is_updated = false;
  };
  vector<Bucket> _pmap;
  size_t whichBucket(const K& key) {
    return _pmap.size() * key.size() / WORD_LENGTH;
  }
};

#endif /*H_CONCURRENT_MAP*/