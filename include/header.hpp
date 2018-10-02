#ifndef HEADER_HPP
#define HEADER_HPP

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

extern sf::RenderWindow window;
extern HWND handle;
extern TCHAR w_title[256];
extern std::string title;

namespace data
{
extern Json::Value cfg;

void create_config();

bool init();

sf::Texture &load_texture(std::string path);
}; // namespace data

namespace osu
{
std::tuple<double, double> bezier(double ratio, std::vector<double> &points, int length);

void init();

void draw();
}; // namespace osu

#endif // HEADER_HPP