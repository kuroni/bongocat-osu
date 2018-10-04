#include "header.hpp"

sf::RenderWindow window;
HWND handle;
TCHAR w_title[256];
std::string title;

namespace data
{
Json::Value cfg;
std::map<std::string, sf::Texture> img_holder;

void create_config()
{
    std::ifstream f("config.json");
    if (!f.good())
    {
        std::ofstream cfg("config.json");
        std::string s =
            "{\
    \"mode\": 1,\
    \"resolution\": {\
        \"letterboxing\": false,\
        \"width\": 1920,\
        \"height\": 1080,\
        \"horizontalPosition\": 100,\
        \"verticalPosition\": -100\
    },\
    \"decoration\": {\
        \"red\": 255,\
        \"green\": 255,\
        \"blue\": 255,\
        \"leftHanded\": false,\
        \"offsetX\": [0, 11],\
        \"offsetY\": [0, -65],\
        \"scalar\": [1, 1]\
    },\
    \"osu\": {\
        \"mouse\": false,\
        \"key1\": 90,\
        \"key2\": 88\
    },\
    \"taiko\": {\
        \"keyLeftCentre\": 88,\
        \"keyRightCentre\": 67,\
        \"keyLeftRim\": 90,\
        \"keyRightRim\": 86\
    }\
}";
        cfg << s;
    }
}

void error_msg(std::string error, std::string title)
{
    MessageBoxA(NULL, error.c_str(), title.c_str(), MB_ICONERROR | MB_OK);
    exit(0);
}

bool init()
{
    create_config();
    std::ifstream cfg_file("config.json", std::ifstream::binary);
    std::string cfg_string((std::istreambuf_iterator<char>(cfg_file)), std::istreambuf_iterator<char>()), error;
    Json::CharReaderBuilder cfg_builder;
    Json::CharReader *cfg_reader = cfg_builder.newCharReader();
    if (!cfg_reader->parse(cfg_string.c_str(), cfg_string.c_str() + cfg_string.size(), &cfg, &error))
        error_msg(error, "Error reading config.json");

    delete cfg_reader;
    cfg_file.close();
    img_holder.clear();

    int mode = data::cfg["mode"].asInt();

    switch (mode)
    {
    case 1:
        osu::init();
        break;
        // case 2: taiko::init(); break;
        // case 3: catch::init(); break;
        // case 4: mania::init(); break;
    }
    return true;
}

sf::Texture &load_texture(std::string path)
{
    if (img_holder.find(path) == img_holder.end())
        if (!img_holder[path].loadFromFile(path))
            error_msg("Cannot find file " + path, "Error importing images");
    return img_holder[path];
}
}; // namespace data

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    window.create(sf::VideoMode(612, 352), "Bongo Cat for osu!", sf::Style::Titlebar | sf::Style::Close);
    // window.setFramerateLimit(240);

    // loading configs
    data::init();

    int mode = data::cfg["mode"].asInt();
    int red_value = data::cfg["decoration"]["red"].asInt();
    int green_value = data::cfg["decoration"]["green"].asInt();
    int blue_value = data::cfg["decoration"]["blue"].asInt();

    bool is_reload = false;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
            if (event.type == sf::Event::Closed)
                window.close();
        window.clear(sf::Color(red_value, green_value, blue_value));

        bool is_bongo = false;

        handle = GetForegroundWindow();
        if (handle)
        {
            TCHAR w_title[256];
            GetWindowText(handle, w_title, GetWindowTextLength(handle));
            std::string title = w_title;
            is_bongo = (title.find("Bongo Cat for osu!") == 0);
        }

        // reloading config device
        if ((GetKeyState(VK_ESCAPE) & 0x8000) && is_bongo)
        {
            if (!is_reload)
                data::init();
            is_reload = true;
        }
        else
            is_reload = false;

        switch (mode)
        {
        case 1:
            osu::draw();
            break;
            // case 2: taiko::draw(); break;
            // case 3: catch::draw(); break;
            // case 4: mania::draw(); break;
        }

        window.display();
    }

    return 0;
}