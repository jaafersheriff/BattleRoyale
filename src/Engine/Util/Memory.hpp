#pragma once



#include <scoped_allocator>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>

#include "ThirdParty/CoherentLabs_rpmalloc/rpmalloc.h"



inline void * coherent_rpmalloc::rpmalloc_allocate_memory_external(size_t size) {
    return std::malloc(size);
}

inline void coherent_rpmalloc::rpmalloc_deallocate_memory_external(void * ptr) {
    return std::free(ptr);
}



inline void initializeMemory() {
    coherent_rpmalloc::rpmalloc_initialize();
}

inline void * allocate(size_t size) {
    return coherent_rpmalloc::rpmalloc(size);
}

inline void deallocate(void * ptr) {
    return coherent_rpmalloc::rpfree(ptr);
}



template <typename T>
struct Allocator {

    template <typename U> friend struct Allocator;

    using value_type = T;
    using pointer = T *;

    using propagate_on_container_copy_assignment = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type;

    Allocator() = default;

    ~Allocator() = default;

    template <typename U> Allocator(const Allocator<U> &) {}

    pointer allocate(std::size_t n) {
        return static_cast<pointer>(::allocate(n * sizeof(T)));
    }

    void deallocate(pointer p, std::size_t n) {
        ::deallocate(p);
    }

};

template <typename T> using ScopedAllocator = std::scoped_allocator_adaptor<Allocator<T>>;



// Standard containers using custom memory allocator

using String = std::basic_string<char, std::char_traits<char>, ScopedAllocator<char>>;

template <typename T> using Vector = std::vector<T, ScopedAllocator<T>>;

template <typename T> using List = std::list<T, ScopedAllocator<T>>;

template <typename K> using Set = std::set<K, std::less<K>, ScopedAllocator<K>>;

template <typename K> using UnorderedSet = std::unordered_set<K, std::hash<K>, std::equal_to<K>, ScopedAllocator<K>>;

template <typename K, typename V> using Map = std::map<K, V, std::less<K>, ScopedAllocator<std::pair<K, V>>>;

template <typename K, typename V> using UnorderedMap = std::unordered_map<K, V, std::hash<K>, std::equal_to<K>, ScopedAllocator<std::pair<K, V>>>;



// std::unique_ptr variant using custom memory allocator
template <typename T>
class UniquePtr {

    public:

    template <typename... Args> static UniquePtr<T> make(Args &&... args);

    template <typename SubT, typename... Args> static UniquePtr<T> makeAs(Args &&... args);

    public:

    UniquePtr();
    UniquePtr(const UniquePtr<T> & other) = delete;
    UniquePtr(UniquePtr<T> && other);

    ~UniquePtr();

    UniquePtr<T> & operator=(const UniquePtr<T> & other) = delete;
    UniquePtr<T> & operator=(UniquePtr<T> && other);

    T * get() { return m_v; }
    const T * get() const { return m_v; }

    T & operator*() { return *m_v; }
    const T & operator*() const { return *m_v; }

    T * operator->() { return m_v; }
    const T * operator->() const { return m_v; }

    private:

    UniquePtr(T * v);

    private:

    T * m_v;

};



// TEMPLATE IMPLEMENTATION /////////////////////////////////////////////////////



template <typename T>
template <typename... Args>
UniquePtr<T> UniquePtr<T>::make(Args &&... args) {
    return UniquePtr<T>(new (allocate(sizeof(T))) T(std::forward<Args>(args)...));
}

template <typename T>
template <typename SubT, typename... Args>
UniquePtr<T> UniquePtr<T>::makeAs(Args &&... args) {
    static_assert(std::is_base_of<T, SubT>::value, "SubT must be derived from T");
    return UniquePtr<T>(new (allocate(sizeof(SubT))) SubT(std::forward<Args>(args)...));
}

template <typename T>
UniquePtr<T>::UniquePtr() :
    m_v(nullptr)
{}

template <typename T>
UniquePtr<T>::UniquePtr(UniquePtr<T> && other) :
    m_v(other.m_v)
{
    other.m_v = nullptr;
}

template <typename T>
UniquePtr<T>::~UniquePtr() {
    if (!m_v) return;
    m_v->~T();
    deallocate(m_v);
}

template <typename T>
UniquePtr<T> & UniquePtr<T>::operator=(UniquePtr<T> && other) {
    m_v = other.m_v;
    other.m_v = nullptr;
}

template <typename T>
UniquePtr<T>::UniquePtr(T * v) :
    m_v(v)
{}