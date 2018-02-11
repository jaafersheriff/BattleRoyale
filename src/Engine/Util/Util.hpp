/* Utility class for basic math functions */
#pragma once
#ifndef _UTIL_HPP_
#define _UTIL_HPP_

#include <string>
#include <iostream>

struct Util {
    
    static constexpr double PI = 3.14159265359;

    static constexpr float infinity = std::numeric_limits<float>::infinity();

    static inline void printVector(const std::string & name, const glm::vec3 & vec) {
        std::cout << name << ": <" <<
            vec.x << ", " << vec.y << " " << vec.z << ">" << std::endl;
    }

    static inline glm::vec3 axisVec(int axis, bool sign) {
        static const glm::vec3 k_posAxisVecs[3]{ {  1.0f, 0.0f, 0.0f }, { 0.0f,  1.0f, 0.0f }, { 0.0f, 0.0f,  1.0f } };
        static const glm::vec3 k_negAxisVecs[3]{ { -1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f } };

        return sign ? k_posAxisVecs[axis] : k_negAxisVecs[axis];
    }

    static inline bool isZero(float v) {
        return glm::abs(v) < std::numeric_limits<float>::epsilon();
    }

    static inline bool isZero(const glm::vec3 & v) {
        isZero(v.x) && isZero(v.y) && isZero(v.z);
    }

    // for adding up a large number of floats
    // adds them in a recursive binary tree
    // avoid the significant precision issues you get when simply adding up
    // a large number of floats linearly
    static float pairwiseSum(int n, const float * vals) {
        if (n == 0) return 0.0f;
        if (n == 1) return vals[0];
        if (n == 2) return vals[0] + vals[1];
        return pairwiseSum(n / 2, vals) + pairwiseSum((n + 1) / 2, vals + n / 2);
    }

};

#endif
