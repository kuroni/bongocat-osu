#include "header.hpp"

namespace mania
{
sf::Sprite bg, left[4], right[4];
int left_key_value[2], right_key_value[2];

bool init()
{
    // getting configs
    left_key_value[0] = data::cfg["mania"]["key"][0].asInt();
    left_key_value[1] = data::cfg["mania"]["key"][1].asInt();
    right_key_value[0] = data::cfg["mania"]["key"][2].asInt();
    right_key_value[1] = data::cfg["mania"]["key"][3].asInt();

    // importing sprites
    bg.setTexture(data::load_texture("img/mania/bg.png"));
    for (int i = 0; i < 4; i++)
    {
        left[i].setTexture(data::load_texture("img/mania/left" + std::to_string(i) + ".png"));
        right[i].setTexture(data::load_texture("img/mania/right" + std::to_string(i) + ".png"));
    }

    return true;
}

void draw()
{
    window.draw(bg);

    // 0: not pressed, 1: left, 2: right, 3: both
    int left_state = 0;
    for (int i = 0; i < 2; i++)
        if (GetKeyState(left_key_value[i]) & 0x8000)
            left_state |= (1 << i);
    window.draw(left[left_state]);

    int right_state = 0;
    for (int i = 0; i < 2; i++)
        if (GetKeyState(right_key_value[i]) & 0x8000)
            right_state |= (1 << i);
    window.draw(right[right_state]);
}
}; // namespace mania
