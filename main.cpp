#include "color.h"
#include "vec3.h"

#include <iostream>

int main() {
    // Image
    auto aspect_ratio  = 16.0 / 9.0;
    int image_width    = 400;
    int image_height   = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height; // Must be greater than 0
    
    auto viewport_height = 2.0;
    auto viewport_width  = viewport_height * (double(image_width) / image_height);

    // PPM file header
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    
    for (int j = 0; j < image_width; j++) {
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
        for (int i = 0; i < image_height; i++) {            
      
            // Normalize coordinates to a 0.0 to 1.0 range
            //auto r = double(i) / (image_width - 1);
            //auto g = double(j) / (image_height - 1);
            //auto b = 0.0;
            auto pixel_color = color(double(i) / (image_width - 1), double(j) / (image_height -1), 0);
    
            // Convert color values to 8-bit integers
            //int ir = int(255.999 * r);
            //int ig = int(255.999 * g); 
            //int ib = int(255.999 * b);

            // Output RGB triplet for the current pixel
            //std::cout << ir << ' ' << ig << ' ' << ib << '\n';
            write_color(std::cout, pixel_color);
        }
    }   
    std::clog << "\nDone.               \n";
}
