#include "window.h"

#include "objects/shapes.h"
#include "dialog.h"
#include "utils/command.h"
#include "matrix.h"
#include "../algebra.h"

#include <iostream>
#include <utility>
#include <vector>

namespace rudolph {

using namespace rudolph::objects;
using Size = geometry::Size;

void on_close(GtkWidget* btn, gpointer* data) {
    reinterpret_cast<MainWindow*>(data)->close();
}

GtkWidget* create_gtk_window(GtkBuilder* gtk_builder, Size size) {
    auto err = static_cast<GError*>(nullptr);
    if (not gtk_builder_add_from_file(gtk_builder, "mainwindow.ui", &err)) {
        throw GtkInitError(
                std::string{"failed to initialize main window from UI file: "} +
                err->message + "\n");
    }

    auto pre_window = gtk_builder_get_object(gtk_builder, "main_window");
    auto gtk_window = GTK_WIDGET(pre_window);
    gtk_window_set_default_size(GTK_WINDOW(gtk_window), size.width, size.height);
    gtk_window_set_title(GTK_WINDOW(gtk_window), "Rudolph Software Rendeer");

    return gtk_window;
}

GtkWidget* get_component(GtkBuilder* gtk_builder, std::string id) {
    return GTK_WIDGET(gtk_builder_get_object(GTK_BUILDER(gtk_builder), id.c_str()));
}

bool update_tree_selection(GtkTreeView*& tree,
                           GtkTreeSelection*& selection,
                           GdkEventButton*& event) {
    GtkTreePath* path;
    GtkTreeViewColumn* column;

    if (not gtk_tree_view_get_path_at_pos(tree,
                                          event->x, event->y,
                                          &path,
                                          &column,
                                          nullptr,
                                          nullptr)) {
        return false;
    }

    if (column != gtk_tree_view_get_column(tree, 0)) {
        gtk_tree_path_free(path);
        return false;
    }

    gtk_tree_selection_unselect_all(selection);
    gtk_tree_selection_select_path(selection, path);
    gtk_tree_path_free(path);

    return true;
}

std::string extract_tree_value(GtkTreeModel* model,
                               GtkTreeIter& iter) {
    char* value;
    gtk_tree_model_get(model, &iter, 0, &value, -1);
    auto str = std::string{value};
    g_free(value);
    return str;
}

void show_popup(GtkTreeView*& tree,
                GtkMenu*& popup,
                GtkTreeSelection*& selection,
                gpointer* data)
{
    GtkTreeIter iter;
    GtkTreeModel* model;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        auto value = extract_tree_value(model, iter);
    }

    //gtk_menu_popup_at_pointer(popup, nullptr);
}

bool on_mouse_press(GtkWidget* widget, GdkEventButton* event, gpointer* data) {
    auto tree = GTK_TREE_VIEW(widget);
    auto popup = GTK_MENU(reinterpret_cast<GMenu*>(data));

    if (event->type == GDK_BUTTON_PRESS and event->button == 3) {
        auto selection = gtk_tree_view_get_selection(tree);

        if (not update_tree_selection(tree, selection, event)) {
            return false;
        }

        show_popup(tree, popup, selection, data);

        return true;
    }
    return false;
}

std::string gtkentry_value(GtkBuilder* builder, const std::string& id) {
    return gtk_entry_get_text(
            reinterpret_cast<GtkEntry*>(get_component(builder, id))
    );
}

MainWindow::MainWindow(Size size):
    _size{size},
    gtk_builder{gtk_builder_new()},
    gtk_window(create_gtk_window(gtk_builder, _size)),
    renderer{get_component(gtk_builder, "canvas")}
{
    g_signal_connect(gtk_window, "destroy", G_CALLBACK(on_close), this);
    g_signal_connect(get_component(gtk_builder, "treeview1"),
                     "button-press-event", G_CALLBACK(on_mouse_press),
                     GTK_MENU(get_component(gtk_builder, "displayfile_popup")));

    setup();
}

void MainWindow::execute(const std::string& cmd) {
    auto _cmd = command::parse(cmd);
    auto command = _cmd.command;
    auto args = _cmd.args;

    if (command == "print") {
        for (auto arg: args) {
            std::cout << arg << " ";
        }
        std::cout << '\n';
    } else if (command == "translate") {
        auto obj_id = args[0];

        for (auto& obj: renderer.display_file()) {
            if (obj.name() == args[0]) {
                obj.translate(atoi(args[1].c_str()), atoi(args[2].c_str()));
            }
        }
    }
}

void MainWindow::setup()
{
    static auto wrong_way_to_do_this = std::make_pair(this,  gtk_builder);
    auto button_events = std::vector<Event<void(GtkWidget*, void**)>>{
        {"btn_up", "clicked",
            [](GtkWidget* w, gpointer* data) {
                auto& r = *reinterpret_cast<Renderer*>(data);
                auto& rt = r.render_target();
                rt.move_camera(0, 1);
            }, &renderer},
        {"btn_down", "clicked",
            [](GtkWidget* w, gpointer* data) {
                auto& r = *reinterpret_cast<Renderer*>(data);
                auto& rt = r.render_target();
                rt.move_camera(0, -1);
            }, &renderer},
        {"btn_left", "clicked",
            [](GtkWidget* w, gpointer* data) {
                auto& r = *reinterpret_cast<Renderer*>(data);
                auto& rt = r.render_target();
                rt.move_camera(-1, 0);
            }, &renderer},
        {"btn_right", "clicked",
            [](GtkWidget* w, gpointer* data) {
                auto& r = *reinterpret_cast<Renderer*>(data);
                auto& rt = r.render_target();
                rt.move_camera(1, 0);
            }, &renderer},
        {"btn_in", "clicked",
            [](GtkWidget* w, gpointer* data) {
                auto& r = *reinterpret_cast<Renderer*>(data);
                auto& rt = r.render_target();
                rt.zoom(-0.1);
            }, &renderer},
        {"btn_out", "clicked",
            [](GtkWidget* w, gpointer* data) {
                auto& r = *reinterpret_cast<Renderer*>(data);
                auto& rt = r.render_target();
                rt.zoom(0.1);
            }, &renderer},
        {"btn_rot_left", "clicked",
            [](GtkWidget* w, gpointer* data) {
                auto& r = *reinterpret_cast<Renderer*>(data);
                auto& rt = r.render_target();
                rt.window().rotate(algebra::pi/18);
            }, &renderer},
        {"btn_rot_right", "clicked",
            [](GtkWidget* w, gpointer* data) {
                auto& r = *reinterpret_cast<Renderer*>(data);
                auto& rt = r.render_target();
                rt.window().rotate(-algebra::pi/18);
            }, &renderer},

        {"btn_new", "clicked",
            [](GtkWidget* w, gpointer* data) {
                std::cout << "btn new" << std::endl;
                auto dialog_new = DialogWindow{geometry::Size{300, 400}, "newobject.ui"};
                dialog_new.setup();
                dialog_new.show();
            }, &renderer},

        {"btn_edit", "clicked",
            [](GtkWidget* w, gpointer* data) {
                std::cout << "btn edit\n";
                Matrix<int> a( *(new std::vector<int>{3, 1, 2, 3, 9, 2}) );
                a.to_string();
                std::cout << std::endl;
            }, &renderer},
        {"btn_del", "clicked",
            [](GtkWidget* w, gpointer* data) {
                std::cout << "btn del\n";
            }, &renderer},
        {"btn_update_window", "clicked",
            [](GtkWidget* w, gpointer* data) {
                auto& window = *reinterpret_cast<MainWindow*>(data);
                window.refresh();
            }, this},
        {"btn_exec_cmdline", "clicked",
            [](GtkWidget* w, gpointer* _data) {
                auto& data = *reinterpret_cast<std::pair<MainWindow*,
                                                         GtkBuilder*>*>(_data);
                auto window = data.first;
                auto gtk_builder = data.second;

                window->execute(gtkentry_value(gtk_builder, "edt_cmdline"));
            }, &wrong_way_to_do_this},
    };

    for (auto event: button_events) {
        link_signal(event);
    }

    gtk_entry_set_text(GTK_ENTRY(get_component(gtk_builder, "edt_cmdline")), "translate pol1 -30 0");
}

void MainWindow::show() {
    gtk_widget_show_all(gtk_window);

    renderer.add_object(Point{-10, -10});
    //renderer.add_object(Line{0, 0, 40, 0});
    //renderer.add_object(Line{100, 0, 100, 40});
    auto points = std::vector<Point2D>{
        Point2D{150-200, 150},
        Point2D{175-200, 175},
        Point2D{160-200, 200},
        Point2D{140-200, 200},
        Point2D{125-200, 175},
    };
    renderer.add_object(Polygon(points));

    points = std::vector<Point2D>{
        Point2D{-230, 100},
        Point2D{-160, 100},
        Point2D{-155, 80},
        Point2D{-210, 60},
        Point2D{-165, 20},
        Point2D{-235, 25}
    };
    renderer.add_object(Polygon(points, true));

    points = std::vector<Point2D>{
        Point2D{0, 0},
        Point2D{20, 40},
        Point2D{80, -40},
        Point2D{100, 0}
    };
    renderer.add_object(BezierCurve(points));

    update_list();
}

void MainWindow::refresh() {
    auto window_size = Size{
        (double)std::stoi(gtkentry_value(gtk_builder, "edt_window_width")),
        (double)std::stoi(gtkentry_value(gtk_builder, "edt_window_height"))
    };
    renderer.render_target().window().resize(window_size);
}

void MainWindow::update_list() {
    auto list = GTK_LIST_STORE(get_component(gtk_builder, "list_store"));
    auto iter = GtkTreeIter{};
    for (auto obj: renderer.display_file()) {
        auto name = obj.name();
        gtk_list_store_append(list, &iter);
        gtk_list_store_set(list, &iter, 0, name.c_str(), -1);
    }
}

void MainWindow::close() {
    gtk_main_quit();
}

}
