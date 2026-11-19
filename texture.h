#ifndef TEXTURE_H
#define TEXTURE_H

class texture {
public:
    virtual ~texture() = default;
    virtual color value(double u, double v, const point3& p) const = 0; // Pure Virtual Function
    // The "= 0" basically tells the compiler this virtual function has no implementation
};

#endif
