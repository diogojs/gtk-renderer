#include "camerawindow.h"

namespace rudolph {
using Point3D = geometry::Point3D;

void CameraWindow::move(double dx, double dy, double dz) {
    auto d = Point3D{dx, dy, dz};

    bottom_left() += d;
    top_right() += d;
    top_left() += d;
    bottom_right() += d;
}

void CameraWindow::zoom(double step) {
    auto d = (top_right() - bottom_left()) * step;

    top_right() += d;
}

void CameraWindow::rotate(double da) {
    _angle += da;
}

void CameraWindow::set_width(double width) {
    top_right().x() = bottom_left().x() + width;
}

void CameraWindow::set_height(double height) {
    top_right().y() = bottom_left().y() + height;
}

Point3D CameraWindow::vrp() {
    return (top_right() + bottom_left())*0.5;
}

}
