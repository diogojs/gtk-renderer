#include "render.h"

#include "objects/shapes.h"
#include "clipper.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "utils/tiny_obj_loader.h"

#include <utility>
#include <iostream>
#include <cmath>

namespace {
    using namespace rudolph;

    using Rect = geometry::Rect;
    using Size = geometry::Size;
    using Point3D = geometry::Point3D;


    void clear(cairo_surface_t* surface) {
        auto cr = cairo_create(surface);
        cairo_set_source_rgb(cr, 1, 1, 1);
        cairo_paint(cr);
        cairo_destroy(cr);
    }


    cairo_surface_t* surface_from_widget(GtkWidget* widget) {
        auto surface = gdk_window_create_similar_surface(
                           gtk_widget_get_window(widget),
                           CAIRO_CONTENT_COLOR,
                           gtk_widget_get_allocated_width(widget),
                           gtk_widget_get_allocated_height(widget)
                       );
        clear(surface);
        return surface;
    }


    gboolean on_draw(GtkWidget* widget, cairo_t* cr, gpointer* data)
    {
        auto& renderer = *reinterpret_cast<Renderer*>(data);
        auto surface = renderer.surface();

        cairo_set_source_surface(cr, surface, 0, 0);
        clear(surface);

        renderer.refresh();
        renderer.invalidate();

        cairo_paint(cr);

        return false;
    }


    gboolean on_config_event(GtkWidget* widget,
                          GdkEventConfigure* event,
                          gpointer* data)
    {
        return true;
    }


    gboolean on_resize(
            GtkWidget* widget,
            GtkAllocation* event,
            gpointer* data)
    {
        return true;
    }


    gboolean on_key_press(
            GtkWidget* widget,
            GdkEventKey* event,
            gpointer* data)
    {
        auto renderer = reinterpret_cast<Renderer*>(data);
        auto& target = renderer->render_target();
        switch (event->keyval) {
            case GDK_KEY_Up:
                target.move_camera(0, -1, 0);
                break;
            case GDK_KEY_Down:
                target.move_camera(0, 1, 0);
                break;
            case GDK_KEY_Left:
                target.move_camera(1, 0, 0);
                break;
            case GDK_KEY_Right:
                target.move_camera(-1, 0, 0);
                break;
            case GDK_KEY_Page_Up:
                target.zoom(0.1);
                break;
            case GDK_KEY_Page_Down:
                target.zoom(-0.1);
                break;
        }
        return true;
    }


    gboolean on_scroll(
            GtkWidget* widget,
            GdkEventScroll* event,
            gpointer* data)
    {
        double delta = (event->direction * 2 - 1);
        delta /= 10;
        auto target = reinterpret_cast<Renderer*>(data)->render_target();
        target.zoom(delta);
        return true;
    }

    Size parent_size(GtkWidget* parent) {
        GtkRequisition parent_size;
        gtk_widget_get_preferred_size(parent, nullptr, &parent_size);
        return Size{(double)parent_size.width, (double)parent_size.height};
    }
}

int RenderTarget::counter = 0;
bool RenderTarget::log = false;

Renderer::Renderer(GtkWidget* parent):
    parent{parent},
    target{}
{
    g_signal_connect(parent, "draw", G_CALLBACK(on_draw), this);
    g_signal_connect(parent, "configure-event", G_CALLBACK(on_config_event), &target);
    g_signal_connect(parent, "size-allocate", G_CALLBACK(on_resize), this);
    g_signal_connect(parent, "key-press-event", G_CALLBACK(on_key_press), this);

    gtk_widget_add_events(GTK_WIDGET(parent), GDK_SCROLL_MASK);
    g_signal_connect(parent, "scroll-event", G_CALLBACK(on_scroll), this);
}


void Renderer::refresh()
{
    target.draw_viewport();

    for (auto obj: _display_file)
    {
        obj.draw(target);
    }
}


void Renderer::clear()
{
    target.clear();
}

RenderTarget::RenderTarget():
    camera_window{Point3D{0, 0, 0}, Point3D{26, 26, 0}},
    viewport{Size{520, 520}},
    transform{calc_transform()},
    back_buffer_{
        cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                   viewport.width()+20,
                                   viewport.height()+20) }
{}

RenderTarget::~RenderTarget() {
    cairo_surface_destroy(surface());
}

Point3D RenderTarget::world_to_normal(double xw, double yw) {
    Matrix<double> coord{xw, yw, 1};
    if (RenderTarget::log)
        std::cout << "xw: " << xw << " yw: " << yw << std::endl;
    
    double cos_vy = std::cos(camera_window.angle());
    double sin_vy = std::sin(camera_window.angle());
    double cam_x = camera_window.bottom_left().x();
    double cam_y = camera_window.bottom_left().y();
    double cam_width = camera_window.width();
    double cam_height = camera_window.height();

    if (RenderTarget::log) {
        camera_window.bottom_left().to_string("cam_xy");
        std::cout << "cam_width,height: " << cam_width << " , " << cam_height << std::endl;
    }

    // Normalized Coordinates
    // Translate to origin, rotate, and scale
    Matrix<double> normalizer({
        cos_vy * 2/cam_width, -sin_vy * 2/cam_height, 0,
        sin_vy * 2/cam_width, cos_vy * 2/cam_height, 0,
        (-cos_vy*cam_x - sin_vy*cam_y)*2/cam_width, (sin_vy*cam_x - cos_vy*cam_y)*2/cam_height, 1
        },
        3, 3
    );
    if (RenderTarget::log)
        normalizer.to_string("normalizer");
    
    coord = coord * normalizer;

    auto result = Point3D{coord(0, 0), coord(0, 1)};
    if (RenderTarget::log)
        result.to_string("result in");

    return result;
}

Point3D RenderTarget::world_to_normal(Point3D p) {
    return world_to_normal(p.x(), p.y());
}

Point3D RenderTarget::normal_to_viewport(double xw, double yw) {
    Matrix<double> coord{xw, yw, 1};

    // Viewport Coordinates
    Matrix<double> viewporter(
        {
        (double)viewport.width()/2, 0, 0,
        0, (double)-viewport.height()/2, 0,
        viewport.top_left().x()+(double)viewport.width()/2, viewport.top_left().y()+(double)viewport.height()/2, 1
        },
        3, 3
    );
    
    coord = coord * viewporter;

    return Point3D{coord(0, 0), coord(0, 1)};
}

Point3D RenderTarget::normal_to_viewport(Point3D p) {
    return normal_to_viewport(p.x(), p.y());
}

Point3D RenderTarget::world_to_viewport(double xw, double yw) {
    auto normalized = world_to_normal(xw, yw);
    return normal_to_viewport(normalized.x(), normalized.y());
}

Point3D RenderTarget::world_to_viewport(Point3D p) {
    return world_to_viewport(p.x(), p.y());
}

Point3D RenderTarget::world_to_2d(Point3D p) {
    RenderTarget::counter++;
    //if (RenderTarget::counter % 201 == 0 || RenderTarget::counter % 202 == 0)
    //    RenderTarget::log = true;

    Matrix<double> coord{p.x(), p.y(), p.z(), 1};
    if (RenderTarget::log)
        p.to_string("p");
    coord = coord * transform;

    auto transformed_p = Point3D{coord(0, 0), coord(0, 1), coord(0, 2)};
    if (RenderTarget::log)
        transformed_p.to_string("transformed_p");

    auto result = world_to_normal(transformed_p);
    if (RenderTarget::log)
        result.to_string("result final");

    RenderTarget::log = false;

    return result;
}

Matrix<double> RenderTarget::calc_transform() {
    auto vrp = camera_window.vrp();
    //std::cout << "vrp: " << vrp.x() << " " << vrp.y() << " " << vrp.z() << std::endl;

    Matrix<double> trans(
        {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        -vrp.x(), -vrp.y(), -vrp.z(), 1
        },
        4, 4
    );
    //trans.to_string("trans:");

    auto u = camera_window.bottom_right() - camera_window.bottom_left();
    //u.to_string("u");
    auto v = camera_window.top_left() - camera_window.bottom_left();
    //v.to_string("v");
    auto normal = u.cross(v).unit();
    //normal.to_string("normal");

    // Rotate in x
    auto tx = normal.angle_x();
    //std::cout << "angle_x: " << tx << std::endl;
    Matrix<double> rotx(
        {
        1, 0, 0, 0,
        0, cos(tx), sin(tx), 0,
        0, -sin(tx), cos(tx), 0,
        0, 0, 0, 1
        },
        4, 4
    );
    //rotx.to_string("rotx:");

    auto ty = normal.angle_y();
    //std::cout << "angle_y: " << ty << std::endl;
    Matrix<double> roty(
        {
        cos(ty), 0, -sin(ty), 0,
        0, 1, 0, 0,
        sin(ty), 0, cos(ty), 0,
        0, 0, 0, 1
        },
        4, 4
    );
    //roty.to_string("roty:");

    auto _transform = trans * rotx * roty;
    //_transform.to_string("_transform:");
    return _transform;
}

void RenderTarget::clear() {
    ::clear(back_buffer_);
}

void RenderTarget::draw_point(Point3D p) {
    auto clipped = Clipper().clip_point(p);

    if (clipped) // if point was clipped it is out of the window
        return;  // no need to draw it

    auto vpoint = normal_to_viewport(p);
    auto x = vpoint.x();
    auto y = vpoint.y();

    auto region = Rect{x, y, 1, 1};

    auto cr = cairo_create(back_buffer_);

    cairo_set_source_rgb(cr, 0, 0, 1);
    cairo_set_line_width(cr, 1);

    cairo_rectangle(cr, region.x, region.y, region.width, region.height);
    cairo_fill(cr);

    cairo_destroy(cr);
}

void RenderTarget::draw_line(Point3D a, Point3D b) {

    auto clipper = Clipper(ClipMethod::LIANG_BARSKY);

    std::vector<Point3D> clipped = clipper.clip_line(a, b);

    if (clipped.size() > 0) {
        auto va = normal_to_viewport(clipped[0]);
        auto vb = normal_to_viewport(clipped[1]);

        auto cr = cairo_create(back_buffer_);

        cairo_set_source_rgb(cr, 0, 0, 1);
        cairo_set_line_width(cr, 1);

        cairo_move_to(cr, va.x(), va.y());
        cairo_line_to(cr, vb.x(), vb.y());

        cairo_stroke(cr);
        cairo_destroy(cr);
    }
}

void RenderTarget::draw_polygon(std::vector<Point3D> points, bool filled) {
    auto cr = cairo_create(back_buffer_);
    cairo_set_source_rgb(cr, 0, 0, 1);
    cairo_set_line_width(cr, 1);

    auto clipper = Clipper();
    std::vector<Point3D> clipped = clipper.clip_polygon(points);

    if (clipped.size() > 0) {
        // Move to first point
        auto va = normal_to_viewport(clipped[0]);
        cairo_move_to(cr, va.x(), va.y());
        // Iterate through every point
        for (auto i = 1u; i < clipped.size(); ++i) {
            auto vb = normal_to_viewport(clipped[i]);

            cairo_line_to(cr, vb.x(), vb.y());
        }
        // Go back to first point to close polygon
        cairo_line_to(cr, va.x(), va.y());

        if (filled) {
            cairo_fill(cr);
        } else {
            cairo_stroke(cr);
        }

    }
    cairo_destroy(cr);
}

void RenderTarget::draw_curve(std::vector<Point3D> points) {
    auto cr = cairo_create(back_buffer_);
    cairo_set_source_rgb(cr, 0, 0, 1);
    cairo_set_line_width(cr, 1);

    auto clipper = Clipper();
    std::vector<Point3D> clipped = clipper.clip_curve(points);

    if (clipped.size() > 0) {
        // Move to first point
        auto va = normal_to_viewport(clipped[0]);
        cairo_move_to(cr, va.x(), va.y());
        // Iterate through every point
        for (auto i = 1u; i < clipped.size(); ++i) {
            auto vb = normal_to_viewport(clipped[i]);

            cairo_line_to(cr, vb.x(), vb.y());
        }

        cairo_stroke(cr);

    }
    cairo_destroy(cr);
}

void RenderTarget::draw_viewport() {
    auto cr = cairo_create(back_buffer_);
    cairo_set_source_rgb(cr, 1, 0, 0);
    cairo_set_line_width(cr, 1);

    // Move to first point
    auto va = viewport.top_left();
    cairo_move_to(cr, va.x(), va.y());

    auto vb = va;
    vb.x() += viewport.width();
    cairo_line_to(cr, vb.x(), vb.y());

    vb.y() += viewport.height();
    cairo_line_to(cr, vb.x(), vb.y());

    vb.x() -= viewport.width();
    cairo_line_to(cr, vb.x(), vb.y());

    cairo_line_to(cr, va.x(), va.y());

    cairo_stroke(cr);
    cairo_destroy(cr);
}

void RenderTarget::move_camera(double dx, double dy, double dz) {
    camera_window.move(dx * _step, dy * _step, dz * _step);
    transform = calc_transform();
}

void RenderTarget::rotate_camera(double ax, double ay, double az) {
    camera_window.rotate_x(ax);
    camera_window.rotate_y(ay);
    camera_window.rotate_z(az);
    transform = calc_transform();
}

void RenderTarget::zoom(double ratio) {
    camera_window.zoom(ratio);
    calc_transform();
}

void Renderer::invalidate() {
    invalidate(Rect{0, 0,
                    (double)gtk_widget_get_allocated_width(parent),
                    (double)gtk_widget_get_allocated_height(parent)});
}

void Renderer::invalidate(Rect region) {
    gtk_widget_queue_draw_area(
            parent,
            region.x - 1,
            region.y - 1,
            region.width + 2,
            region.height + 2
    );
}

void Renderer::load_obj(std::string filename) {
    using Edge = geometry::Edge;
    using Face = geometry::Face;

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes_tiny;
    std::vector<tinyobj::material_t> materials;

    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes_tiny, &materials, &err, filename.c_str());

    if (!err.empty()) {
        std::cerr << err << std::endl;
    }

    if (!ret) {
        return;
    }

    std::vector<Point3D> points;
    std::vector<Edge> edges;
    std::vector<Face> faces;

    // loop over vertices
    for (auto v = 0u; v < attrib.vertices.size(); v+=3) {
        auto vx = attrib.vertices[v];
        auto vy = attrib.vertices[v+1];
        auto vz = attrib.vertices[v+2];

        points.push_back(Point3D(vx, vy, vz));
    }

    // loop over shapes
    for (auto shape: shapes_tiny) {
        // loop over faces
        size_t idx_offset = 0;
        for (size_t fid = 0u; fid < shape.mesh.num_face_vertices.size(); fid++) {
            int fv = shape.mesh.num_face_vertices[fid];

            // link all vertices in edges
            for (size_t v = 0; v < fv-1; v++) {
                int idx1 = shape.mesh.indices[idx_offset+v].vertex_index;
                int idx2 = shape.mesh.indices[idx_offset+v+1].vertex_index;
                edges.push_back( std::make_pair(idx1, idx2) );
            }
            int idx1 = shape.mesh.indices[idx_offset+fv-1].vertex_index;
            int idx2 = shape.mesh.indices[idx_offset].vertex_index;
            edges.push_back( std::make_pair(idx1, idx2) );

            idx_offset += fv;
        }
        add_object(objects::Object3D(points, edges, faces, shape.name));
    }
}
