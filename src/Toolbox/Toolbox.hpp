/* Toolbox class
 * Provides various utility functions */
#pragma once
#ifndef _TOOLBOX_HPP_
#define _TOOLBOX_HPP_

#include "glm/glm.hpp"
#include <stdlib.h>

// TODO : Rewrite this so it's up to C++ standards
class Toolbox {
public:
    /* PI */
    static constexpr double PI = 3.14159265359;

    /* Generate a random float [0, 1] */
    static inline float genRandom() {
        return rand() / (float) RAND_MAX;
    }

    /* Generate a random value in a range [min, max] */
    static inline float genRandom(const float min, const float max) {
        return genRandom() * (max - min) + min;
    }

    /* Generate a random vec3 with values [0, 1] */
    static inline glm::vec3 genRandomVec3() {
        return glm::vec3(genRandom(), genRandom(), genRandom());
    }
    
    /* Generate random vec3 with values [0, 1] */
    static inline glm::vec3 genRandomVec3(const float min, const float max) {
        return glm::vec3(genRandom(min, max), genRandom(min, max), genRandom(min, max));
    }
};


#endif