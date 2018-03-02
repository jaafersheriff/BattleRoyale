#pragma once

#ifndef __GNUC__
#define USE_RPMALLOC
#endif


#include <scoped_allocator>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <cstdlib>


#ifdef USE_RPMALLOC



#include "ThirdParty/CoherentLabs_rpmalloc/rpmalloc.h"



// trick to call memory initialization functions before static evaluations
namespace detail {

struct InitializeMemory {

    InitializeMemory() {
        if (!initialized++) {
            coherent_rpmalloc::rpmalloc_initialize();
        }
    }

    static int initialized;

};

static InitializeMemory f_initializeMemory;

}



#endif



inline void * allocate(size_t size) {
#ifdef USE_RPMALLOC
    return coherent_rpmalloc::rpmalloc(size);
#else
    return std::malloc(size);
#endif
}

inline void deallocate(void * ptr) {
#ifdef USE_RPMALLOC
    return coherent_rpmalloc::rpfree(ptr);
#else
    return std::free(ptr);
#endif
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

template <typename T1, typename T2>
bool operator==(const Allocator<T1> & a1, const Allocator<T2> & a2) {
    return true;
}

template <typename T1, typename T2>
bool operator!=(const Allocator<T1> & a1, const Allocator<T2> & a2) {
    return false;
}

#ifdef USE_RPMALLOC
template <typename T> using ScopedAllocator = std::scoped_allocator_adaptor<Allocator<T>>;
#else
template <typename T> using ScopedAllocator = std::allocator<T>;
#endif



// Standard containers using custom memory allocator

// std::string equivalent
using String = std::basic_string<char, std::char_traits<char>, ScopedAllocator<char>>;

#ifdef USE_RPMALLOC

// convert String to std::string
inline std::string convert(const String & string) {
    return std::string(string.c_str());
}

// convert std::string to String
inline String convert(const std::string & string) {
    return String(string.c_str());
}

#else

inline std::string convert(const std::string & string) {
    return string;
}

#endif

// std::vector equivalent
template <typename T> using Vector = std::vector<T, ScopedAllocator<T>>;

// std::list equivalent
template <typename T> using List = std::list<T, ScopedAllocator<T>>;

// std::set equivalent
template <typename K> using Set = std::set<K, std::less<K>, ScopedAllocator<K>>;

// std::unordered_set equivalent
template <typename K> using UnorderedSet = std::unordered_set<K, std::hash<K>, std::equal_to<K>, ScopedAllocator<K>>;

// std::map equivalent
template <typename K, typename V> using Map = std::map<K, V, std::less<K>, ScopedAllocator<std::pair<K, V>>>;

// std::unordered_map equivalent
template <typename K, typename V> using UnorderedMap = std::unordered_map<K, V, std::hash<K>, std::equal_to<K>, ScopedAllocator<std::pair<const K, V>>>;



// std::unique_ptr variant using custom memory allocator
template <typename T>
class UniquePtr {

    template <typename U> friend class UniquePtr;

    public:

    template <typename... Args> static UniquePtr<T> make(Args &&... args);

    template <typename SubT, typename... Args> static UniquePtr<T> makeAs(Args &&... args);

    public:

    UniquePtr();
    UniquePtr(const UniquePtr<T> & other) = delete;
    template <typename U, typename std::enable_if<std::is_base_of<T, U>::value, int>::type = 0>
    UniquePtr(UniquePtr<U> && other);

    ~UniquePtr();

    UniquePtr<T> & operator=(const UniquePtr<T> & other) = delete;
    UniquePtr<T> & operator=(UniquePtr<T> && other);

    explicit operator bool() const { return m_v != nullptr; }

    void release();

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

// std::unique_ptr array variant using custom memory allocator
template <typename T>
class UniquePtr<T[]> {

    public:

    static UniquePtr<T[]> make(size_t size);

    public:

    UniquePtr();
    UniquePtr(const UniquePtr<T[]> & other) = delete;
    UniquePtr(UniquePtr<T[]> && other);

    ~UniquePtr();

    UniquePtr<T[]> & operator=(const UniquePtr<T[]> & other) = delete;
    UniquePtr<T[]> & operator=(UniquePtr<T[]> && other);

    explicit operator bool() const { return m_vs != nullptr; }

    void release();

    T * get() { return m_vs; }
    const T * get() const { return m_vs; }

    T & operator[](size_t i) { return m_vs[i]; };

    size_t size() const { return m_size; }

    private:

    UniquePtr(T * vs, size_t size);

    private:

    T * m_vs;
    size_t m_size;

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
template <typename U, typename std::enable_if<std::is_base_of<T, U>::value, int>::type>
UniquePtr<T>::UniquePtr(UniquePtr<U> && other) :
    m_v(other.m_v)
{
    other.m_v = nullptr;
}

template <typename T>
UniquePtr<T>::~UniquePtr() {
    release();
}

template <typename T>
UniquePtr<T> & UniquePtr<T>::operator=(UniquePtr<T> && other) {
    m_v = other.m_v;
    other.m_v = nullptr;
    return *this;
}

template <typename T>
void UniquePtr<T>::release() {
    if (!m_v) return;
    m_v->~T();
    deallocate(m_v);
    m_v = nullptr;
    m_size = 0;
}

template <typename T>
UniquePtr<T>::UniquePtr(T * v) :
    m_v(v)
{}



template <typename T>
UniquePtr<T[]> UniquePtr<T[]>::make(size_t size) {
    static_assert(std::is_default_constructible<T>::value, "T must be default constructible");
    return UniquePtr<T[]>(new (allocate(size * sizeof(T))) T[size], size);
}

template <typename T>
UniquePtr<T[]>::UniquePtr() :
    m_vs(nullptr),
    m_size(0)
{}

template <typename T>
UniquePtr<T[]>::UniquePtr(UniquePtr<T[]> && other) :
    m_vs(other.m_vs),
    m_size(other.m_size)
{
    other.m_vs = nullptr;
    other.m_size = 0;
}

template <typename T>
UniquePtr<T[]>::~UniquePtr() {
    release();
}

template <typename T>
UniquePtr<T[]> & UniquePtr<T[]>::operator=(UniquePtr<T[]> && other) {
    m_vs = other.m_vs;
    m_size = other.m_size;
    other.m_vs = nullptr;
    other.m_size = 0;
    return *this;
}

template <typename T>
void UniquePtr<T[]>::release() {
    if (!m_vs) return;
    for (size_t i(0); i < m_size; ++i) m_vs[i].~T();
    deallocate(m_vs);
    m_vs = nullptr;
    m_size = 0;
}

template <typename T>
UniquePtr<T[]>::UniquePtr(T * vs, size_t size) :
    m_vs(vs),
    m_size(size)
{}