#include "Memory.hpp"



#ifdef USE_RPMALLOC

namespace detail {

int InitializeMemory::initialized = 0;

}

#endif