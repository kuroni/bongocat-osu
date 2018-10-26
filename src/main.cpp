#include "header.hpp"

sf::RenderWindow window;
HWND handle;
TCHAR w_title[256];
std::string title;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    window.create(sf::VideoMode(612, 352), "Bongo Cat for osu!", sf::Style::Titlebar | sf::Style::Close);

    // loading configs
    while (!data::init())
        continue;

    bool is_reload = false;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
            if (event.type == sf::Event::Closed)
                window.close();

        bool is_bongo = false;

        handle = GetForegroundWindow();
        if (handle)
        {
            TCHAR w_title[256];
            GetWindowText(handle, w_title, GetWindowTextLength(handle));
            std::string title = w_title;
            is_bongo = (title.find("Bongo Cat for osu") == 0);
        }

        // reloading config device
        if ((GetKeyState(VK_ESCAPE) & 0x8000) && is_bongo)
        {
            if (!is_reload)
                while (!data::init())
                    continue;
            is_reload = true;
        }
        else
            is_reload = false;

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
        // case 4:
        //     mania::draw();
        //     break;
        }

        window.display();
    }

    return 0;
}