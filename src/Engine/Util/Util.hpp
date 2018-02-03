/* Utility class for basic math functions */
#pragma once
#ifndef _UTIL_HPP_
#define _UTIL_HPP_

#include <string>
#include <iostream>

struct Util {
    static constexpr double PI = 3.14159265359;

    static inline void printVector(std::string name, glm::vec3 vec) {
        std::cout << name << ": <" <<
            vec.x << ", " << vec.y << " " << vec.z << ">" << std::endl;
    }
};

#endif
