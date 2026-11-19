#ifndef BVH_H
#define BVH_H

#include "aabb.h"
#include "hittable.h"
#include "hittable_list.h"

#include <algorithm>

class bvh_node : public hittable {

public:
    //******************************************************************************************************************
    // Root Constructor (Recursive)
    bvh_node(hittable_list list) : bvh_node(list.objects, 0, list.objects.size()) {
        // There is a C++ subtlety here. This constructor (without span indices) creates an implicit copy of the
        // hittable list, which we will modify. The lifetime of the copied list only extends until this constructor exits.
        // This is okay, because we only need to persist the resulting bounding hierarchy.
    }

    //******************************************************************************************************************
    // Internal Node Constructor (Recursive)
    bvh_node(std::vector<shared_ptr<hittable>>& objects, size_t start, size_t end) {
        
        // Randomly choose an axis to split on
        int axis = random_int(0, 2);
        
        // Ensure bounding box exists
        auto comparator = (axis == 0) ? box_x_compare // TODO: Must define these comparator functions
                        : (axis == 1) ? box_y_compare
                                      : box_z_compare;
        // Quantity of input elements 
        size_t object_span = end - start;
        
        // Exactly 1 element
        if (object_span == 1) {
            left = right = objects[start]; // Duplicate primitive
            // End recursion

        // Exactly 2 elements
        } else if (object_span == 2) {
            left  = objects[start];    
            right = objects[start+1]; // Assign primitives as leaf nodes
            // End recursion
        
        // More than 2 elements
        } else {
            std::sort(std::begin(objects) + start, std::begin(objects) + end, comparator);
            auto mid = start + object_span / 2; // Cut input in half, divide into two subtrees
            left     = std::make_shared<bvh_node>(objects, start, mid); // Recursive call
            right    = std::make_shared<bvh_node>(objects, mid, end);   // Recursive call
        }
        // Calculate new bounding box
        bbox = aabb(left->bounding_box(), right->bounding_box());
    }

    //******************************************************************************************************************
    // Check For Hit - Override
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        if (!bbox.hit(r, ray_t))
            return false;

        bool hit_left  = left->hit(r, ray_t, rec);
        bool hit_right = right->hit(r, interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);

        return hit_left || hit_right;
    }

    //******************************************************************************************************************
    // Get Bounding Box - Override
    aabb bounding_box() const override { return bbox; }

    //******************************************************************************************************************
    // BVH_NODE ATTRIBUTES
private:
    shared_ptr<hittable> left;  // Left child
    shared_ptr<hittable> right; // Right child
    aabb                 bbox;  // Bounding box

    //******************************************************************************************************************
    // Bounding Box Comparator Functions
    
    static bool box_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b, int axis_index) {
        auto a_axis_interval = a->bounding_box().axis_interval(axis_index);
        auto b_axis_interval = b->bounding_box().axis_interval(axis_index);
        return a_axis_interval.min < b_axis_interval.min;
    }

    static bool box_x_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b) {
        return box_compare(a, b, 0); 
    }
    static bool box_y_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b) {
        return box_compare(a, b, 1); 
    }
    static bool box_z_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b) {
        return box_compare(a, b, 2); 
    }
};
//________________________________________________________________________________________________________________________
#endif
