#pragma once
#include <ostream>

struct Sphere
{
    float radius;
    MATH::Vec3 center;

    friend std::ostream& operator<<(std::ostream& os, const Sphere& s)
    {
        os << "Sphere(center: (" << s.center.x << ", " << s.center.y << ", " << s.center.z << "), radius: " << s.radius << ")";
        return os;
    }
};
