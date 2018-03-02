/* Utility class for basic math functions */
#pragma once
#ifndef _UTIL_HPP_
#define _UTIL_HPP_

#include <iostream>
#include <sstream>
#include <fstream>
#include <chrono>

#include "glm/gtc/matrix_transform.hpp"

#include "Model/Mesh.hpp"

struct Util {

    static inline float infinity() {
        return std::numeric_limits<float>::infinity();
    }

    static constexpr float epsilon = std::numeric_limits<float>::epsilon();

    static inline void printVector(const String & name, const glm::vec3 & vec) {
        std::cout << name << ": <" <<
            vec.x << ", " << vec.y << " " << vec.z << ">" << std::endl;
    }

    static inline glm::vec3 axisVec(int axis, bool sign) {
        static const glm::vec3 k_posAxisVecs[3]{ {  1.0f, 0.0f, 0.0f }, { 0.0f,  1.0f, 0.0f }, { 0.0f, 0.0f,  1.0f } };
        static const glm::vec3 k_negAxisVecs[3]{ { -1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f } };

        return sign ? k_posAxisVecs[axis] : k_negAxisVecs[axis];
    }

    static inline bool isZero(float v, float e = epsilon) {
        return glm::abs(v) < e;
    }

    static inline bool isZero(const glm::vec3 & v, float e = epsilon) {
        return isZero(v.x, e) && isZero(v.y, e) && isZero(v.z, e);
    }

    static inline bool isEqual(float v1, float v2, float e = epsilon) {
        return isZero(v1 - v2, e);
    }

    static inline bool isEqual(const glm::vec3 & v1, const glm::vec3 & v2, float e = epsilon) {
        return isZero(v1 - v2, e);
    }

    static inline bool isGreater(float v1, float v2, float e = epsilon) {
        return v1 - v2 > -e;
    }

    static inline bool isLess(float v1, float v2, float e = epsilon) {
        return v1 - v2 < e;
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
    
    // the order glm does matrix comp seems super backwards to me so i made these helper functions
    static glm::mat4 compositeTransform(const glm::fvec3 & scale, const glm::fmat3 & rotation, const glm::fvec3 & translation) {
        return compositeTransform(scale, glm::mat4(rotation), translation);
    }
    static glm::mat4 compositeTransform(const glm::fvec3 & scale, const glm::fmat4 & rotation, const glm::fvec3 & translation) {
        return glm::scale(glm::translate(glm::mat4(), translation) * rotation, scale);
    }
    static glm::mat4 compositeTransform(const glm::fvec3 & scale, const glm::fvec3 & translation) {
        return glm::scale(glm::translate(glm::mat4(), translation), scale);
    }

    // rad is the sphere's radius
    // theta is CCW angle on xy plane
    // phi is angle from +z axis
    // all angles are in radians
    static glm::vec3 sphericalToCartesian(float rad, float theta, float phi) {
        float sinTheta = std::sin(theta);
        float cosTheta = std::cos(theta);
        float sinPhi = std::sin(phi);
        float cosPhi = std::cos(phi);

        return glm::vec3(
            rad * sinPhi * cosTheta,
            rad * sinPhi * sinTheta,
            rad * cosPhi
        );
    }

    static glm::vec3 sphericalToCartesian(const glm::vec3 & v) {
        return sphericalToCartesian(v.x, v.y, v.z);
    }

    // return a vec3 with the following values
    // first (x) is the spherical radius
    // second (y) is the theta angle on xy plane
    // third (z) is the phi angle off +z axis
    // all angles are in radians
    static glm::vec3 cartesianToSpherical(const glm::vec3 & v) {
        float rad(glm::length(v));
        return glm::vec3(
            rad,
            std::atan2(v.y, v.x),
            std::acos(v.z / rad)
        );
    }

    // calculated a view matrix based on camera u v w vectors, which must be orthonormal
    static glm::mat4 viewMatrix(const glm::vec3 & camPos, const glm::vec3 & camU, const glm::vec3 & camV, const glm::vec3 & camW) {
        glm::vec3 trans(-camPos);
        return glm::mat4(
                           camU.x,                camV.x,                camW.x, 0.0f,
                           camU.y,                camV.y,                camW.y, 0.0f,
                           camU.z,                camV.z,                camW.z, 0.0f,
            glm::dot(camU, trans), glm::dot(camV, trans), glm::dot(camW, trans), 1.0f
        );
    }

    // creates a matrix that maps to the given orthonormal basis
    static glm::mat3 mapTo(const glm::vec3 & x, const glm::vec3 & y, const glm::vec3 & z) {
        return glm::transpose(glm::mat3(x, y, z));
    }

    // creates a matrix that maps from the given orthonormal basis
    static glm::mat3 mapFrom(const glm::vec3 & x, const glm::vec3 & y, const glm::vec3 & z) {
        return glm::mat3(x, y, z);
    }

    static String toString(const glm::vec3 & v) {
        return convert("(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")");
    }

    // project v onto plane defined by unit vector norm
    static glm::vec3 projectOnto(const glm::vec3 & v, const glm::vec3 & norm) {
        return v - glm::dot(v, norm) * norm;
    }

    // norm must be unit vector
    // If v dot norm is positive, just return v. Otherwise, return the closest
    // point on the plane defined by norm.
    static glm::vec3 removeAllAgainst(const glm::vec3 & v, const glm::vec3 & norm) {
        float dot(glm::dot(v, norm));
        if (dot >= 0) {
            return v;
        }
        return v - dot * norm;
    }
    
    // norm must be unit vector
    // If v dot norm is positive, just return v. Otherwise, return the point
    // amount closer to the plane defined by norm
    static glm::vec3 removeSomeAgainst(const glm::vec3 & v, const glm::vec3 & norm, float amount) {
        float dot(glm::dot(v, norm));
        if (dot >= 0) {
            return v;
        }
        return v + glm::min(-dot, amount) * norm;
    }

    inline static glm::vec2 safeNorm(const glm::vec2 & v) {
        if (v != glm::vec2()) {
            return glm::normalize(v);
        }
        return glm::vec2();
    }

    inline static glm::vec3 safeNorm(const glm::vec3 & v) {
        if (v != glm::vec3()) {
            return glm::normalize(v);
        }
        return glm::vec3();
    }

    inline static glm::vec4 safeNorm(const glm::vec4 & v) {
        if (v != glm::vec4()) {
            return glm::normalize(v);
        }
        return glm::vec4();
    }    

    inline static bool readTextFile(const String & filepath, String & dst) {
        std::ifstream ifs(filepath.c_str());
        if (!ifs.good()) {
            return false;
        }
        
        std::basic_stringstream<char, std::char_traits<char>, ScopedAllocator<char>> ss;
        ss << ifs.rdbuf();
        ifs.close();
        dst = ss.str();

        return true;
    }

    inline static bool solveQuadratic(float a, float b, float c, float & r_v1, float & r_v2) {
        float s(b * b - 4.0f * a * c);
        if (s < 0.0f) {
            return false;
        }
        s = std::sqrt(s);
        float d(0.5f / a);
        r_v1 = (-s - b) * d;
        r_v2 = (s - b) * d;
        return true;
    }

    static double time() {
        using namespace std::chrono;
        static high_resolution_clock::time_point s_start(high_resolution_clock::now());
        return duration_cast<std::chrono::duration<double>>(high_resolution_clock::now() - s_start).count();
    }

    class Stopwatch {

        public:

        Stopwatch() :
            initT(time()),
            lapT(initT)
        {}

        double lap() {
            double last(lapT);
            lapT = time();
            return lapT - last;
        }

        double total() const {
            return time() - initT;
        }

        private:

        double initT, lapT;

    };

    using nat = intptr_t;

    template <typename T, typename std::enable_if<std::is_floating_point<T>::value, int>::type = 0>
    constexpr static nat floor(T v) {
        nat i = nat(v);
        return i - (v < i);
    }

    template <typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
    constexpr static T floor(T v) {
        return v;
    }

    template <typename T, typename std::enable_if<std::is_floating_point<T>::value, int>::type = 0>
    constexpr static nat ceil(T v) {
        nat i = nat(v);
        return i + (v > i);
    }

    template <typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
    constexpr static T ceil(T v) {
        return v;
    }

    template <typename T>
    constexpr static T pow2(int v) {
        return T(1) << v;
    }

    template <typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
    constexpr static bool isPow2(T v) {
        return (v & (v - 1)) == 0;
    }

    template <typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
    constexpr static T log2Floor(T v) {
        static_assert(sizeof(T) <= 8, "log2 function needs updated for larger integer types");

        T log(0);

        if (sizeof(T) >= 8)
            if (v & 0xFFFFFFFF00000000ULL) { v >>= 32; log += 32; }
        if (sizeof(T) >= 4)
            if (v & 0x00000000FFFF0000ULL) { v >>= 16; log += 16; }
        if (sizeof(T) >= 2)
            if (v & 0x000000000000FF00ULL) { v >>=  8; log +=  8; }
        if     (v & 0x00000000000000F0ULL) { v >>=  4; log +=  4; }
        if     (v & 0x000000000000000CULL) { v >>=  2; log +=  2; }
        if     (v & 0x0000000000000002ULL) {           log +=  1; }

        return log;
    }

    template <typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
    constexpr static T log2Ceil(T v) {
        return log2Floor(2 * v - 1);
    }

    template <typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
    constexpr static T floor2(T v) {
        return T(1) << log2Floor(v);
    }

    template <typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
    constexpr static T ceil2(T v) {
        return T(1) << log2Ceil(v);
    }

};



#endif
