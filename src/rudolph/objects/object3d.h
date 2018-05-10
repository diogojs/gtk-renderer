#ifndef RUDOLPH_OBJECTS_3D_H
#define RUDOLPH_OBJECTS_3D_H

#include <vector>
#include "../render.h"
#include "../geometry.h"

namespace rudolph {
namespace objects {

using Point3D = geometry::Point3D;
using Edge = geometry::Edge;

class Object3D {
public:
    Object3D(std::vector<Point3D> points, std::vector<Edge> edges):
        _points{std::move(points)},
        _edges{std::move(edges)},
        scn_points{_points},
        scn_valid{false},
        _id{objects_id++},
        _name{"object" + std::to_string(_id)}
    {}

    void draw(RenderTarget&);

    std::string name() const {
        return _name;
    }

    Point3D center() const;

    void translate(double dx, double dy);

    void scale(double sx, double sy);

    void rotate_origin(double angle);
    void rotate_pin(double angle, Point3D pin);
    void rotate_center(double angle);

private:
    std::vector<Point3D> _points;
    std::vector<Point3D> scn_points;
    std::vector<Edge> _edges;
    bool scn_valid;
    unsigned _id;
    std::string _name;
    
    const std::string _type{"Object"};
    static unsigned int objects_id;
};

}
}

#endif
