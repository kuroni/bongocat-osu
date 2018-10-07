#include "header.hpp"

namespace taiko
{
Json::Value rim_key_value[2], centre_key_value[2];
sf::Sprite bg, up[2], rim[2], centre[2];

int key_state[2] = {0, 0};
bool rim_key_state[2] = {false, false};
bool centre_key_state[2] = {false, false};
double timer_rim_key[2] = {-1, -1};
double timer_centre_key[2] = {-1, -1};

bool init()
{
    // getting configs
    bool chk[256];

    std::fill(chk, chk + 256, false);
    rim_key_value[0] = data::cfg["taiko"]["leftRim"];
    for (Json::Value &v : rim_key_value[0])
        chk[v.asInt()] = true;
    centre_key_value[0] = data::cfg["taiko"]["leftCentre"];
    for (Json::Value &v : centre_key_value[0])
        if (chk[v.asInt()])
        {
            data::error_msg("Overlapping osu!taiko keybinds", "Error reading configs");
            return false;
        }

    std::fill(chk, chk + 256, false);
    rim_key_value[1] = data::cfg["taiko"]["rightRim"];
    for (Json::Value &v : rim_key_value[1])
        chk[v.asInt()] = true;
    centre_key_value[1] = data::cfg["taiko"]["rightCentre"];
    for (Json::Value &v : centre_key_value[1])
        if (chk[v.asInt()])
        {
            data::error_msg("Overlapping osu!taiko keybinds", "Error reading configs");
            return false;
        }

    // importing sprites
    bg.setTexture(data::load_texture("img/taiko/bg.png"));
    up[0].setTexture(data::load_texture("img/taiko/leftup.png"));
    rim[0].setTexture(data::load_texture("img/taiko/leftrim.png"));
    centre[0].setTexture(data::load_texture("img/taiko/leftcentre.png"));
    up[1].setTexture(data::load_texture("img/taiko/rightup.png"));
    rim[1].setTexture(data::load_texture("img/taiko/rightrim.png"));
    centre[1].setTexture(data::load_texture("img/taiko/rightcentre.png"));

    return true;
}

void draw()
{
    window.draw(bg);

    // 0 for left side, 1 for right side
    for (int i = 0; i < 2; i++)
    {
        bool rim_key = false;
        for (Json::Value &v : rim_key_value[i])
            if (GetKeyState(v.asInt()) & 0x8000)
            {
                rim_key = true;
                break;
            }
        if (rim_key)
        {
            if (!rim_key_state[i])
            {
                key_state[i] = 1;
                rim_key_state[i] = true;
            }
        }
        else
            rim_key_state[i] = false;

        bool centre_key = false;
        for (Json::Value &v : centre_key_value[i])
            if (GetKeyState(v.asInt()) & 0x8000)
            {
                centre_key = true;
                break;
            }
        if (centre_key)
        {
            if (!centre_key_state[i])
            {
                key_state[i] = 2;
                centre_key_state[i] = true;
            }
        }
        else
            centre_key_state[i] = false;

        if (!rim_key_state[i] && !centre_key_state[i])
        {
            key_state[i] = 0;
            window.draw(up[i]);
        }
        if (key_state[i] == 1)
        {
            if ((clock() - timer_centre_key[i]) / CLOCKS_PER_SEC > 0.031)
            {
                window.draw(rim[i]);
                timer_rim_key[i] = clock();
            }
            else
                window.draw(up[i]);
        }
        else if (key_state[i] == 2)
        {
            if ((clock() - timer_rim_key[i]) / CLOCKS_PER_SEC > 0.031)
            {
                window.draw(centre[i]);
                timer_centre_key[i] = clock();
            }
            else
                window.draw(up[i]);
        }
    }
}
}; // namespace taiko