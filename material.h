#ifndef MATERIAL_H
#define MATERIAL_H
//------------------------------------------------------------------------------------------------------
#include "hittable.h"

//------------------------------------------------------------------------------------------------------
// This class has two important jobs:
// 1. Produce a scattered ray (or say it absorbed the incident ray).
// 2. If scattered, say how much the ray should be attenuated. 
//=======================================================================================================
class material {
public:
    // Virtual destructor
    virtual ~material() = default;

    // Virtual scatter
    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const {
        return false;
    }
};


//=======================================================================================================
// DIFFUSE (MATTE)
class lambertian : public material {
public:
    // Constructor
    lambertian(const color& albedo) : albedo(albedo) {}

    //***************************************************************************************************
    // DIFFUSE SCATTER
    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        //
        // Generate random scatter direction
        auto scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate directions
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal; // Tiny rays may be rounded to zero vector

        // Save scattered ray
        scattered   = ray(rec.p, scatter_direction);
        attenuation = albedo;

        return true; // 
    }
private:
    color albedo;
};

//=======================================================================================================
// METAL (REFLECTIVE)
class metal : public material {
public:
    // Constructor
    metal(const color& albedo) : albedo(albedo) {}

    //***************************************************************************************************
    // METAL SCATTER
    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        
        // Reflect incident ray
        vec3 reflection = reflect(r_in.direction(), rec.normal);
        
        // Save scattered ray
        scattered       = ray(rec.p, reflection);
        attenuation     = albedo;
        return true;
    }
private:
    color albedo;
};

//--------------------------------------------------------------------------------
#endif

