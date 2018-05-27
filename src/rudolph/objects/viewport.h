#ifndef RUDOLPH_VIEWPORT_H
#define RUDOLPH_VIEWPORT_H

#include <iostream>

#include "../geometry.h"

namespace rudolph {

class Viewport {
    using Point3D = geometry::Point3D;
public:
    Viewport(Point3D top_left, Point3D bottom_right):
        _top_left{top_left},
        _bottom_right{bottom_right}
    {}

    Viewport(geometry::Size size):
        _top_left{10, 10},
        _bottom_right{_top_left.x()+size.width, _top_left.y()+size.height}
    {}

    Point3D top_left() const {
        return _top_left;
    }

    Point3D bottom_right() const {
        return _bottom_right;
    }

    int width() const {
        return (_bottom_right.x() - _top_left.x());
    }

    int height() const {
        return (_bottom_right.y() - _top_left.y());
    }

    void set_width(int width) {
        _bottom_right.x() = _top_left.x() + width;
    }

    void set_height(int height) {
        _bottom_right.y() = _top_left.y() + height;
    }

    void resize(int width, int height) {
        set_width(width);
        set_height(height);
    }

  private:
    Point3D _top_left;
    Point3D _bottom_right;
};

}

#endif
