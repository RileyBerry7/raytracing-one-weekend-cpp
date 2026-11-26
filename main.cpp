#include "rtweekend.h"

#include "bvh.h"
#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "quad.h"
#include "sphere.h"
#include "texture.h"

#include <chrono>

void cornell_box(hittable_list& world, camera& cam) {

    auto red = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));
    world.add(make_shared<quad>(point3(555,0,0), vec3(0,555,0), vec3(0,0,555), green));
    world.add(make_shared<quad>(point3(0,0,0), vec3(0,555,0), vec3(0,0,555), red));
    world.add(make_shared<quad>(point3(343, 554, 332), vec3(-130,0,0), vec3(0,0,-105), light));
    world.add(make_shared<quad>(point3(0,0,0), vec3(555,0,0), vec3(0,0,555), white));
    world.add(make_shared<quad>(point3(555,555,555), vec3(-555,0,0), vec3(0,0,-555), white));
    world.add(make_shared<quad>(point3(0,0,555), vec3(555,0,0), vec3(0,555,0), white));

    cam.aspect_ratio = 1.0;
    cam.image_width = 200;
    cam.samples_per_pixel = 80;
    cam.max_depth = 30;
    //cam.background = color(0,0,0);
    cam.vfov = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat = point3(278, 278, 0);
    cam.vup = vec3(0,1,0);
    cam.defocus_angle = 0;
    cam.background    = color(0.1,0.1,0.1);

    cam.render(world);
}

void spheres(hittable_list& world, camera& cam){

    // Earth texture test
    auto earth_texture = make_shared<image_texture>("earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);

    auto checker = make_shared<checker_texture>(0.32, color(0.2, 0.2, 0.1), color(0.9, 0.9, 0.9));
    auto ground_material = make_shared<lambertian>(checker);
    //world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    world.add(make_shared<quad>(point3(-20, 0, -20), vec3(40, 0,0), vec3(0, 0, 40), ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;
            
                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2 = center + vec3(0, random_double(0,0.5), 0);
                    //world.add(make_shared<sphere>(center, center2, 0.2, sphere_material)); // Dynamic
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));            // Static
                } else if (choose_mat < 0.95) {
                    // Metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz   = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0),  1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0),  1.0, material2));

    auto material3   = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, earth_surface));

    // Generate BVH (Initates recursion)
    world = hittable_list(make_shared<bvh_node>(world)); // 6.5x to 9.2x speed up

    //auto material_bubble = make_shared<dielectric>(1.00 / 1.50);
    //world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.4, material_bubble));
    //
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 50;
    cam.max_depth         = 20;

    cam.vfov              = 20;
    cam.lookfrom          = point3(13,2,3);
    cam.lookat            = point3(0,0,0);
    cam.vup               = vec3(0,1,0);

    cam.defocus_angle     = 0.6; // 
    cam.focus_distance    = 10.0;

    cam.render(world);
}

//==============================================================================
int main() {

    // Start timer
    auto start = std::chrono::steady_clock::now();

    hittable_list world; // Initialize world
    camera cam;          // Initialize camera
    
    // Render scene
    cornell_box(world, cam);


    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    
    int pixel_count = cam.image_width * cam.image_width / cam.aspect_ratio;
    std::clog << "Pixel Count  : " << pixel_count << std::endl;
    std::clog << "Samples/px   : " << cam.samples_per_pixel << std::endl;
    std::clog << "Max Ray Depth: " << cam.max_depth << std::endl;
    std::clog << "Runtime      : " << elapsed.count() << " seconds" << std::endl;
    return 0;
}
//******************************************************************************
