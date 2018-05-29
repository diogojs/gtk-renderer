#include "object3d.h"
#include "../render.h"

#include <iostream>

namespace rudolph {

unsigned int Object3D::objects_id = 0;

void Object3D::draw(RenderTarget& target) {
    if (!scn_valid) {
        for (auto i = 0u; i < scn_points.size(); ++i) {
            scn_points[i] = target.world_to_2d(_points[i]);
        }
        scn_valid = true;
    }
    for (auto i = 0u; i < _edges.size(); ++i) {
        target.draw_line( scn_points[_edges[i].first], scn_points[_edges[i].second]);
    }
}

Point3D Object3D::center() const {
    auto point = _points[0];
    for (auto i = 1u; i < _points.size(); ++i) {
        point += _points[i];
    }
    point.x() = point.x()/_points.size();
    point.y() = point.y()/_points.size();
    point.z() = point.z()/_points.size();
    return point;
}

void Object3D::translate(double dx, double dy, double dz) {
    for (auto i = 0u; i < _points.size(); ++i) {
        _points[i].translate(dx, dy, dz);
    }
    scn_valid = false;
}

void Object3D::scale(double sx, double sy, double sz) {
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

void Object3D::rotate_origin(double angle) {
	for (auto i = 0u; i < _points.size(); ++i) {
        _points[i].rotate_z(angle);
    }
    scn_valid = false;
}

void Object3D::rotate_pin(double angle, Point3D pin) {
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

void Object3D::rotate_center(double angle) {
    rotate_pin(angle, center());
}

void Object3D::rotate_x(double angle) {
    auto pin = center();
    for (auto i = 0u; i < _points.size(); ++i) {
        _points[i].translate(-pin.x(), -pin.y(), -pin.z());
    }
    for (auto i = 0u; i < _points.size(); ++i) {
        _points[i].rotate_x(angle);
    }
    for (auto i = 0u; i < _points.size(); ++i) {
        _points[i].translate(pin.x(), pin.y(), pin.z());
    }
    scn_valid = false;
}

void Object3D::rotate_y(double angle) {
    auto pin = center();
    for (auto i = 0u; i < _points.size(); ++i) {
        _points[i].translate(-pin.x(), -pin.y(), -pin.z());
    }
    for (auto i = 0u; i < _points.size(); ++i) {
        _points[i].rotate_y(angle);
    }
    for (auto i = 0u; i < _points.size(); ++i) {
        _points[i].translate(pin.x(), pin.y(), pin.z());
    }
    scn_valid = false;
}

void Object3D::rotate_z(double angle) {
    auto pin = center();
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

}
