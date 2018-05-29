#ifndef RUDOLPH_CAMERA_H
#define RUDOLPH_CAMERA_H

#include "../geometry.h"
#include "object3d.h"

namespace rudolph {

class CameraWindow: public Object3D {
    using Point3D = geometry::Point3D;
    using Edge = geometry::Edge;
    using Face = geometry::Face;
public:
    CameraWindow():
        Object3D{
            std::vector<Point3D>{Point3D(0, 0), Point3D(26, 0), Point3D(26, 26), Point3D(0, 26)},
            std::vector<Edge>{std::make_pair(0, 1), std::make_pair(1, 2), std::make_pair(2, 3), std::make_pair(3, 0)},
            std::vector<Face>{Face( 0, 1, 2 ), Face(0, 2, 3)},
            "Camera"
        },
        scn_cam{
            std::vector<Point3D>{Point3D(0, 0), Point3D(26, 0), Point3D(26, 26), Point3D(0, 26)},
            std::vector<Edge>{std::make_pair(0, 1), std::make_pair(1, 2), std::make_pair(2, 3), std::make_pair(3, 0)},
            std::vector<Face>{Face( 0, 1, 2 ), Face(0, 2, 3)},
            "SCNCamera"
        },
        _angle{0}
    {}

    CameraWindow(Point3D bottom_left, Point3D top_right, Point3D top_left = {}, Point3D bottom_right = {}):
        Object3D{
            std::vector<Point3D>{bottom_left, bottom_right, top_right, top_left},
            std::vector<Edge>{std::make_pair(0, 1), std::make_pair(1, 2), std::make_pair(2, 3), std::make_pair(3, 0)},
            std::vector<Face>{Face( 0, 1, 2 ), Face(0, 2, 3)},
            "Camera"
        },
        scn_cam{
            std::vector<Point3D>{bottom_left, bottom_right, top_right, top_left},
            std::vector<Edge>{std::make_pair(0, 1), std::make_pair(1, 2), std::make_pair(2, 3), std::make_pair(3, 0)},
            std::vector<Face>{Face( 0, 1, 2 ), Face(0, 2, 3)},
            "SCNCamera"
        },
        _angle{0}
    {
        auto empty = Point3D(0, 0, 0);
        if (top_left == empty && bottom_right == empty) {
            _points[3] = (_points[0] + Point3D(0, _points[2].y(), 0) );
            _points[1] = (_points[2] - Point3D(0, _points[2].y(), 0) );
        }
    }

    CameraWindow(geometry::Size size):
        Object3D{
            std::vector<Point3D>{Point3D(0, 0), Point3D(size.width, 0), Point3D(size.width, size.height), Point3D(0, size.height)},
            std::vector<Edge>{std::make_pair(0, 1), std::make_pair(1, 2), std::make_pair(2, 3), std::make_pair(3, 0)},
            std::vector<Face>{Face( 0, 1, 2 ), Face(0, 2, 3)},
            "Camera"
        },
        scn_cam{
            std::vector<Point3D>{Point3D(0, 0), Point3D(size.width, 0), Point3D(size.width, size.height), Point3D(0, size.height)},
            std::vector<Edge>{std::make_pair(0, 1), std::make_pair(1, 2), std::make_pair(2, 3), std::make_pair(3, 0)},
            std::vector<Face>{Face( 0, 1, 2 ), Face(0, 2, 3)},
            "SCNCamera"
        },
        _angle{0}
    {}

    Point3D bottom_left() const {
        return _points[0];
    }

    Point3D bottom_right() const {
        return _points[1];
    }

    Point3D top_right() const {
        return _points[2];
    }

    Point3D top_left() const {
        return _points[3];
    }

    Point3D& bottom_left() {
        return _points[0];
    }

    Point3D& bottom_right() {
        return _points[1];
    }

    Point3D& top_right() {
        return _points[2];
    }

    Point3D& top_left() {
        return _points[3];
    }

    double width() const {
        return (top_right() - top_left()).modulo();
    }

    double height() const {
        return (top_left() - bottom_left()).modulo();
    }

    double depth() const {
        return (top_right() - bottom_left()).z();
    }

    double angle() const {
        return _angle;
    }

    void move(double dx, double dy, double dz);
    void zoom(double step);
    void set_width(double width);
    void set_height(double height);
    void rotate(double da);
    void rotate_z(double angle);
    
    Point3D vrp();
    
    void resize(double width, double height) {
        set_width(width);
        set_height(height);
    }

    void resize(geometry::Size size) {
        resize(size.width, size.height);
    }

  private:
    double _angle;
    Object3D scn_cam;
};

}

#endif
