#ifndef HITTABLE_H
#define HITTABLE_H

#include "rtweekend.h"

class hit_record {
public:
    point3 p;           // Intersection point
    vec3   normal;      // Surface normal (outwards/unit-vector)
    double t;           // Distance from ray origin
    bool   front_face;  // Whether or not front face was hit

    void set_face_normal(const ray& r, const vec3& outward_normal) {

        front_face = dot(r.direction(), outward_normal) < 0;        // Checks side hit
        normal     = front_face ? outward_normal : -outward_normal; // Checks normal direction
    }    
};

class hittable {
public:
    virtual ~hittable() = default;

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};

#endif
