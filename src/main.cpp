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
        const char *s =
            R"V0G0N({
    "mode": 1,
    "resolution": {
        "letterboxing": false,
        "width": 1920,
        "height": 1080,
        "horizontalPosition": 100,
        "verticalPosition": -100
    },
    "decoration": {
        "leftHanded": false,
        "rgb": [255, 255, 255],
        "offsetX": [0, 11],
        "offsetY": [0, -65],
        "scalar": [1, 1]
    },
    "osu": {
        "mouse": true,
        "key1": [90],
        "key2": [88]
    },
    "taiko": {
        "leftCentre": [88],
        "rightCentre": [67],
        "leftRim": [90],
        "rightRim": [86]
    },
    "catch": {
        "left": [37],
        "right": [39],
        "dash": [16]
    }
})V0G0N";
        cfg << s;
    }
}

void error_msg(std::string error, std::string title)
{
    if (MessageBoxA(NULL, error.c_str(), title.c_str(), MB_ICONERROR | MB_RETRYCANCEL) == IDCANCEL)
        exit(0);
}

bool init()
{
    create_config();
    while (true)
    {
        std::ifstream cfg_file("config.json", std::ifstream::binary);
        std::string cfg_string((std::istreambuf_iterator<char>(cfg_file)), std::istreambuf_iterator<char>()), error;
        Json::CharReaderBuilder cfg_builder;
        Json::CharReader *cfg_reader = cfg_builder.newCharReader();
        if (!cfg_reader->parse(cfg_string.c_str(), cfg_string.c_str() + cfg_string.size(), &cfg, &error))
            error_msg("Syntax error in config.json", "Error reading configs");
        else
            break;
    }

    img_holder.clear();

    int mode = data::cfg["mode"].asInt();

    switch (mode)
    {
    case 1:
        return osu::init();
    case 2:
        return taiko::init();
    case 3:
        return ctb::init();
    // case 4:
    //     mania::init();
    //     break;
    default:
        error_msg("Mode value is not correct", "Error reading configs");
        return false;
    }
}

sf::Texture &load_texture(std::string path)
{
    if (img_holder.find(path) == img_holder.end())
        while (!img_holder[path].loadFromFile(path))
            error_msg("Cannot find file " + path, "Error importing images");
    return img_holder[path];
}
}; // namespace data

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