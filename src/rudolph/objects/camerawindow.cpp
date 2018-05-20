#include "camerawindow.h"

namespace rudolph {
using Point3D = geometry::Point3D;

void CameraWindow::move(double dx, double dy) {
    auto d = Point3D{dx, dy};

    _bottom_left += d;
    _top_right += d;
}

void CameraWindow::zoom(double step) {
    auto d = (_top_right - _bottom_left) * step;

    _top_right += d;
}

void CameraWindow::rotate(double da) {
    _angle += da;
}

void CameraWindow::rotate_x(double da) {
    _top_right.rotate_x(da);
    _bottom_left.rotate_x(da);
}

void CameraWindow::rotate_y(double da) {
    _top_right.rotate_y(da);
    _bottom_left.rotate_y(da);
}

void CameraWindow::rotate_z(double da) {
    _top_right.rotate_z(da);
    _bottom_left.rotate_z(da);
}

void CameraWindow::set_width(double width) {
    _top_right.x() = _bottom_left.x() + width;
}

void CameraWindow::set_height(double height) {
    _top_right.y() = _bottom_left.y() + height;
}

Point3D CameraWindow::vrp() {
    return (_top_right + _bottom_left)*0.5;
}

}
