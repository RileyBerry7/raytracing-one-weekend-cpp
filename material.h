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

        // Return reflection status
        return true; //  Always true
    }
private:
    color albedo;
};

//=======================================================================================================
// METAL (REFLECTIVE)
class metal : public material {
public:
    // Constructor
    metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    //***************************************************************************************************
    // METAL SCATTER
    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        
        // Reflect incident ray
        vec3 reflection = reflect(r_in.direction(), rec.normal);

        // Add fuzz
        reflection = unit_vector(reflection) + (fuzz * random_unit_vector());
        
        // Save scattered ray
        scattered   = ray(rec.p, reflection);
        attenuation = albedo;

        // Return reflection status
        return (dot(scattered.direction(), rec.normal) > 0); // True when ray & normal point in same direction
    }
private:
    color albedo;
    double fuzz;
};


//=======================================================================================================
// GLASS (TRANSPARENT)
class dielectric : public material {
public:
    // Constructor
    dielectric(double refraction_index) : refraction_index(refraction_index) {}

    //***************************************************************************************************
    // DIALECTRIC SCATTER
    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {

        attenuation = color(1.0, 1.0, 1.0);
        double ri   = rec.front_face ? (1.0/refraction_index) : refraction_index;

        vec3 unit_direction = unit_vector(r_in.direction());
        vec3 refracted      = refract(unit_direction, rec.normal, ri);

        // Save scattered ray
        scattered = ray(rec.p, refracted);

        // Return reflection status
        return true; //  Always true
    }
private:
    double refraction_index; // may be a ration of ri/ri of enclosing medium
};


//--------------------------------------------------------------------------------
#endif

