#include "Memory.hpp"

#include "ThirdParty/CoherentLabs_rpmalloc/rpmalloc.h"

#ifdef USE_RPMALLOC

void * coherent_rpmalloc::rpmalloc_allocate_memory_external(size_t size) {
    return std::malloc(size);
}

void coherent_rpmalloc::rpmalloc_deallocate_memory_external(void * ptr) {
    return std::free(ptr);
}

namespace detail {

int InitializeMemory::initialized = 0;

}

#endif