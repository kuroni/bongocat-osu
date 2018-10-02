#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>

#include <time.h>
#include <windows.h>
#include <math.h>

#include <SFML/Graphics.hpp>
#include "json/json.h"

sf::RenderWindow window(sf::VideoMode(612, 352), "ESC for switching device", sf::Style::Titlebar | sf::Style::Close);
HWND handle;
TCHAR w_title[256];
std::string title;

namespace data
{
std::map<std::string, sf::Texture> img_holder;
Json::Value cfg;

void create_config()
{
    std::ifstream f("config.json");
    if (!f.good())
    {
        std::ofstream cfg("config.json");
        cfg << "{ \n \
	\"resolution\": { \n \
		\"letterboxing\": false, \n \
		\"width\": 1920, \n \
		\"height\": 1080, \n \
		\"horizontalPosition\": 100, \n \
		\"verticalPosition\": -100 \n \
	}, \n \
	\"decoration\": { \n \
		\"red\": 255, \n \
		\"green\": 255, \n \
		\"blue\": 255, \n \
		\"leftHanded\": false, \n \
		\"rgbArm\": false, \n \
		\"mouseXOffset\": 0, \n \
		\"mouseYOffset\": 0, \n \
		\"mouseScalar\": 1, \n \
		\"tabletXOffset\": 11, \n \
		\"tabletYOffset\": -65, \n \
		\"tabletScalar\": 1 \n \
	}, \n \
	\"osu\": { \n \
		\"mouse\": true, \n \
		\"key1\": 90, \n \
		\"key2\": 88 \n \
	} \n \
}";
    }
}

bool init()
{
    create_config();
    std::ifstream cfg_file("config.json", std::ifstream::binary);
    std::string cfg_string((std::istreambuf_iterator<char>(cfg_file)), std::istreambuf_iterator<char>());
    Json::Reader cfg_reader;
    if (!cfg_reader.parse(cfg_string, cfg))
    {
        std::cout << "Error reading the config\n";
        return false;
    }
    cfg_file.close();

    img_holder.clear();
    return true;
}

sf::Texture &load_texture(std::string path)
{
    if (img_holder.find(path) == img_holder.end())
        if (!img_holder[path].loadFromFile(path))
        {
            // handle error here
        }
    return img_holder[path];
}
}; // namespace data

namespace osu
{
int left_key_value, right_key_value;
int osu_x, osu_y, osu_h, osu_v;
int offset_x, offset_y, scale;
int horizontal, vertical;
bool is_mouse, is_letterbox, is_left_handed;
sf::Sprite bg, up, left, right, device;

int key_state = 0;
bool left_key_state = false;
bool right_key_state = false;
double timer_left_key = -1;
double timer_right_key = -1;

std::tuple<double, double> bezier(double ratio, std::vector<double> &points, int length)
{
    double fact[22] = {0.001, 0.001, 0.002, 0.006, 0.024, 0.12, 0.72, 5.04, 40.32, 362.88, 3628.8, 39916.8, 479001.6, 6227020.8, 87178291.2, 1307674368.0, 20922789888.0, 355687428096.0, 6402373705728.0, 121645100408832.0, 2432902008176640.0, 51090942171709440.0};
    int nn = (length / 2) - 1;
    double xx = 0;
    double yy = 0;

    for (int point = 0; point <= nn; point++)
    {
        double tmp = fact[nn] / (fact[point] * fact[nn - point]) * pow(ratio, point) * pow(1 - ratio, nn - point);
        xx += points[2 * point] * tmp;
        yy += points[2 * point + 1] * tmp;
    }

    return std::make_tuple(xx / 1000, yy / 1000);
}

void init()
{
    // getting configs
    is_mouse = data::cfg["osu"]["mouse"].asBool();
    left_key_value = data::cfg["osu"]["key1"].asInt();
    right_key_value = data::cfg["osu"]["key2"].asInt();

    is_letterbox = data::cfg["resolution"]["letterboxing"].asBool();
    osu_x = data::cfg["resolution"]["width"].asInt();
    osu_y = data::cfg["resolution"]["height"].asInt();
    osu_h = data::cfg["resolution"]["horizontalPosition"].asInt();
    osu_v = data::cfg["resolution"]["verticalPosition"].asInt();
    is_left_handed = data::cfg["decoration"]["leftHanded"].asBool();

    if (is_mouse)
    {
        offset_x = (data::cfg["decoration"]["offsetX"])[0].asInt();
        offset_y = (data::cfg["decoration"]["offsetY"])[0].asInt();
        scale = (data::cfg["decoration"]["scalar"])[0].asInt();
    }
    else
    {
        offset_x = (data::cfg["decoration"]["offsetX"])[1].asInt();
        offset_y = (data::cfg["decoration"]["offsetY"])[1].asInt();
        scale = (data::cfg["decoration"]["scalar"])[1].asInt();
    }
    // reading sprites
    up.setTexture(data::load_texture("img/osu/up.png"));
    left.setTexture(data::load_texture("img/osu/left.png"));
    right.setTexture(data::load_texture("img/osu/right.png"));
    if (is_mouse)
    {
        bg.setTexture(data::load_texture("img/osu/mousebg.png"));
        device.setTexture(data::load_texture("img/osu/mouse.png"), true);
    }
    else
    {
        bg.setTexture(data::load_texture("img/osu/tabletbg.png"));
        device.setTexture(data::load_texture("img/osu/tablet.png"), true);
    }
    device.setScale(scale, scale);

    // getting resolution
    RECT desktop;
    const HWND h_desktop = GetDesktopWindow();
    GetWindowRect(h_desktop, &desktop);
    horizontal = desktop.right;
    vertical = desktop.bottom;
}

void draw()
{
    // getting device resolution
    double letter_x, letter_y, s_height, s_width;
    if (handle)
    {
        if (title.find("osu!") < 300)
        {
            RECT oblong;
            GetWindowRect(handle, &oblong);
            s_height = osu_y * 0.8;
            s_width = s_height * 4 / 3;
            if (!is_letterbox)
            {
                letter_x = oblong.left + ((oblong.right - oblong.left) - s_width) / 2;
                letter_y = oblong.top + osu_y * 0.117;
            }
            else
            {
                double l = (horizontal - osu_x) * (osu_h + 100) / 200.0;
                double r = l + osu_x;
                letter_x = l + ((r - l) - s_width) / 2;
                letter_y = (vertical - osu_y) * (osu_v + 100) / 200.0 + osu_y * 0.117;
            }
        }
        else
        {
            s_width = horizontal;
            s_height = vertical;
            letter_x = 0;
            letter_y = 0;
        }
    }
    else
    {
        s_width = horizontal;
        s_height = vertical;
        letter_x = 0;
        letter_y = 0;
    }
    double x, y;
    POINT point;
    if (GetCursorPos(&point))
    {
        double fx = ((double)point.x - letter_x) / s_width;
        if (is_left_handed)
            fx = 1 - fx;
        double fy = ((double)point.y - letter_y) / s_height;
        fx = std::min(fx, 1.0);
        fx = std::max(fx, 0.0);
        fy = std::min(fy, 1.0);
        fy = std::max(fy, 0.0);
        x = -97 * fx + 44 * fy + 184;
        y = -76 * fx - 40 * fy + 324;
    }
    window.draw(bg);

    // drawing keypresses
    if (GetKeyState(left_key_value) & 0x8000)
    {
        if (!left_key_state)
        {
            key_state = 1;
            left_key_state = true;
        }
    }
    else
        left_key_state = false;

    if (GetKeyState(right_key_value) & 0x8000)
    {
        if (!right_key_state)
        {
            key_state = 2;
            right_key_state = true;
        }
    }
    else
        right_key_state = false;
    if (!left_key_state && !right_key_state)
    {
        key_state = 0;
        window.draw(up);
    }
    if (key_state == 1)
    {
        if (clock() - timer_right_key > 31)
        {
            window.draw(left);
            timer_left_key = clock();
        }
        else
            window.draw(up);
    }
    else if (key_state == 2)
    {
        if (clock() - timer_left_key > 31)
        {
            window.draw(right);
            timer_right_key = clock();
        }
        else
            window.draw(up);
    }

    // initializing pss and pss2 (kuvster's magic)
    int oof = 6;
    std::vector<double> pss = {211.0, 159.0};
    double dist = hypot(211 - x, 159 - y);
    double centreleft0 = 211 - 0.7237 * dist / 2;
    double centreleft1 = 159 + 0.69 * dist / 2;
    for (double i = 1; i < oof; i++)
    {
        double p0;
        double p1;
        std::vector<double> bez = {211, 159, centreleft0, centreleft1, x, y};
        std::tie(p0, p1) = bezier(i / oof, bez, 6);
        pss.push_back(p0);
        pss.push_back(p1);
    }
    pss.push_back(x);
    pss.push_back(y);
    double a = y - centreleft1;
    double b = centreleft0 - x;
    double le = hypot(a, b);
    a = x + a / le * 60;
    b = y + b / le * 60;
    int a1 = 258;
    int a2 = 228;
    dist = hypot(a1 - a, a2 - b);
    double centreright0 = a1 - 0.6 * dist / 2;
    double centreright1 = a2 + 0.8 * dist / 2;
    int push = 20;
    double s = x - centreleft0;
    double t = y - centreleft1;
    le = hypot(s, t);
    s *= push / le;
    t *= push / le;
    double s2 = a - centreright0;
    double t2 = b - centreright1;
    le = hypot(s2, t2);
    s2 *= push / le;
    t2 *= push / le;
    for (double i = 1; i < oof; i++)
    {
        double p0;
        double p1;
        std::vector<double> bez = {x, y, x + s, y + t, a + s2, b + t2, a, b};
        std::tie(p0, p1) = bezier(i / oof, bez, 8);
        pss.push_back(p0);
        pss.push_back(p1);
    }
    pss.push_back(a);
    pss.push_back(b);
    for (double i = oof - 1; i > 0; i--)
    {
        double p0;
        double p1;
        std::vector<double> bez = {1.0 * a1, 1.0 * a2, centreright0, centreright1, a, b};
        std::tie(p0, p1) = bezier(i / oof, bez, 6);
        pss.push_back(p0);
        pss.push_back(p1);
    }
    pss.push_back(a1);
    pss.push_back(a2);
    double mpos0 = (a + x) / 2 - 52 - 15;
    double mpos1 = (b + y) / 2 - 34 + 5;
    double dx = -38;
    double dy = -50;

    const int iter = 25;

    std::vector<double> pss2 = {pss[0] + dx, pss[1] + dy};
    for (double i = 1; i < iter; i++)
    {
        double p0;
        double p1;
        std::tie(p0, p1) = bezier(i / iter, pss, 38);
        pss2.push_back(p0 + dx);
        pss2.push_back(p1 + dy);
    }
    pss2.push_back(pss[36] + dx);
    pss2.push_back(pss[37] + dy);

    device.setPosition(mpos0 + dx + offset_x, mpos1 + dy + offset_y);

    // drawing mouse
    if (is_mouse)
        window.draw(device);

    // drawing arms
    sf::VertexArray fill(sf::TriangleStrip, 26);
    for (int i = 0; i < 26; i += 2)
    {
        fill[i].position = sf::Vector2f(pss2[i], pss2[i + 1]);
        fill[i + 1].position = sf::Vector2f(pss2[52 - i - 2], pss2[52 - i - 1]);
    }
    window.draw(fill);

    // drawing circ
    int shad = 77;
    sf::VertexArray edge(sf::TriangleStrip, 52);
    double width = 7;
    sf::CircleShape circ(width / 2);
    circ.setFillColor(sf::Color(0, 0, 0, shad));
    circ.setPosition(pss2[0] - width / 2, pss2[1] - width / 2);
    window.draw(circ);
    for (int i = 0; i < 50; i += 2)
    {
        double vec0 = pss2[i] - pss2[i + 2];
        double vec1 = pss2[i + 1] - pss2[i + 3];
        double dist = hypot(vec0, vec1);
        edge[i].position = sf::Vector2f(pss2[i] + vec1 / dist * width / 2, pss2[i + 1] - vec0 / dist * width / 2);
        edge[i + 1].position = sf::Vector2f(pss2[i] - vec1 / dist * width / 2, pss2[i + 1] + vec0 / dist * width / 2);
        edge[i].color = sf::Color(0, 0, 0, shad);
        edge[i + 1].color = sf::Color(0, 0, 0, shad);
        width -= 0.08;
    }
    double vec0 = pss2[50] - pss2[48];
    double vec1 = pss2[51] - pss2[49];
    dist = hypot(vec0, vec1);
    edge[51].position = sf::Vector2f(pss2[50] + vec1 / dist * width / 2, pss2[51] - vec0 / dist * width / 2);
    edge[50].position = sf::Vector2f(pss2[50] - vec1 / dist * width / 2, pss2[51] + vec0 / dist * width / 2);
    edge[50].color = sf::Color(0, 0, 0, shad);
    edge[51].color = sf::Color(0, 0, 0, shad);
    window.draw(edge);
    circ.setRadius(width / 2);
    circ.setPosition(pss2[50] - width / 2, pss2[51] - width / 2);
    window.draw(circ);

    // drawing circ2
    sf::VertexArray edge2(sf::TriangleStrip, 52);
    width = 6;
    sf::CircleShape circ2(width / 2);
    circ2.setFillColor(sf::Color::Black);
    circ2.setPosition(pss2[0] - width / 2, pss2[1] - width / 2);
    window.draw(circ2);
    for (int i = 0; i < 50; i += 2)
    {
        vec0 = pss2[i] - pss2[i + 2];
        vec1 = pss2[i + 1] - pss2[i + 3];
        double dist = hypot(vec0, vec1);
        edge2[i].position = sf::Vector2f(pss2[i] + vec1 / dist * width / 2, pss2[i + 1] - vec0 / dist * width / 2);
        edge2[i + 1].position = sf::Vector2f(pss2[i] - vec1 / dist * width / 2, pss2[i + 1] + vec0 / dist * width / 2);
        edge2[i].color = sf::Color::Black;
        edge2[i + 1].color = sf::Color::Black;
        width -= 0.08;
    }
    vec0 = pss2[50] - pss2[48];
    vec1 = pss2[51] - pss2[49];
    dist = hypot(vec0, vec1);
    edge2[51].position = sf::Vector2f(pss2[50] + vec1 / dist * width / 2, pss2[51] - vec0 / dist * width / 2);
    edge2[50].position = sf::Vector2f(pss2[50] - vec1 / dist * width / 2, pss2[51] + vec0 / dist * width / 2);
    edge2[50].color = sf::Color::Black;
    edge2[51].color = sf::Color::Black;
    window.draw(edge2);
    circ2.setRadius(width / 2);
    circ2.setPosition(pss2[50] - width / 2, pss2[51] - width / 2);
    window.draw(circ2);

    // drawing tablet
    if (!is_mouse)
        window.draw(device);
}
}; // namespace osu

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    window.setFramerateLimit(240);

    // loading configs

    if (data::init())
        std::cout << "Reading config completed!\n";
    osu::init();

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
            is_bongo = (title.find("ESC") < 300);
        }

        // ESCAPE for switching device
        // A few suggestion here, instead of switching device, we can implement so that ESCAPE means reloading the config.json file
        // Which is better for later on since I'm planning to support other game modes
        if ((GetKeyState(VK_ESCAPE) & 0x8000) && is_bongo)
        {
            if (!is_reload)
            {
                data::init();
                osu::init();
            }
            is_reload = true;
        }
        else
            is_reload = false;

        osu::draw();

        window.display();
    }

    return 0;
}