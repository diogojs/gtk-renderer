#ifndef RUDOLPH_CLIPPER_H
#define RUDOLPH_CLIPPER_H

#include "geometry.h"

#include <vector>

namespace rudolph {

enum class RegionCode {
    LEFT = 1,
    RIGHT = 2,
    UP = 4,
    DOWN = 8
};

enum class ClipMethod {
    COHEN_SUTHERLAND,
    LIANG_BARSKY
};

class Clipper {
    using Point3D = geometry::Point3D;
public:
    Clipper(ClipMethod method = ClipMethod::LIANG_BARSKY):
        _method{method}
    {}

    // Return false if point was not clipped (visible), true if clipped (hidden)
    bool clip_point(Point3D a);
    std::vector<Point3D> clip_line(Point3D a, Point3D b);
    std::vector<Point3D> clip_polygon(std::vector<Point3D>& points);
    std::vector<Point3D> clip_curve(std::vector<Point3D>& points);

private:
    ClipMethod _method;
    int region_code(Point3D p);
    std::vector<Point3D> cohen_sutherland(Point3D a, Point3D b);
    std::vector<Point3D> liang_barsky(Point3D a, Point3D b);
    void clip_pol_aux(std::vector<Point3D>& new_polygon, Point3D a, Point3D b);
    Point3D intersection(Point3D e1, Point3D e2, Point3D a, Point3D b);

    const double edge_left = 0;
    const double edge_right = 2;
    const double edge_up = 0;
    const double edge_down = 2;
    std::vector<Point3D> clip_window{
        Point3D{edge_left, edge_up},
        Point3D{edge_right, edge_up},
        Point3D{edge_right, edge_down},
        Point3D{edge_left, edge_down}
    };
};

} // end namespace rudolph

#endif
