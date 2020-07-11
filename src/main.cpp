#include "header.hpp"

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

sf::RenderWindow window;

int main(int argc, char ** argv) {

    window.create(sf::VideoMode(612, 352), "Bongo Cat for osu!", sf::Style::Titlebar | sf::Style::Close);

    Display *dpy = XOpenDisplay(NULL);
    Window root = RootWindow(dpy, 0);

    XRRScreenConfiguration *conf = XRRGetScreenInfo(dpy, root);
    short refresh_rate = XRRConfigCurrentRate(conf);

    XCloseDisplay(dpy);
    window.setFramerateLimit(refresh_rate);

    // loading configs
    while (!data::init()) {
        continue;
    }

    // initialize input
    input::init();

    bool is_reload = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;

            // get reload config prompt
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::R && event.key.control) {
                    if (!is_reload) {
                        while (!data::init()) {
                            continue;
                        }
                    }
                    is_reload = true;
                    break;
                }
            default:
                is_reload = false;
            }
        }

        int mode = data::cfg["mode"].asInt();
        int red_value = data::cfg["decoration"]["rgb"][0].asInt();
        int green_value = data::cfg["decoration"]["rgb"][1].asInt();
        int blue_value = data::cfg["decoration"]["rgb"][2].asInt();

        window.clear(sf::Color(red_value, green_value, blue_value));
        switch (mode) {
        case 1:
            osu::draw();
            break;
        case 2:
            taiko::draw();
            break;
        case 3:
            ctb::draw();
            break;
        case 4:
            mania::draw();
            break;
        }

        window.display();
    }

    osu::cleanup();
    return 0;
}
