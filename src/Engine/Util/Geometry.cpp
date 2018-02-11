#include "Geometry.hpp"

#include "glm/gtx/component_wise.hpp"
#include "glm/gtx/norm.hpp"



bool collide(const AABox & box1, const AABox & box2, glm::vec3 * delta_) {
    if (
        box1.min.x >= box2.max.x || box1.max.x <= box2.min.x ||
        box1.min.y >= box2.max.y || box1.max.y <= box2.min.y ||
        box1.min.z >= box2.max.z || box1.max.z <= box2.min.z
    ) {
        return false;
    }
    if (!delta_) {
        return true;
    }

    glm::vec3 delta1(box2.max - box1.min);
    glm::vec3 delta2(box2.min - box1.max);
    glm::vec3 delta(
        glm::abs(delta1.x) <= glm::abs(delta2.x) ? delta1.x : delta2.x,
        glm::abs(delta1.y) <= glm::abs(delta2.y) ? delta1.y : delta2.y,
        glm::abs(delta1.z) <= glm::abs(delta2.z) ? delta1.z : delta2.z
    );

    // preserve only minimum component
    if (glm::abs(delta.x) < glm::abs(delta.z)) {
        delta.z = 0.0f;
        if (glm::abs(delta.x) < glm::abs(delta.y)) {
            delta.y = 0.0f;
        }
        else {
            delta.x = 0.0f;
        }
    }
    else {
        delta.x = 0.0f;
        if (glm::abs(delta.z) < glm::abs(delta.y)) {
            delta.y = 0.0f;
        }
        else {
            delta.z = 0.0f;
        }
    }

    *delta_ = delta;

    return true;
}

bool collide(const AABox & box, const Sphere & sphere, glm::vec3 * delta_) {
    glm::vec3 p(glm::clamp(sphere.origin, box.min, box.max));    
    glm::vec3 dVec(p - sphere.origin);
    float d2(glm::length2(dVec));
    
    if (d2 >= sphere.radius * sphere.radius) {
        return false;
    }
    if (!delta_) {
        return true;
    }

    glm::vec3 delta;
    // sphere center is outside box
    if (!Util::isZero(d2)) {
        float d(std::sqrt(d2));
        delta = (sphere.radius - d) * (dVec / d);
    }
    // sphere center is inside box
    else {
        // nearest corner
        p.x = sphere.origin.x - box.min.x <= box.max.x - sphere.origin.x ? box.min.x : box.max.x;
        p.y = sphere.origin.y - box.min.y <= box.max.y - sphere.origin.y ? box.min.y : box.max.y;
        p.z = sphere.origin.z - box.min.z <= box.max.z - sphere.origin.z ? box.min.z : box.max.z;

        delta = sphere.origin - p;
        glm::vec3 aDelta(glm::abs(delta));
        glm::vec3 boxC(box.min + (box.max - box.min) * 0.5f);

        // preserve onlz minimum component and add radius
        if (aDelta.x < aDelta.z) {
            delta.z = 0.0f;
            if (aDelta.x < aDelta.y) {
                delta.y = 0.0f;
                delta.x += sphere.radius * glm::sign(boxC.x - sphere.origin.x);
            }
            else {
                delta.x = 0.0f;
                delta.y += sphere.radius * glm::sign(boxC.y - sphere.origin.y);
            }
        }
        else {
            delta.x = 0.0f;
            if (aDelta.z < aDelta.y) {
                delta.y = 0.0f;
                delta.z += sphere.radius * glm::sign(boxC.z - sphere.origin.z);
            }
            else {
                delta.z = 0.0f;
                delta.y += sphere.radius * glm::sign(boxC.y - sphere.origin.y);
            }
        }
    }

    *delta_ = delta;

    return true;
}

bool collide(const AABox & box, const Capsule & cap, glm::vec3 * delta_) {
    glm::vec3 boxP(glm::clamp(cap.center, box.min, box.max));
    glm::vec3 lowP(cap.center); lowP.y -= cap.height * 0.5f;
    glm::vec3 highP(cap.center); highP.y += cap.height * 0.5f;
    glm::vec3 rodP(cap.center.x, glm::clamp(boxP.y, lowP.y, highP.y), cap.center.z);
    glm::vec3 dVec(boxP - rodP);
    float d2(glm::length2(dVec));
    
    if (d2 >= cap.radius * cap.radius) {
        return false;
    }
    if (!delta_) {
        return true;
    }

    glm::vec3 delta;
    // rod core is outside box
    if (!Util::isZero(d2)) {
        float d(std::sqrt(d2));
        delta = (cap.radius - d) * (dVec / d);
    }
    // rod core is at least partially inside box
    else {
        glm::vec3 boxC(box.min + (box.max - box.min) * 0.5f);

        if (cap.center.y < boxC.y) {
            boxP.y = glm::clamp(highP.y, box.min.y, box.max.y);
        }
        else {
            boxP.y = glm::clamp(lowP.y, box.min.y, box.max.y);
        }

        // nearest corner
        glm::vec3 c(
            boxP.x - box.min.x <= box.max.x - boxP.x ? box.min.x : box.max.x,
            boxP.y - box.min.y <= box.max.y - boxP.y ? box.min.y : box.max.y,
            boxP.z - box.min.z <= box.max.z - boxP.z ? box.min.z : box.max.z
        );

        delta = boxP - c;
        glm::vec3 aDelta(glm::abs(delta));

        // preserve only minimum component and add radius
        if (aDelta.x < aDelta.z) {
            delta.z = 0.0f;
            if (aDelta.x < aDelta.y) {
                delta.y = 0.0f;
                delta.x += cap.radius * glm::sign(boxC.x - boxP.x);
            }
            else {
                delta.x = 0.0f;
                if (boxP.y - boxC.y > 0) {
                    delta.y = lowP.y - box.max.y - cap.radius;
                }
                else {
                    delta.y = highP.y - box.min.y + cap.radius;
                }
            }
        }
        else {
            delta.x = 0.0f;
            if (aDelta.z < aDelta.y) {
                delta.y = 0.0f;
                delta.z += cap.radius * glm::sign(boxC.z - boxP.z);
            }
            else {
                delta.z = 0.0f;
                if (boxP.y - boxC.y > 0) {
                    delta.y = lowP.y - box.max.y - cap.radius;
                }
                else {
                    delta.y = highP.y - box.min.y + cap.radius;
                }
            }
        }
    }

    *delta_ = delta;

    return true;
}

bool collide(const Sphere & sphere1, const Sphere & sphere2, glm::vec3 * delta) {
    float combR(sphere1.radius + sphere2.radius);
    glm::vec3 dVec(sphere1.origin - sphere2.origin);
    float d2(glm::length2(dVec));
    
    if (d2 >= combR * combR) {
        return false;
    }
    if (!delta) {
        return true;
    }

    if (Util::isZero(d2)) {
        *delta = glm::vec3(0.0f, combR, 0.0f);
    }
    else {
        float d(std::sqrt(d2));
        *delta = (combR - d) * (dVec / d);
    }

    return true;
}

bool collide(const Sphere & sphere, const Capsule & cap, glm::vec3 * delta) {
    float combR(cap.radius + sphere.radius);
    glm::vec3 lowP(cap.center); lowP.y -= cap.height * 0.5f;
    glm::vec3 highP(cap.center); highP.y += cap.height * 0.5f;
    glm::vec3 rodP(cap.center.x, glm::clamp(sphere.origin.y, lowP.y, highP.y), cap.center.z);
    glm::vec3 dVec(sphere.origin - rodP);
    float d2(glm::length2(dVec));

    if (d2 >= combR * combR) {
        return false;
    }
    if (!delta) {
        return true;
    }

    if (Util::isZero(d2)) {
        if (sphere.origin.y >= cap.center.y) {
            *delta = glm::vec3(0.0f, 0.0f, highP.y - sphere.origin.y + combR);
        }
        else {
            *delta = glm::vec3(0.0f, 0.0f, lowP.y - sphere.origin.y - combR);
        }
    }
    else {
        float d(std::sqrt(d2));
        *delta = (combR - d) * (dVec / d);
    }

    return true;
}

bool collide(const Capsule & cap1, const Capsule & cap2, glm::vec3 * delta) {
    float combR(cap1.radius + cap2.radius);
    glm::vec3 lowP1(cap1.center); lowP1.y -= cap1.height * 0.5f;
    glm::vec3 highP1(cap1.center); highP1.y += cap1.height * 0.5f;
    glm::vec3 lowP2(cap2.center); lowP2.y -= cap2.height * 0.5f;
    glm::vec3 highP2(cap2.center); highP2.y += cap2.height * 0.5f;
    glm::vec3 rodP1(cap1.center.x, glm::clamp(cap2.center.y, lowP1.y, highP1.y), cap1.center.z);
    glm::vec3 rodP2(cap2.center.x, glm::clamp(rodP1.y, lowP2.y, highP2.y), cap2.center.z);
    glm::vec3 dVec(rodP1 - rodP2);
    float d2(glm::length2(dVec));

    if (d2 >= combR * combR) {
        return false;
    }
    if (!delta) {
        return true;
    }

    if (Util::isZero(d2)) {
        if (cap1.center.y >= cap2.center.y) {
            *delta = glm::vec3(0.0f, 0.0f, highP2.y - lowP1.y + combR);
        }
        else {
            *delta = glm::vec3(0.0f, 0.0f, lowP2.y - highP1.y - combR);
        }
    }
    else {
        float d(std::sqrt(d2));
        *delta = (combR - d) * (dVec / d);
    }

    return true;
}

Intersect intersect(const Ray & ray, const AABox & box) {

    glm::vec3 invDir(1.0f / ray.dir);

    glm::vec3 tsLow((box.min - ray.loc) * invDir);
    glm::vec3 tsHigh((box.max - ray.loc) * invDir);
    glm::vec3 tsMin(tsLow), tsMax(tsHigh);
    glm::bvec3 isLowHighsMin(false), isLowHighsMax(true);
    float tMinor, tMajor;
    int axisMinor, axisMajor;
    bool isLowHighMin, isLowHighMax;

    if (tsHigh.x < tsLow.x) { tsMin.x = tsHigh.x; isLowHighsMin.x = true; }
    if (tsHigh.y < tsLow.y) { tsMin.y = tsHigh.y; isLowHighsMin.y = true; }
    if (tsHigh.z < tsLow.z) { tsMin.z = tsHigh.z; isLowHighsMin.z = true; }
    if (tsLow.x > tsHigh.x) { tsMax.x = tsLow.x; isLowHighsMax.x = false; }
    if (tsLow.y > tsHigh.y) { tsMax.y = tsLow.y; isLowHighsMax.y = false; }
    if (tsLow.z > tsHigh.z) { tsMax.z = tsLow.z; isLowHighsMax.z = false; }

    if (tsMin.x > tsMin.y) {
        if (tsMin.x > tsMin.z) {
            tMinor = tsMin.x;
            axisMinor = 0;
            isLowHighMin = isLowHighsMin.x;
        }
        else {
            tMinor = tsMin.z;
            axisMinor = 2;
            isLowHighMin = isLowHighsMin.z;
        }
    }
    else {
        if (tsMin.y > tsMin.z) {
            tMinor = tsMin.y;
            axisMinor = 1;
            isLowHighMin = isLowHighsMin.y;
        }
        else {
            tMinor = tsMin.z;
            axisMinor = 2;
            isLowHighMin = isLowHighsMin.z;
        }
    }

    if (tsMax.x < tsMax.y) {
        if (tsMax.x < tsMax.z) {
            tMajor = tsMax.x;
            axisMajor = 0;
            isLowHighMax = isLowHighsMax.x;
        }
        else {
            tMajor = tsMax.z;
            axisMajor = 2;
            isLowHighMax = isLowHighsMax.z;
        }
    }
    else {
        if (tsMax.y < tsMax.z) {
            tMajor = tsMax.y;
            axisMajor = 1;
            isLowHighMax = isLowHighsMax.y;
        }
        else {
            tMajor = tsMax.z;
            axisMajor = 2;
            isLowHighMax = isLowHighsMax.z;
        }
    }

     // extra negation so that a NaN case returns no intersection
    if (tMajor <= 0.0f || !(tMajor > tMinor)) {
        return Intersect();
    }

    // exterior collision
    if (tMinor >= 0.0f) {
        glm::vec3 loc(ray.loc + tMinor * ray.dir);
        glm::vec3 norm(Util::axisVec(axisMinor, isLowHighMin));

        return Intersect(true, tMinor, loc, norm, true);
    }
    // interior collision
    else {
        glm::vec3 loc(ray.loc + tMajor * ray.dir);
        glm::vec3 norm(Util::axisVec(axisMajor, isLowHighMax));

        return Intersect(true, tMajor, loc, norm, false);
    }
}

Intersect intersect(const Ray & ray, const Sphere & sphere) {
    glm::vec3 C(sphere.origin - ray.loc);
    float rad2(sphere.radius * sphere.radius);
    bool face(glm::length2(C) - rad2 >= 0.0f);

    float p(glm::dot(ray.dir, C));

    if (face && p <= 0.0f) {
        return Intersect();
    }

    glm::vec3 P(p * ray.dir);
    float d2(glm::length2(P - C));

    if (d2 >= rad2) {
        return Intersect();
    }

    float h(face ? std::sqrt(rad2 - d2) : -std::sqrt(rad2 - d2));
    glm::vec3 I(P - h * ray.dir);

    return Intersect(true, p - h, ray.loc + I, (I - C) / sphere.radius, face);
}

Intersect intersect(const Ray & ray, const Capsule & cap) {
    // TODO: figure this out
    return Intersect();
}