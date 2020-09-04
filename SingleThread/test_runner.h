#ifndef H_TEST_RUNNER
#define H_TEST_RUNNER

#include <iostream>
#include <set>
#include <map>
#include <string>

template <class T>
std::ostream& operator << (std::ostream& os, const std::set<T>& s);

template <class K, class V>
std::ostream& operator << (std::ostream& os, const std::map<K, V>& s);

template<class T, class U>
void AssertEqual(const T& t, const U& u, const std::string& hint = {});

inline void Assert(bool b, const std::string& hint) {
  AssertEqual(b, true, hint);
}

class TestRunner {
public:
  template <class TestFunc>
  void RunTest(TestFunc func, const std::string& test_name);
  ~TestRunner();
private:
  int fail_count = 0;
};

#define ASSERT_EQUAL(x, y) {            \
  ostringstream assert_os;                     \
  assert_os << #x << " != " << #y << ", "      \
    << __FILE__ << ":" << __LINE__;     \
  AssertEqual(x, y, assert_os.str());          \
}

#define ASSERT(x) {                     \
  ostringstream assert_os;                     \
  assert_os << #x << " is false, "             \
    << __FILE__ << ":" << __LINE__;     \
  Assert(x, assert_os.str());                  \
}

#define RUN_TEST(tr, func)              \
  tr.RunTest(func, #func)

#endif /*H_TEST_RUNNER*/

