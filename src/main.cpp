#include "header.hpp"

#if !defined(__unix__) && !defined(__unix)
#include <windows.h>
#endif

sf::RenderWindow window;

#if defined(__unix__) || defined(__unix)
int main(int argc, char ** argv) {
#else
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
#endif

    // loading configs
    while (!data::init()) {
        continue;
    }

    Json::Value windowWidth = data::cfg["decoration"]["window_width"];
    Json::Value windowHeight = data::cfg["decoration"]["window_height"];
    window.create(sf::VideoMode(windowWidth.asInt(), windowHeight.asInt()), "Bongo Cat for osu!", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(MAX_FRAMERATE);


    // initialize input
    if (!input::init()) {
        return EXIT_FAILURE;
    }

    bool is_reload = false;
    bool is_show_input_debug = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;

            case sf::Event::KeyPressed:
                // get reload config prompt
                if (event.key.code == sf::Keyboard::R && event.key.control) {
                    if (!is_reload) {
                        while (!data::init()) {
                            continue;
                        }
                    }
                    is_reload = true;
                    break;
                }

                // toggle joystick debug panel
                if (event.key.code == sf::Keyboard::D && event.key.control) {
                    is_show_input_debug = !is_show_input_debug;
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

        // Translation for artifacts when resize is applied
        sf::Transform transform = sf::Transform();
        transform.translate(0, windowHeight.asInt()-BASE_HEIGHT);
        sf::RenderStates rstates = sf::RenderStates(transform);

        window.clear(sf::Color(red_value, green_value, blue_value, alpha_value));
        switch (mode) {
        case 1:
            osu::draw(rstates);
            break;
        case 2:
            taiko::draw(rstates);
            break;
        case 3:
            ctb::draw(rstates);
            break;
        case 4:
            mania::draw(rstates);
            break;
        case 5:
            custom::draw(rstates);
        }

        if (is_show_input_debug) {
            input::drawDebugPanel();
        }

        window.display();
    }

    input::cleanup();
    return 0;
}

