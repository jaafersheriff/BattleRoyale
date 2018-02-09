#pragma once



#include <memory>
#include <unordered_map>



template <typename T>
class Depot {

    private:

    static std::unordered_map<const T *, std::unique_ptr<T>> s_store;

    private:

    Depot() = default;

    public:

    static T * add(T * v) {
        s_store.emplace(v, std::unique_ptr<T>(v));
        return v;
    }

    static bool has(const T * v) {
        return s_store.count(v) > 0;
    }

    static std::unique_ptr<T> remove(T * v) {
        auto it(s_store.find(v));
        if (it == s_store.end()) {
            return nullptr;
        }
        std::unique_ptr<T> e(std::move(it->second));
        s_store.erase(it);
        return e;
    }

};

template <typename T>
std::unordered_map<const T *, std::unique_ptr<T>> Depot<T>::s_store;