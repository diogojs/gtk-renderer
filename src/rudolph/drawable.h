#ifndef RUDOLPH_DRAWABLE_H
#define RUDOLPH_DRAWABLE_H

#include "../utils.h"
#include "geometry.h"

#include <string>

namespace rudolph {

class RenderTarget;

class Drawable {
    using Point3D = geometry::Point3D;
public:
    template <typename T>
    Drawable(T t):
        data{utils::make_unique<ModelImpl<T>>(std::move(t))}
    {}

    Drawable(const Drawable& other):
        data{other.data->copy()}
    {}

    Drawable(Drawable&& other):
        data{std::move(other.data)}
    {}

    void draw(RenderTarget& target) {
        data->draw(target);
    }

    std::string name() const {
        return data->name();
    }

    Point3D center() const {
        data->center();
    }

    void translate(double dx, double dy, double dz) {
        data->translate(dx, dy, dz);
    }

    void scale(double sx, double sy, double sz) {
        data->scale(sx, sy, sz);
    }

    void rotate_origin(double angle) {
        data->rotate_origin(angle);
    }

    void rotate_pin(double angle, Point3D pin) {
        data->rotate_pin(angle, pin);
    }

    void rotate_center(double angle) {
        data->rotate_center(angle);
    }

    void rotate_x(double angle) {
        data->rotate_x(angle);
    }

    void rotate_y(double angle) {
        data->rotate_y(angle);
    }

    void rotate_z(double angle) {
        data->rotate_z(angle);
    }

private:
    /**
     * DrawableImpl components' interface
     */
    struct Model {
        virtual ~Model() = default;
        virtual std::unique_ptr<Model> copy() const = 0;
        virtual void draw(RenderTarget&) = 0;
        virtual std::string name() const = 0;
        virtual Point3D center() const = 0;
        virtual void translate(double dx, double dy, double dz) = 0;
        virtual void scale(double sx, double sy, double sz) = 0;
        virtual void rotate_origin(double angle) = 0;
        virtual void rotate_pin(double angle, Point3D pin) = 0;
        virtual void rotate_center(double angle) = 0;
        virtual void rotate_x(double angle) = 0;
        virtual void rotate_y(double angle) = 0;
        virtual void rotate_z(double angle) = 0;
    };

    template <typename T>
    struct ModelImpl: Model {
        ModelImpl(T x):
            x{x}
        {}

        std::unique_ptr<Model> copy() const override {
            return utils::make_unique<ModelImpl>(*this);
        }

        void draw(RenderTarget& target) override {
            x.draw(target);
        }

        std::string name() const override {
            return x.name();
        }

        Point3D center() const override {
            return x.center();
        }

        void translate(double dx, double dy, double dz) override {
            x.translate(dx, dy, dz);
        }

        void scale(double sx, double sy, double sz) override {
            x.scale(sx, sy, sz);
        }

        void rotate_origin(double angle) {
            x.rotate_origin(angle);
        }

        void rotate_pin(double angle, Point3D pin) {
            x.rotate_pin(angle, pin);
        }

        void rotate_center(double angle) {
            x.rotate_center(angle);
        }

        void rotate_x(double angle) {
            x.rotate_x(angle);
        }

        void rotate_y(double angle) {
            x.rotate_y(angle);
        }

        void rotate_z(double angle) {
            x.rotate_z(angle);
        }

        T x;
    };

    std::unique_ptr<Model> data;
};

}

#endif
