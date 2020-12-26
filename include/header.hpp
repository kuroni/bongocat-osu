#pragma once
#define BONGO_KEYPRESS_THRESHOLD 0
#define WINDOW_WIDTH 612
#define WINDOW_HEIGHT 352
#define MAX_FRAMERATE 60

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
bool init();

bool is_pressed(int key_code);

bool is_joystick_connected();
bool is_joystick_pressed(int key_code);

std::pair<double, double> bezier(double ratio, std::vector<double> &points, int length);

std::pair<double, double> get_xy();

void drawDebugPanel();

void cleanup();
}; // namespace input

namespace osu {
bool init();

void draw();
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

namespace custom {
bool init();

void draw();
}; // namespace custom
