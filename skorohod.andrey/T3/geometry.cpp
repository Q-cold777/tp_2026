#include "geometry.hpp"
#include <math.h>

bool Point::operator==(const Point& other) const
{
    return x == other.x && y == other.y;
}

double Polygon::area() const
{
    if (points.size() < 3) return 0.0;
    double sum = 0.0;
    for (size_t i = 0; i < points.size(); ++i)
    {
        const Point& p1 = points[i];
        const Point& p2 = points[(i+1) % points.size()];
        sum += static_cast<double>(p1.x * p2.y - p2.x * p1.y);
    }
    return std::abs(sum) / 2.0;
}
