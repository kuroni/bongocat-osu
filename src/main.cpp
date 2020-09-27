#include "header.hpp"

#if defined(__unix__) || defined(__unix)
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#else
#include <windows.h>
#endif

sf::RenderWindow window;

#if defined(__unix__) || defined(__unix)
int main(int argc, char ** argv) {
#else
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
#endif

    window.create(sf::VideoMode(612, 352), "Bongo Cat for osu!", sf::Style::Titlebar | sf::Style::Close);

    // get refresh rate and set the frame limit
#if defined(__unix__) || defined(__unix)
    Display *dpy = XOpenDisplay(NULL);
    Window root = RootWindow(dpy, 0);

    XRRScreenConfiguration *conf = XRRGetScreenInfo(dpy, root);
    short refresh_rate = XRRConfigCurrentRate(conf);

    XCloseDisplay(dpy);
    window.setFramerateLimit(refresh_rate);
#else
    DISPLAY_DEVICE device;
    ZeroMemory(&device, sizeof(device));
    device.cb = sizeof(device);
    EnumDisplayDevices(NULL, (DWORD)0, &device, 0);
    DEVMODE devmode;
    ZeroMemory(&devmode, sizeof(DEVMODE));
    devmode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettings((LPSTR)device.DeviceName, ENUM_REGISTRY_SETTINGS, &devmode);
    window.setFramerateLimit(devmode.dmDisplayFrequency);
#endif

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

        Json::Value rgb = data::cfg["decoration"]["rgb"];
        int red_value = rgb[0].asInt();
        int green_value = rgb[1].asInt();
        int blue_value = rgb[2].asInt();
        int alpha_value = rgb.size() == 3 ? 255 : rgb[3].asInt();

        window.clear(sf::Color(red_value, green_value, blue_value, alpha_value));
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
        case 5:
            custom::draw();
        }

        window.display();
    }

    input::cleanup();
    return 0;
}
