#ifndef RUDOLPH_VECTOR3_H
#define RUDOLPH_VECTOR3_H

#include <cmath>
#include "matrix.h"
#include "geometry.h"

namespace rudolph {

class Vector3 {
  public:
    Vector3(double x, double y, double z):
        _matrix{x, y, z}
    {}

    Vector3(geometry::Point3D p):
        _matrix{p.x(), p.y(), p.z()}
    {}

    double& x() {
        return _matrix(0, 0);
    }

    double& y() {
        return _matrix(0, 1);
    }

    double& z() {
        return _matrix(0, 2);
    }

    const double& x() const {
        return _matrix(0, 0);
    }

    const double& y() const {
        return _matrix(0, 1);
    }

    const double& z() const {
        return _matrix(0, 2);
    }

    Vector3 cross(const Vector3& v) {
        Vector3 result {
            (y()*v.z() - z()*v.y()), // i
            (z()*v.x() - x()*v.z()), // j
            (x()*v.y() - y()*v.x()) // k
        };
        return result;
    }

    double modulo() {
        return sqrt(x()*x() + y()*y() + z()*z());
    }

    Vector3 unit() {
        auto n = modulo();
        Vector3 unitary {
            x()/n,
            y()/n,
            z()/n
        };
        return unitary;
    }

    double angle_x() {
        return atan(y()/z());
    }

    double angle_y() {
        return atan(x()/z());
    }

    void to_string() {
        _matrix.to_string();
    }

  private:
    Matrix<double> _matrix;
};

}

#endif
