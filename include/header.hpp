#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>

#include <time.h>

#include <math.h>
#include <string.h>

#include <SFML/Graphics.hpp>
#include "json/json.h"

extern sf::RenderWindow window;

namespace data {
extern Json::Value cfg;

void error_msg(std::string error, std::string title);

bool init();

sf::Texture &load_texture(std::string path);
}; // namespace data

namespace input {
    void init();

    sf::Keyboard::Key ascii_to_key(int ascii_key);

    bool is_pressed(int ascii_key);

    bool is_pressed(sf::Keyboard::Key key);
}; // namespace input

namespace osu {
bool init();

void draw();

void cleanup();

}; // namespace osu

namespace taiko {
bool init();

void draw();
}; // namespace taiko

namespace ctb {
bool init();

void draw();
}; // namespace ctb

namespace mania {
bool init();

void draw();
}; // namespace mania
