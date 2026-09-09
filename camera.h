#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"
#include "hittable.h"
#include "hittable_list.h"
#include  "sphere.h"

// Two important jobs:
// 1. Construct and dispatch rays into the world
// 2. Use the results of these rays to construct the rendered image

class camera {
public:
    // Camera Parameters
    double aspect_ratio;
    int    image_width;

    void initialize() {

        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height; // Must be greater than 0
     
        // Camera
        auto focal_length    = 1.0;
        auto viewport_height = 2.0;
        auto viewport_width  = viewport_height * (double(image_width) / image_height);
        camera_center        = point3(0,0,0);

        // Viewport edge vectors
        auto viewport_u = vec3(viewport_width, 0, 0);
        auto viewport_v = vec3(0, -viewport_height, 0);

        // Pixel-to-pixel deltas
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;
        
        //
        auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

    void render(const hittable& world) {
        initialize();

        // PPM file header
        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
        
        for (int j = 0; j < image_height; j++) {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; i++) {            
                
                auto pixel_center  = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
                auto ray_direction = pixel_center - camera_center;
                ray r(camera_center, ray_direction);

                color pixel_color  = ray_color(r, world);  
                write_color(std::cout, pixel_color);
            }
        }   
        std::clog << "\nDone.               \n";
        
    }

private:
    int    image_height;
    point3 camera_center;
    point3 pixel00_loc;
    vec3   pixel_delta_u;
    vec3   pixel_delta_v;

    //ray   get_ray(const ray& r, const hittable& world) const) {
    //}

    color ray_color(const ray& r, const hittable& world) const {
        hit_record record; // Create hit record
        
        // Check if ray hits
        if (world.hit(r, interval(0, infinity), record)) {
            return 0.5 * (record.normal + color(1, 1, 1));
        }

        // Background
        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5*(unit_direction.y() + 1.0);
        return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
    }
};

#endif
