#ifndef MACRODEBUGASSERT_H
#define MACRODEBUGASSERT_H

#ifndef NDEBUG
  #include <cassert>
  #define DEBUG_ASSERT(condition)                                                  \
  {                                                                             \
      if(!(condition))                                                          \
      {                                                                         \
          std::cerr << "Assertion failed at " << __FILE__ << ":" << __LINE__;   \
          std::cerr << " inside " << __FUNCTION__ << std::endl;                 \
          std::cerr << "Condition: " << #condition;                             \
          abort();                                                              \
      }                                                                         \
  }
#else
  #define DEBUG_ASSERT(condition) (condition)
#endif


#endif // MACRODEBUGASSERT_H
