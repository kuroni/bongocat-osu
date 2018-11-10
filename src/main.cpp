#include "header.hpp"

sf::RenderWindow window;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    window.create(sf::VideoMode(612, 352), "Bongo Cat for osu!", sf::Style::Titlebar | sf::Style::Close);
    window.setVerticalSyncEnabled(true);

    // loading configs
    while (!data::init())
        continue;

    bool is_reload = false;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;

            // get reload config prompt
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::R && event.key.control)
                {
                    if (!is_reload)
                        while (!data::init())
                            continue;
                    is_reload = true;
                    break;
                }
            default:
                is_reload = false;
            }

        int mode = data::cfg["mode"].asInt();
        int red_value = data::cfg["decoration"]["rgb"][0].asInt();
        int green_value = data::cfg["decoration"]["rgb"][1].asInt();
        int blue_value = data::cfg["decoration"]["rgb"][2].asInt();

        window.clear(sf::Color(red_value, green_value, blue_value));
        switch (mode)
        {
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

    return 0;
}
