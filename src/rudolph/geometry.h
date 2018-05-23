#ifndef RUDOLPH_GEOMETRY_H
#define RUDOLPH_GEOMETRY_H

#include <memory>
#include <cmath>

#include "matrix.h"

namespace rudolph {
namespace geometry {

struct Size {
    double width{0};
    double height{0};

    Size(double w, double h):
        width{w},
        height{h}
    {}
};

struct Point3D {
    Matrix<double> data;
    
    Point3D():
        data{0, 0, 0, 1}
    {}

    Point3D(double x, double y, double z = 0):
        data{x, y, z, 1}
    {}

    double& x() {
        return data(0, 0);
    }

    double& y() {
        return data(0, 1);
    }

    double& z() {
        return data(0, 2);
    }

    const double& x() const {
        return data(0, 0);
    }

    const double& y() const {
        return data(0, 1);
    }

    const double& z() const {
        return data(0, 2);
    }

    Point3D cross(const Point3D& v) {
        Point3D result {
            (y()*v.z() - z()*v.y()), // i
            (z()*v.x() - x()*v.z()), // j
            (x()*v.y() - y()*v.x()) // k
        };
        return result;
    }

    double modulo() {
        return sqrt(x()*x() + y()*y() + z()*z());
    }

    Point3D unit() {
        auto n = modulo();
        Point3D unitary {
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

    void to_string(std::string title = " ") {
        std::cout << title << ": " << x() << " " << y() << " " << z() << std::endl;
    }

    Point3D& operator+=(const Point3D& p);
    Point3D& operator-=(const Point3D& p);
    bool operator==(const Point3D& p);

    void translate(double dx, double dy, double dz = 0);
    void scale(double sx, double sy, double sz = 1);
    //void rotate(Eixo axis, double angle);
    void rotate_x(double angle);
    void rotate_y(double angle);
    void rotate_z(double angle);
};

Point3D operator-(const Point3D&);
Point3D operator+(const Point3D&, const Point3D&);
Point3D operator-(const Point3D&, const Point3D&);
Point3D operator*(const Point3D&, int);
Point3D operator*(const Point3D&, double);
Point3D operator*(int value, const Point3D& p);
Point3D operator*(double value, const Point3D& p);

typedef std::pair<unsigned, unsigned> Edge;
//typedef int[] Eixo;

struct Face {
    Face(unsigned a, unsigned b, unsigned c):
        edges{ a, b, c }
    {}

    std::vector<unsigned> edges;
};

struct Rect {
    double x{0};
    double y{0};
    double width{0};
    double height{0};

    Rect(double x, double y, double w, double h):
        x{x},
        y{y},
        width{w},
        height{h}
    {}
};

}
}


#endif
