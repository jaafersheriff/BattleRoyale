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
        return 4.0f / 3.0f * glm::pi<float>() * radius * radius * radius;
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
        return glm::pi<float>() * radius * radius * (height + 4.0f / 3.0f * radius);
    }

};

// An intersection represents the intersection point of an object and a ray.
struct Intersect {    

    bool is; // if there was an intersection
    float dist; // the distance from the ray origin to the intersection point
    glm::vec3 loc; // the intersection point on the surface of the object
    glm::vec3 norm; // the normal of the surface of the object at the point of intersection
    bool face; // true if the ray hit the outside surface, false if inside surface

    Intersect() :
        is(false),
        dist(std::numeric_limits<float>::infinity()),
        loc(),
        norm(),
        face(true)
    {}

    Intersect(
        float dist,
        const glm::vec3 & loc,
        const glm::vec3 & norm,
        bool face
    ) :
        is(true),
        dist(dist),
        loc(loc),
        norm(norm),
        face(face)
    {}

};



// Returns whether or not there is a collision
// If delta is not null, it is set to the delta the first object should move
// from the second object so that there is no longer a collision
bool collide(const AABox & box1, const AABox & box2, glm::vec3 * delta);
bool collide(const AABox & box1, const Sphere & sphere2, glm::vec3 * delta);
bool collide(const AABox & box1, const Capsule & cap2, glm::vec3 * delta);
bool collide(const Sphere & sphere1, const Sphere & sphere2, glm::vec3 * delta);
bool collide(const Sphere & sphere1, const Capsule & cap2, glm::vec3 * delta);
bool collide(const Capsule & cap1, const Capsule & cap2, glm::vec3 * delta);

// Calculates the intersection between the ray and the object
Intersect intersect(const Ray & ray, const AABox & box);
Intersect intersect(const Ray & ray, const Sphere & sphere);
Intersect intersect(const Ray & ray, const Capsule & cap);