#ifndef RUDOLPH_CAMERA_H
#define RUDOLPH_CAMERA_H

#include "../geometry.h"

namespace rudolph {

class CameraWindow {
    using Point3D = geometry::Point3D;
public:
    CameraWindow():
        _top_right{Point3D{200, 100}},
        _bottom_left{Point3D{0, 0}},
        _angle{0}
    {}

    CameraWindow(Point3D bottom_left, Point3D top_right, Point3D top_left = {}, Point3D bottom_right = {}):
        _top_right{top_right},
        _bottom_left{bottom_left},
        _top_left{top_left},
        _bottom_right{bottom_right},
        _angle{0}
    {
        auto empty = Point3D(0, 0, 0);
        if (top_left == empty && bottom_right == empty) {
            _top_left = (_bottom_left + Point3D(0, _top_right.y(), 0) );
            _bottom_right = (_top_right - Point3D(0, _top_right.y(), 0) );
        }
    }

    CameraWindow(geometry::Size size):
        _top_right{size.width, size.height},
        _bottom_left{0, 0},
        _angle{0}
    {}

    Point3D top_right() const {
        return _top_right;
    }

    Point3D bottom_left() const {
        return _bottom_left;
    }

    Point3D top_left() const {
        return _top_left;
    }

    Point3D bottom_right() const {
        return _bottom_right;
    }

    double width() const {
        return (_top_right - _bottom_left).x();
    }

    double height() const {
        return (_top_right - _bottom_left).y();
    }

    double depth() const {
        return (_top_right - _bottom_left).z();
    }

    double angle() const {
        return _angle;
    }

    void move(double dx, double dy, double dz);
    void zoom(double step);
    void set_width(double width);
    void set_height(double height);
    void rotate(double da);
    void rotate_x(double da);
    void rotate_y(double da);
    void rotate_z(double da);

    Point3D vrp();
    
    void resize(double width, double height) {
        set_width(width);
        set_height(height);
    }

    void resize(geometry::Size size) {
        resize(size.width, size.height);
    }


  private:
    Point3D _top_right;
    Point3D _bottom_left;
    Point3D _top_left;
    Point3D _bottom_right;

    double _angle;
};

}

#endif
