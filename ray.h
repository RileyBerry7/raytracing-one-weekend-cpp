#ifndef RAY_H
#define RAY_H
//--------------------------------------------------------------------------------
#include "vec3.h"

//==============================================================================
class ray {
public:

    //**************************************************************************
    // CONSTRUCTORS
    
    ray() {} // Default

    // Origin, direction, time
    ray(const point3& origin, const vec3& direction, double time) 
        : orig(origin), dir(direction), tm(time) {}

    // Origin, direction
    ray(const point3& origin, const vec3& direction) 
        : ray(origin, direction, 0) {}

    //**************************************************************************
    // GETTERS
    const point3& origin()    const { return orig; }
    const vec3&   direction() const { return dir;  }
    double        time()      const { return tm;   }
    
    //**************************************************************************
    // GET POINT ALONG RAY
    point3 at(double t) const {
        return orig + t*dir;
    }
//==============================================================================
private:
    point3  orig; // ray source/origin
    vec3    dir;  // ray direction
    double  tm;   // ray time
};
//--------------------------------------------------------------------------------
#endif
