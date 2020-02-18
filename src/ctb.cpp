#include "header.hpp"

namespace ctb {
Json::Value left_key_value, right_key_value, dash_key_value;
sf::Sprite bg, mid, left, right, dash, up;

int key_state = 0;
bool left_key_state = false;
bool right_key_state = false;
double timer_left_key = -1;
double timer_right_key = -1;

bool init() {
    // getting configs
    bool chk[256];
    std::fill(chk, chk + 256, false);
    left_key_value = data::cfg["catch"]["left"];
    for (Json::Value &v : left_key_value) {
        chk[v.asInt()] = true;
    }
    right_key_value = data::cfg["catch"]["right"];
    for (Json::Value &v : right_key_value) {
        if (chk[v.asInt()]) {
            data::error_msg("Overlapping osu!catch keybinds", "Error reading configs");
            return false;
        }
    }
    dash_key_value = data::cfg["catch"]["dash"];

    // importing sprites
    bg.setTexture(data::load_texture("img/catch/bg.png"));
    mid.setTexture(data::load_texture("img/catch/mid.png"));
    left.setTexture(data::load_texture("img/catch/left.png"));
    right.setTexture(data::load_texture("img/catch/right.png"));
    dash.setTexture(data::load_texture("img/catch/dash.png"));
    up.setTexture(data::load_texture("img/catch/up.png"));

    return true;
}

void draw() {
    window.draw(bg);
    
    // drawing left-right keypresses
    bool left_key = false;
    for (Json::Value &v : left_key_value) {
        if (GetKeyState(v.asInt()) & WM_KEYDOWN) {
            left_key = true;
            break;
        }
    }
    if (left_key) {
        if (!left_key_state) {
            key_state = 1;
            left_key_state = true;
        }
    } else {
        left_key_state = false;
    }

    bool right_key = false;
    for (Json::Value &v : right_key_value) {
        if (GetKeyState(v.asInt()) & WM_KEYDOWN) {
            right_key = true;
            break;
        }
    }
    if (right_key) {
        if (!right_key_state) {
            key_state = 2;
            right_key_state = true;
        }
    } else {
        right_key_state = false;
    }

    if (!left_key_state && !right_key_state) {
        key_state = 0;
        window.draw(mid);
    }
    if (key_state == 1) {
        if ((clock() - timer_right_key) / CLOCKS_PER_SEC > 0.031) {
            window.draw(left);
            timer_left_key = clock();
        } else {
            window.draw(mid);
        }
    } else if (key_state == 2) {
        if ((clock() - timer_left_key) / CLOCKS_PER_SEC > 0.031) {
            window.draw(right);
            timer_right_key = clock();
        } else {
            window.draw(mid);
        }
    }

    bool is_dash = false;
    for (Json::Value &v : dash_key_value) {
        if (GetKeyState(v.asInt()) & WM_KEYDOWN) {
            window.draw(dash);
            is_dash = true;
            break;
        }
    }
    if (!is_dash) {
        window.draw(up);
    }
}
}; // namespace ctb
