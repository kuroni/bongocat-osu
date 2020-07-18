#include "header.hpp"

namespace mania {
sf::Sprite bg, left_handup, right_handup, left_hand[3], right_hand[3];
sf::Sprite left_4K[2], right_4K[2], left_7K[4], right_7K[4];
int left_key_value_4K[2], right_key_value_4K[2];
int left_key_value_7K[4], right_key_value_7K[4];
bool is_4K;

bool init() {
    // getting configs
    Json::Value mania = data::cfg["mania"];

    is_4K = mania["4K"].asBool();

    for (int i = 0; i < 2; i++) {
        left_key_value_4K[i] = mania["key4K"][i].asInt();
    }
    for (int i = 0; i < 2; i++) {
        right_key_value_4K[i] = mania["key4K"][i + 2].asInt();
    }

    for (int i = 0; i < 4; i++) {
        left_key_value_7K[i] = mania["key7K"][i].asInt();
    }
    for (int i = 0; i < 4; i++) {
        right_key_value_7K[i] = mania["key7K"][i + 3].asInt();
    }

    // importing sprites
    left_handup.setTexture(data::load_texture("img/mania/leftup.png"));
    right_handup.setTexture(data::load_texture("img/mania/rightup.png"));
    for (int i = 0; i < 3; i++) {
        left_hand[i].setTexture(data::load_texture("img/mania/left" + std::to_string(i) + ".png"));
        right_hand[i].setTexture(data::load_texture("img/mania/right" + std::to_string(i) + ".png"));
    }

    if (is_4K) {
        bg.setTexture(data::load_texture("img/mania/4K/bg.png"));
        for (int i = 0; i < 2; i++) {
            left_4K[i].setTexture(data::load_texture("img/mania/4K/" + std::to_string(i) + ".png"));
        }
        for (int i = 0; i < 2; i++) {
            right_4K[i].setTexture(data::load_texture("img/mania/4K/" + std::to_string(i + 2) + ".png"));
        }
    } else {
        bg.setTexture(data::load_texture("img/mania/7K/bg.png"));
        for (int i = 0; i < 4; i++) {
            left_7K[i].setTexture(data::load_texture("img/mania/7K/" + std::to_string(i) + ".png"));
        }
        for (int i = 0; i < 4; i++) {
            right_7K[i].setTexture(data::load_texture("img/mania/7K/" + std::to_string(i + 3) + ".png"));
        }
    }

    return true;
}

void draw_4K() {
    window.draw(bg);

    int left_cnt = 0, right_cnt = 0;
    int left_sum = 0, right_sum = 0;

    for (int i = 0; i < 2; i++) {
        if (GetKeyState(left_key_value_4K[i]) & WM_KEYDOWN) {
            window.draw(left_4K[i]);
            left_cnt++;
            left_sum += i;
        }
        if (GetKeyState(right_key_value_4K[i]) & WM_KEYDOWN) {
            window.draw(right_4K[i]);
            right_cnt++;
            right_sum += i;
        }
    }

    // draw left hand
    if (left_cnt == 0) {
        window.draw(left_handup);
    } else {
        double avg = 1.0 * left_sum / left_cnt;
        if (avg == 0) {
            window.draw(left_hand[0]);
        } else if (avg == 0.5) {
            window.draw(left_hand[1]);
        } else {
            window.draw(left_hand[2]);
        }
    }

    // draw right hand
    if (right_cnt == 0) {
        window.draw(right_handup);
    } else {
        double avg = 1.0 * right_sum / right_cnt;
        if (avg == 0) {
            window.draw(right_hand[0]);
        } else if (avg == 0.5) {
            window.draw(right_hand[1]);
        } else {
            window.draw(right_hand[2]);
        }
    }
}

void draw_7K() {
    window.draw(bg);

    int left_cnt = 0, right_cnt = 0;
    int left_sum = 0, right_sum = 0;

    for (int i = 0; i < 4; i++) {
        if (GetKeyState(left_key_value_7K[i]) & WM_KEYDOWN) {
            window.draw(left_7K[i]);
            left_cnt++;
            left_sum += i;
        }
        if (GetKeyState(right_key_value_7K[i]) & WM_KEYDOWN) {
            window.draw(right_7K[i]);
            right_cnt++;
            right_sum += i;
        }
    }

    // draw left hand
    if (left_cnt == 0) {
        window.draw(left_handup);
    } else {
        double avg = 1.0 * left_sum / left_cnt;
        if (avg < 1.0) {
            window.draw(left_hand[0]);
        } else if (avg <= 2.0) {
            window.draw(left_hand[1]);
        } else {
            window.draw(left_hand[2]);
        }
    }

    // draw right hand
    if (right_cnt == 0) {
        window.draw(right_handup);
    } else {
        double avg = 1.0 * right_sum / right_cnt;
        if (avg < 1.0) {
            window.draw(right_hand[0]);
        } else if (avg <= 2.0) {
            window.draw(right_hand[1]);
        } else {
            window.draw(right_hand[2]);
        }
    }
}

void draw() {
    if (is_4K) {
        draw_4K();
    } else {
        draw_7K();
    }
}
}; // namespace mania
