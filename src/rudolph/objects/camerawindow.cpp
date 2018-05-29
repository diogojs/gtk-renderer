#include "camerawindow.h"
#include "../../algebra.h"

namespace rudolph {
using Point3D = geometry::Point3D;

void CameraWindow::move(double dx, double dy, double dz) {
    auto vx = (bottom_right() - bottom_left()).unit() * dx;
    auto vy = (top_left() - bottom_left()).unit() * dy;
    auto vz = ((bottom_right() - bottom_left()).cross(top_left() - bottom_left())).unit() * dz;

    auto d = vx + vy + vz;

    bottom_left() += d;
    top_right() += d;
    top_left() += d;
    bottom_right() += d;

    //center().to_string("center");
}

void CameraWindow::zoom(double step) {
    auto d = (top_right() - bottom_left()) * step;

    top_right() += d;
}

void CameraWindow::rotate(double da) {
    rotate_z(da);
}

void CameraWindow::rotate_z(double da) {
    Object3D::rotate_z(da);
    _angle += algebra::pi/180*da;
}

void CameraWindow::set_width(double width) {
    top_right().x() = bottom_left().x() + width;
}

void CameraWindow::set_height(double height) {
    top_right().y() = bottom_left().y() + height;
}

Point3D CameraWindow::vrp() {
    return center();
}

}
