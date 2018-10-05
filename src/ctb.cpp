#include "header.hpp"

namespace ctb
{
int left_key_value, right_key_value, dash_key_value;
sf::Sprite bg, up, left, right, dash;

int key_state = 0;
bool left_key_state = false;
bool right_key_state = false;
double timer_left_key = -1;
double timer_right_key = -1;

void init()
{
    // getting configs
    left_key_value = data::cfg["catch"]["left"].asInt();
    right_key_value = data::cfg["catch"]["right"].asInt();
    dash_key_value = data::cfg["catch"]["dash"].asInt();

    // importing sprites
    bg.setTexture(data::load_texture("img/catch/bg.png"));
    up.setTexture(data::load_texture("img/catch/up.png"));
    left.setTexture(data::load_texture("img/catch/left.png"));
    right.setTexture(data::load_texture("img/catch/right.png"));
    dash.setTexture(data::load_texture("img/catch/dash.png"));
}

void draw()
{
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
        if ((clock() - timer_right_key) / CLOCKS_PER_SEC > 0.031)
        {
            window.draw(left);
            timer_left_key = clock();
        }
        else
            window.draw(up);
    }
    else if (key_state == 2)
    {
        if ((clock() - timer_left_key) / CLOCKS_PER_SEC > 0.031)
        {
            window.draw(right);
            timer_right_key = clock();
        }
        else
            window.draw(up);
    }

    if (GetKeyState(dash_key_value) & 0x8000)
        window.draw(dash);
}
}; // namespace ctb