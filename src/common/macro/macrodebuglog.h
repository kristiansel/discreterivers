#ifndef MACRODEBUGLOG_H
#define MACRODEBUGLOG_H


#include <iostream>

#ifndef NDEBUG
    #include <iostream>
    #define DEBUG_LOG(debugstream)                                              \
    {                                                                           \
        std::cerr << "Debug info: " << __FILE__ << ":" << __LINE__;             \
        std::cerr << ":" << __FUNCTION__ << ": " << debugstream << std::endl;   \
    }
#else
    #define DEBUG_LOG(debugstream)
#endif

#endif // MACRODEBUGLOG_H
