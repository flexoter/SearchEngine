#ifndef H_SYNCHRONIZED
#define H_SYNCHRONIZED

#include <mutex>

using namespace std;

template <typename T>
class Synchronized {
public:
  explicit Synchronized(T init = T())
    : _value(move(init))
    {
    } 
  
  struct Access {
    T& ref_to_value;
    lock_guard<mutex> lock;
  };

  Access GetAccess() {
    return {_value, lock_guard<mutex>(ref_mut)};
  }
private:
  mutex ref_mut;
  T _value;
};

#endif /*H_SYNCHRONIZED*/