#ifndef RUDOLPH_RENDER_H
#define RUDOLPH_RENDER_H

#include <memory>
#include <vector>

#include <gtk/gtk.h>

#include "geometry.h"
#include "drawable.h"
#include "matrix.h"
#include "objects/camerawindow.h"
#include "objects/viewport.h"
#include "objects/object3d.h"
#include "../utils.h"

namespace rudolph {

/**
 * Drawable canvas.
 *
 * Abstracts the idea of an output drawing target
 * and applies window/viewport transformations.
 */
class RenderTarget {
    using Point3D = geometry::Point3D;
    using Size = geometry::Size;
public:
    RenderTarget();
    ~RenderTarget();

    Point3D world_to_normal(double xw, double yw);
    Point3D world_to_normal(Point3D p);
    Point3D normal_to_viewport(double xw, double yw);
    Point3D normal_to_viewport(Point3D p);
    Point3D world_to_viewport(double xw, double yw);
    Point3D world_to_viewport(Point3D p);
    Point3D world_to_2d(Point3D p);
    Matrix<double> calc_transform();

    void clear();
    void draw_point(Point3D);
    void draw_line(Point3D, Point3D);
    void draw_polygon(std::vector<Point3D> points, bool filled);
    void draw_curve(std::vector<Point3D> points);
    void draw_viewport();
    void move_camera(double dx, double dy, double dz);
    void rotate_camera(double ax, double ay, double az);

    CameraWindow& camera() {
        return camera_window;
    }

    const CameraWindow& camera() const {
        return camera_window;
    }

    Viewport& get_viewport() {
        return viewport;
    }

    const Viewport& get_viewport() const {
        return viewport;
    }

    cairo_surface_t* surface() const {
        return back_buffer_;
    }

    void zoom(double ratio);

    double step() const {
        return _step;
    }

private:
    CameraWindow camera_window;
    Viewport viewport;
    Matrix<double> transform;
    cairo_surface_t* back_buffer_ = nullptr;
    double _step = 1;
};

/**
 * Manages drawable components.
 */
class Renderer {
    using Size = geometry::Size;
    using Rect = geometry::Rect;
public:
    /**
     * Creates a renderer for a given window.
     *
     * @param parent Parent GtkWindow.
     */
    Renderer(GtkWidget*);

    void refresh();
    void clear();
    void invalidate();
    void invalidate(Rect);
    void load_obj(std::string filename);

    template <typename T>
    void add_object(T x) {
        _display_file.push_back(Drawable(std::move(x)));
    }

    void del_object() {
        if (!_display_file.empty())
            _display_file.pop_back();
    }

    std::vector<Drawable> display_file() const {
        return _display_file;
    }

    std::vector<Drawable>& display_file() {
        return _display_file;
    }    

    cairo_surface_t* surface() const {
        return target.surface();
    }

    RenderTarget& render_target() {
        return target;
    }

private:
    GtkWidget* parent;
    RenderTarget target;
    std::vector<Drawable> _display_file;
};

}

#endif
