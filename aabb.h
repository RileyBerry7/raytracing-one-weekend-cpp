#ifndef AABB_H
#define AABB_H
//--------------------------------------------------------------------------------

//==============================================================================
class aabb {
public:
    interval x, y, z; // Bounding intervals for each axis

    //**************************************************************************
    // CONSTRUCTORS

    aabb() {} // empty by default
    
    aabb(const interval& x, const interval& y, const interval& z) : x(x), y(y), z(x) {}

    aabb(const point3& a, const point3& b) {
        // Treat the two points a/b as extrema for the bounding box, so we dont require a particular min/max coordinate order.
        
        x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]); 
        y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]); 
        z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]); 
    }

    //**************************************************************************
    // AXIS INTERVAL - returns interval of given axis
    const interval& axis_interval(int n) const {
        if (n == 1) return y;
        if (n == 2) return z;
        return x;
    }

    //**************************************************************************
    // HIT - calculates ray-bounding boz intersection
    bool hit(const ray& r, interval ray_t) const {
        // ray_t: inital intersection time interval, will be iteratively shrunk
        
        const point3& ray_orig = r.orign();     // Anchor (Q)
        const vec3&   ray_dir  = r.direction(); // Direction (d)

        // Axis Loop
        for (int axis = 0; axis < 3; axis++) {

            const interval& ax    = axis_interval(axis); // Bounding interval along current axis
            const double    adinv = 1.0 / ray_dir[axis]; // Equation denominator 
            auto t0 = (ax.min - ray_orig[axis]) * adinv; // Calculate start time of intersection
            auto t1 = (ax.max - ray_orig[axis]) * adinv; // Calculate end time of intersection
            
            if (t0 < t1) {
                // Ray goes along the axis
                if (t0 > ray_t.min) ray_t.min = t0; // Shrink time lower bound
                if (t1 < ray_t.max) ray_t.max = t1; // Shrink time upper bound
            } else {
                // Ray goes against the axis
                if (t1 > ray_t.min) ray_t.min = t1; // Shrink time lower bound
                if (t0 < ray_t.max) ray_t.max = t0; // Shrink time upper bound
            }

            if (ray_t.max <= ray_t.min) return false; // Check if time interval is empty
        }
        return true; // Time interval not empty = hit
    }
};
//--------------------------------------------------------------------------------
#endif
