#include "Geometry.hpp"

#include "glm/gtx/component_wise.hpp"
#include "glm/gtx/norm.hpp"



namespace {

constexpr float k_collisionE = 0.000001f;

}



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
    
    if (Util::isGreater(d2, sphere.radius * sphere.radius, k_collisionE)) {
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
        glm::vec3 boxC(box.center());

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
    
    if (Util::isGreater(d2, cap.radius * cap.radius, k_collisionE)) {
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
        glm::vec3 boxC(box.center());

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
    
    if (Util::isGreater(d2, combR * combR, k_collisionE)) {
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

    if (Util::isGreater(d2, combR * combR, k_collisionE)) {
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

    if (Util::isGreater(d2, combR * combR, k_collisionE)) {
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
    glm::vec3 tsLow((box.min - ray.pos) * invDir);
    glm::vec3 tsHigh((box.max - ray.pos) * invDir);
    glm::vec3 tsMin(tsLow), tsMax(tsHigh);
    glm::bvec3 isLowHighsMin(false), isLowHighsMax(true);
    float tMinor, tMajor;
    glm::vec3 axisMinor, axisMajor;

    if (tsHigh.x < tsLow.x) { tsMin.x = tsHigh.x; isLowHighsMin.x = true; }
    if (tsHigh.y < tsLow.y) { tsMin.y = tsHigh.y; isLowHighsMin.y = true; }
    if (tsHigh.z < tsLow.z) { tsMin.z = tsHigh.z; isLowHighsMin.z = true; }
    if (tsLow.x > tsHigh.x) { tsMax.x = tsLow.x; isLowHighsMax.x = false; }
    if (tsLow.y > tsHigh.y) { tsMax.y = tsLow.y; isLowHighsMax.y = false; }
    if (tsLow.z > tsHigh.z) { tsMax.z = tsLow.z; isLowHighsMax.z = false; }

    if (tsMin.x > tsMin.y && tsMin.x > tsMin.z) {
        tMinor = tsMin.x;
        axisMinor.x = isLowHighsMin.x ? 1.0f : -1.0f;
    }
    else if (tsMin.y > tsMin.z) {
        tMinor = tsMin.y;
        axisMinor.y = isLowHighsMin.y ? 1.0f : -1.0f;
    }
    else {
        tMinor = tsMin.z;
        axisMinor.z = isLowHighsMin.z ? 1.0f : -1.0f;
    }

    if (tsMax.x < tsMax.y && tsMax.x < tsMax.z) {
        tMajor = tsMax.x;
        axisMajor.x = isLowHighsMax.x ? 1.0f : -1.0f;
    }
    else if (tsMax.y < tsMax.z) {
        tMajor = tsMax.y;
        axisMajor.y = isLowHighsMax.y ? 1.0f : -1.0f;
    }
    else {
        tMajor = tsMax.z;
        axisMajor.z = isLowHighsMax.z ? 1.0f : -1.0f;
    }

     // extra negation so that a NaN case returns no intersection
    if (tMajor <= 0.0f || !(tMajor > tMinor)) {
        return Intersect();
    }

    // exterior collision
    if (tMinor >= 0.0f) {
        return Intersect(tMinor, ray.pos + tMinor * ray.dir, axisMinor, true);
    }
    // interior collision
    else {
        return Intersect(tMajor, ray.pos + tMajor * ray.dir, axisMajor, false);
    }
}

Intersect intersect(const Ray & ray, const Sphere & sphere) {
    glm::vec3 C(sphere.origin - ray.pos);
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

    return Intersect(p - h, ray.pos + I, (I - C) / sphere.radius, face);
}

namespace {

Intersect intersectCylinderRelative(const Ray & ray, float r) {
    // coefficients for the quadratic intersection equation
    float a(ray.dir.x * ray.dir.x + ray.dir.z * ray.dir.z);
    float c(ray.pos.x * ray.pos.x + ray.pos.z * ray.pos.z - r * r);
    // ray parallel
    if (Util::isZero(a)) {
        // ray inside
        if (c < 0.0f) {
            Intersect inter;
            inter.face = false;
            return inter;
        }
        // ray outside
        else {
            return Intersect();
        }
    }
    float b(2.0f * (ray.dir.x * ray.pos.x + ray.dir.z * ray.pos.z));

    float t1, t2;
    // missed cylinder entirely
    if (!Util::solveQuadratic(a, b, c, t1, t2)) {
        return Intersect();
    }
    // cylinder is "behind" ray
    if (t2 <= 0) {
        return Intersect();
    }
    float t(t1);
    bool face(true);
    if (t <= 0) {
        t = t2;
        face = false;
    }

    glm::vec3 p(ray.pos + ray.dir * t);
    return Intersect(t, p, glm::vec3(p.x, 0.0f, p.z) / r, face);
}

Intersect intersectCylinder(const Ray & ray, const glm::vec3 & center, float r) {
    Intersect inter(intersectCylinderRelative(Ray(ray.pos - center, ray.dir), r));
    if (inter.is) inter.pos += center;
    return inter;
}

Intersect intersectPlane(const Ray & ray, const glm::vec3 & pos, const glm::vec3 & norm) {
    float R_dot_N(glm::dot(ray.dir, norm));

    if (Util::isZero(R_dot_N)) {
        return Intersect();
    }

    float h(glm::dot(ray.pos - pos, norm));

    if (h * R_dot_N >= 0.0f) {
        return Intersect();
    }

    float dist(h / -R_dot_N);
    bool face(h >= 0.0f);

    return Intersect(dist, ray.pos + dist * ray.dir, norm, face);
}

Intersect intersectUpperHemiRelative(const Ray & ray, float radius) {
    if (ray.pos.y < 0.0f) {
        if (ray.dir.y <= 0.0f) {
            return Intersect();
        }
        else {
            // ignore lower half of sphere by casting from xz plane intersection
            Intersect inter(intersectPlane(ray, glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f)));
            float dist(inter.dist);
            inter = (intersect(Ray(inter.pos, ray.dir), Sphere(glm::vec3(), radius)));
            inter.dist += dist;
            return inter;
        }
    }
    else {
        Intersect inter(intersect(ray, Sphere(glm::vec3(), radius)));
        if (inter.pos.y <= 0.0f) {
            return Intersect();
        }
        else {
            return inter;
        }
    }
}

Intersect intersectLowerHemiRelative(const Ray & ray, float r) {
    Intersect inter(intersectUpperHemiRelative(Ray(glm::vec3(ray.pos.x, -ray.pos.y, ray.pos.z), glm::vec3(ray.dir.x, -ray.dir.y, ray.dir.z)), r));
    inter.norm.y = -inter.norm.y;
    inter.pos.y = -inter.pos.y;
    return inter;
}

Intersect intersectUpperHemi(const Ray & ray, const Sphere & sphere) {
    Intersect inter(intersectUpperHemiRelative(Ray(ray.pos - sphere.origin, ray.dir), sphere.radius));
    if (inter.is) inter.pos += sphere.origin;
    return inter;
}

Intersect intersectLowerHemi(const Ray & ray, const Sphere & sphere) {
    Intersect inter(intersectLowerHemiRelative(Ray(ray.pos - sphere.origin, ray.dir), sphere.radius));
    if (inter.is) inter.pos += sphere.origin;
    return inter;
}

Intersect intersectCapsuleRelative(const Ray & ray, float h, float r) {
    Intersect inter(intersectCylinderRelative(ray, r));
    // didn't hit enclosing cylinder
    if (!inter.is && inter.face) {
        return Intersect();
    }

    float h_2(h * 0.5f); // half height
    glm::vec3 uo(0.0f, h_2, 0.0f); // upper cap origin
    glm::vec3 lo(0.0f, -h_2, 0.0f); // lower cap origin

    // ray outside cylinder
    if (inter.face) {
        // potential intersect in upper half
        if (inter.pos.y >= 0.0f) {
            // intersected rod
            if (inter.pos.y <= h_2) {
                return inter;
            }
            // may intersect lower cap
            else {
                return intersect(ray, Sphere(uo, r));
            }
        }
        // potential intersect in lower half
        else {
            // intersected rod
            if (inter.pos.y >= -h_2) {
                return inter;
            }
            // may intersect bottom cap
            else {
                return intersect(ray, Sphere(lo, r));
            }
        }
    }
    // ray inside cylinder
    // could intersect with zero or more of the rod, the upper cap, and the lower cap
    // figure out which, then find minimum
    else {
        bool checkUpper(false), checkLower(false);
        // edge case where ray is parallel to cylinder
        if (!inter.is) {
            if (ray.dir.y < 0.0f) {
                if (ray.pos.y > -(h_2 + r)) {
                    checkLower = true;
                }
                if (ray.pos.y > h_2) {
                    checkUpper = true;
                }
            }
            else {
                if (ray.pos.y < (h_2 + r)) {
                    checkUpper = true;
                }
                if (ray.pos.y < -h_2) {
                    checkLower = true;
                }
            }
        }
        // judge by position of ray
        if (ray.pos.y > h_2) {
            checkUpper = true;
        }
        else if (ray.pos.y < -h_2) {
            checkLower = true;
        }
        // judge by intersect with cylinder
        if (inter.pos.y > h_2) {
            inter = Intersect();
            checkUpper = true;
        }
        else if (inter.pos.y < -h_2) {
            inter = Intersect();
            checkLower = true;
        }
        // sort out the nearest intersect (or no intersect)
        if (checkUpper) {
            Intersect upper(intersectUpperHemi(ray, Sphere(uo, r)));
            if (upper.dist < inter.dist) inter = upper;
        }
        if (checkLower) {
            Intersect lower(intersectLowerHemi(ray, Sphere(lo, r)));
            if (lower.dist < inter.dist) inter = lower;
        }
        return inter;
    }
}

}

Intersect intersect(const Ray & ray, const Capsule & cap) {
    Intersect inter(intersectCapsuleRelative(Ray(ray.pos - cap.center, ray.dir), cap.height, cap.radius));
    if (inter.is) inter.pos += cap.center;
    return inter;
}

float distance(const Ray & r1, const Ray & r2) {
    glm::vec3 n(glm::cross(r1.dir, r2.dir));    
    // lines parallel
    if (Util::isZero(n)) {
        // nearest point on r2
        glm::vec3 p2(glm::dot(r1.pos - r2.pos, r2.dir) * r2.dir + r2.pos);
        return glm::length(r1.pos - p2);
    }
    // lines are askew
    return glm::abs(glm::dot(glm::normalize(n), r1.pos - r2.pos));
}

void nearestPoints(const Ray & r1, const Ray & r2, glm::vec3 & r_p1, glm::vec3 & r_p2) {
    glm::vec3 n(glm::cross(r1.dir, r2.dir));    
    // lines parallel
    if (Util::isZero(n)) {
        r_p1 = r1.pos;
        r_p2 = glm::dot(r1.pos - r2.pos, r2.dir) * r2.dir + r2.pos;
    }
    // lines are askew
    // http://morroworks.com/Content/Docs/Rays%20closest%20point.pdf
    glm::vec3 a(r1.dir);
    glm::vec3 b(r2.dir);
    glm::vec3 c(r2.pos - r1.pos);
    float ab(glm::dot(a, b));
    float ac(glm::dot(a, c));
    float bc(glm::dot(b, c));
    float denom(1.0f / (1.0f - ab * ab));
    float d1((ac - ab * bc) * denom);
    float d2((ab * ac - bc) * denom);
    r_p1 = r1.pos + r1.dir * d1;
    r_p2 = r2.pos + r2.dir * d2;
}