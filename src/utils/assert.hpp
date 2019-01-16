#ifndef C4_ASSERT_HPP
#define C4_ASSERT_HPP

#include "macros.hpp"
#include <iostream>

class Assert {
public:
  explicit Assert(bool cond) : assertion(cond) {}
  ~Assert() {
    if (!assertion) {
      std::cerr << std::endl;
      abort();
    }
  }
  template <class T> inline Assert &operator<<(const T &message) {
    if (!assertion)
      std::cerr << message;
    return *this;
  }

private:
  bool assertion;
};

class NullAssert {
public:
  NullAssert(bool) {}
  template <class T> inline NullAssert &operator<<(const T &message) {
    UNUSED(message);
    return *this;
  }
};

#ifndef NDEBUG
#define my_assert(cond) Assert(cond)
#else
#define my_assert(cond) NullAssert(cond)
#endif

#endif
