#ifndef CAMERA_H
#define CAMERA_H
//--------------------------------------------------------------------------------
#include "rtweekend.h"
#include "hittable.h"
#include "hittable_list.h" // maybe not needed
#include  "sphere.h"       // maybe not needed 
#include "material.h"
//--------------------------------------------------------------------------------
// This class has two important jobs:
// 1. Construct and dispatch rays into the world
// 2. Use the results of these rays to construct the rendered image
//==============================================================================
class camera {
public:
    double aspect_ratio      = 1.0;
    int    image_width       = 100;
    int    samples_per_pixel = 10;
    int    max_depth         = 10;
    double vfov              = 90; // Vertical view angle (fov)

    //**************************************************************************
    void initialize() {

        image_height         = int(image_width / aspect_ratio);
        image_height         = (image_height < 1) ? 1 : image_height; // Must be greater than 0
        pixel_sample_scale   = 1.0 / samples_per_pixel;
        auto focal_length    = 1.0;
        auto theta           = degrees_to_radians(vfov);
        auto h               = std::tan(theta / 2);
        auto viewport_height = 2 * h  * focal_length;
        //auto viewport_height = 2.0; // OLD 
        //
        auto viewport_width  = viewport_height * (double(image_width) / image_height);
        camera_center        = point3(0,0,0);
        auto viewport_u      = vec3(viewport_width, 0, 0);  // Viewport edge vectors
        auto viewport_v      = vec3(0, -viewport_height, 0);//
        pixel_delta_u        = viewport_u / image_width;    // Pixel-to-pixel deltas
        pixel_delta_v        = viewport_v / image_height;   //
        
        auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
        pixel00_loc              = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }
    //**************************************************************************
    void render(const hittable& world) {
        initialize();
        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n"; // PPM file header
        
        // Pixel Row Loop
        for (int j = 0; j < image_height; j++) {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush; // Progress indicator
            
            // Pixel Column Loop
            for (int i = 0; i < image_width; i++) {            
                color pixel_color(0,0,0);
                
                // Per-pixel Sample Loop
                for (int sample = 0; sample < samples_per_pixel; sample++) {
                    ray r = get_ray(i, j);                         // r is randomly sampled from pixel
                    pixel_color += ray_color(r, max_depth, world); // Accumulate colors
                }

                write_color(std::cout, pixel_sample_scale * pixel_color); // Write avg color to pixel
            }
        }   
        std::clog << "\nDone.               \n";
        
    }
//==============================================================================
private:
    int    image_height;
    double pixel_sample_scale;
    point3 camera_center;
    point3 pixel00_loc;
    vec3   pixel_delta_u;
    vec3   pixel_delta_v;

    //**************************************************************************
    // GET RAY - generates a randomly sampled ray for the given pixel
    ray   get_ray(int i, int j) const { 

        auto offset        = sample_square();
        auto pixel_sample  = pixel00_loc 
                                + ((i + offset.x()) * pixel_delta_u) 
                                + ((j + offset.y()) * pixel_delta_v); // Apply random offset
        
        auto ray_origin    = camera_center;             // eye (0,0,0)
        auto ray_direction = pixel_sample - ray_origin; // tip - tail

        return ray(ray_origin, ray_direction);
    }
    //**************************************************************************
    // SAMPLE SQUARE - returns a random vector within the unit square
    vec3 sample_square() const {
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }
    //**************************************************************************
    // RAY COLOR - recursively determines the color of a ray
    color ray_color(const ray& r, int depth, const hittable& world) const {

        if (depth <0) return color(0,0,0); // Bounce limit exceeded
        hit_record record;                 // Create hit record
        
        // Ray hit
        if (world.hit(r, interval(0.001, infinity), record)) { // Min value prevents shadow acne

            ray scattered;
            color attenuation;
            if (record.mat->scatter(r, record, attenuation, scattered))
                return attenuation * ray_color(scattered, depth-1, world);
            return color(0,0,0);

            //vec3 direction = record.normal + random_unit_vector();            // Lambertian diffuse
            //return 0.5 * ray_color(ray(record.p, direction), depth-1, world); // Light bounce(recursive)
        }
        // Ray miss (background)
        vec3 unit_direction = unit_vector(r.direction());             //
        auto a              = 0.5*(unit_direction.y() + 1.0);         // Linear interpolation 
        return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0); // Sky color
    }
};
//--------------------------------------------------------------------------------
#endif
