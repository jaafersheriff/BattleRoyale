#pragma once



#include "glm/glm.hpp"
#include "Util.hpp"



struct Ray {

    glm::vec3 loc;
    glm::vec3 dir;

    Ray() :
        loc(),
        dir()
    {}

    Ray(const glm::vec3 & loc, const glm::vec3 & dir) :
        loc(loc),
        dir(dir)
    {}

};



struct AABox {

    glm::vec3 min;
    glm::vec3 max;

    AABox() :
        min(),
        max()
    {}

    AABox(const glm::vec3 & min, const glm::vec3 & max) :
        min(min),
        max(max)
    {}

    AABox(const glm::vec3 & min, glm::vec3 & max) : 
        min(min),
        max(max)
    {}

    float volume() const {
        glm::vec3 size(max - min);
        return size.x * size.y * size.z;
    }

};



struct Sphere {

    glm::vec3 origin;
    float radius;

    Sphere() :
        origin(),
        radius(0.0f)
    {}

    Sphere(const glm::vec3 & origin, float radius) :
        origin(origin),
        radius(radius)
    {}

    float volume() const {
        return 4.0f / 3.0f * float(Util::PI) * radius * radius * radius;
    }

};



// A capsule looks like a pill. It is comprised of two hemispheres sandwiching
// a cylinder. A capsule is always upright, with the cylinder parallel to the
// y axis.
struct Capsule {

    glm::vec3 center; // the center point of the cylinder
    float radius; // the radius of both the cylinder and the hemispheres
    float height; // the height of the cylinder

    Capsule() :
        center(),
        radius(0.0f),
        height(0.0f)
    {}

    Capsule(const glm::vec3 & center, float radius, float height) :
        center(center),
        radius(radius),
        height(height)
    {}

    float volume() const {
        return float(Util::PI) * radius * radius * (height + 4.0f / 3.0f * radius);
    }

};