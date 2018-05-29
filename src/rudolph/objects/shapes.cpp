#include "shapes.h"

#include <iostream>

namespace rudolph {

unsigned int Point::points_id = 0;
unsigned int Line::lines_id = 0;
unsigned int Polygon::polygons_id = 0;

void Point::draw(RenderTarget& target) {
    if (!scn_valid) {
        scn_point = target.world_to_normal(point);
        scn_valid = true;
    }
    target.draw_point(scn_point);
}

void Point::translate(double dx, double dy, double dz) {
	point.translate(dx, dy, dz);
    scn_valid = false;
}

void Point::scale(double sx, double sy, double sz) {
	point.scale(sx, sy, sz);
    scn_valid = false;
}

void Point::rotate_origin(double angle) {
    point.rotate_z(angle);
    scn_valid = false;
}

void Point::rotate_pin(double angle, Point3D pin) {
    point.translate(-pin.x(), -pin.y(), -pin.z());
    point.rotate_z(angle);
    point.translate(pin.x(), pin.y(), pin.z());
    scn_valid = false;
}

void Point::rotate_center(double angle) {
	// do nothing
}

void Line::draw(RenderTarget& target) {
    if (!scn_valid) {
        scn_a = target.world_to_normal(_a);
        scn_b = target.world_to_normal(_b);
        scn_valid = true;
    }
    target.draw_line(scn_a, scn_b);
}

Point3D Line::center() const {
    Point3D point{(_a.x() + _b.x())/2, (_a.y() + _b.y())/2, (_a.z() + _b.z())/2};
    return point;
}

void Line::translate(double dx, double dy, double dz) {
    _a.translate(dx, dy, dz);
    _b.translate(dx, dy, dz);
    scn_valid = false;
}

void Line::scale(double sx, double sy, double sz) {
    auto center = this->center();
    _a.translate(-center.x(), -center.y(), -center.z());
    _b.translate(-center.x(), -center.y(), -center.z());
    _a.scale(sx, sy, sz);
    _b.scale(sx, sy, sz);
    _a.translate(center.x(), center.y(), center.z());
    _b.translate(center.x(), center.y(), center.z());
    scn_valid = false;
}

void Line::rotate_origin(double angle) {
    _a.rotate_z(angle);
    _b.rotate_z(angle);
    scn_valid = false;
}

void Line::rotate_pin(double angle, Point3D pin) {
    _a.translate(-pin.x(), -pin.y(), -pin.z());
    _b.translate(-pin.x(), -pin.y(), -pin.z());
    _a.rotate_z(angle);
    _b.rotate_z(angle);
    _a.translate(pin.x(), pin.y(), pin.z());
    _b.translate(pin.x(), pin.y(), pin.z());
    scn_valid = false;
}

void Line::rotate_center(double angle) {
    rotate_pin(angle, center());
}

void Polygon::draw(RenderTarget& target) {
    if (!scn_valid) {
        for (auto i = 0u; i < scn_points.size(); ++i) {
            scn_points[i] = target.world_to_normal(_points[i]);
        }
        scn_valid = true;
    }
    target.draw_polygon(scn_points, _filled);
}

Point3D Polygon::center() const {
    auto point = _points[0];
    for (auto i = 1u; i < _points.size(); ++i) {
        point += _points[i];
    }
    point.x() = point.x()/_points.size();
    point.y() = point.y()/_points.size();
    point.z() = point.z()/_points.size();
    return point;
}

void Polygon::translate(double dx, double dy, double dz) {
    for (auto i = 0u; i < _points.size(); ++i) {
        _points[i].translate(dx, dy, dz);
    }
    scn_valid = false;
}

void Polygon::scale(double sx, double sy, double sz) {
    auto center = this->center();
    for (auto i = 0u; i < _points.size(); ++i) {
        _points[i].translate(-center.x(), -center.y(), -center.z());
    }
    for (auto i = 0u; i < _points.size(); ++i) {
        _points[i].scale(sx, sy, sz);
    }
    for (auto i = 0u; i < _points.size(); ++i) {
        _points[i].translate(center.x(), center.y(), center.z());
    }
    scn_valid = false;
}

void Polygon::rotate_origin(double angle) {
	for (auto i = 0u; i < _points.size(); ++i) {
        _points[i].rotate_z(angle);
    }
    scn_valid = false;
}

void Polygon::rotate_pin(double angle, Point3D pin) {
	for (auto i = 0u; i < _points.size(); ++i) {
        _points[i].translate(-pin.x(), -pin.y(), -pin.z());
    }
    for (auto i = 0u; i < _points.size(); ++i) {
        _points[i].rotate_z(angle);
    }
    for (auto i = 0u; i < _points.size(); ++i) {
        _points[i].translate(pin.x(), pin.y(), pin.z());
    }
    scn_valid = false;
}

void Polygon::rotate_center(double angle) {
    rotate_pin(angle, center());
}

}
