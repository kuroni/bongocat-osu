#include "header.hpp"

namespace custom {
struct key {
    Json::Value key_value;
    sf::Sprite sprite;
    bool status;
    double timer;

    key(Json::Value _key_value) {
        sprite = sf::Sprite();
        if (_key_value.isMember("keyCodes") && _key_value["keyCodes"].isArray()) {
            key_value = _key_value["keyCodes"];
        } else {
            data::error_msg("Custom keyCodes values is not set correctly", "Error reading configs");
            throw;
        }
        if (_key_value.isMember("image") && _key_value["image"].isString()) {
            sprite = sf::Sprite();
            sprite.setTexture(data::load_texture(_key_value["image"].asString()));
        } else {
            data::error_msg("Custom image path is not set correctly", "Error reading configs");
            throw;
        }
        status = false;
        timer = -1;
    }

    bool is_pressed() {
        for (Json::Value &v : key_value) {
            if (input::is_pressed(v.asInt())) {
                return true;
            }
        }
        return false;
    }

    void draw() {
        window.draw(sprite);
        timer = clock();
    }
};

struct key_container {
    std::vector<key> keys;
    sf::Sprite default_sprite;
    int key_state;

    key_container(Json::Value key_container_value) {
        key_state = -1;
        if (key_container_value.isObject()) {
            if (!key_container_value.isMember("defaultImage")
                || !key_container_value["defaultImage"].isString()
                || !key_container_value.isMember("keys")
                || !key_container_value["keys"].isArray()) {
                data::error_msg("Key container's object error", "Error reading configs");
                throw;
            } else {
                default_sprite = sf::Sprite();
                default_sprite.setTexture(data::load_texture(key_container_value["defaultImage"].asString()));
                for (Json::Value &child_key : key_container_value["keys"]) {
                    keys.push_back(key(child_key));
                }
            }
        } else {
            data::error_msg("Key container must be an object", "Error reading configs");
            throw;
        }
    }

    void draw() {
        bool is_any_key_pressed = false;
        for (int i = 0; i < keys.size(); i++) {
            key& current_key = keys[i];
            if (current_key.is_pressed()) {
                is_any_key_pressed = true;
                if (!current_key.status) {
                    key_state = i;
                    current_key.status = true;
                }
            } else {
                current_key.status = false;
            }
        }
        if (!is_any_key_pressed) {
            key_state = -1;
            window.draw(default_sprite);
        }
        if (key_state > -1) {
            key& on_key = keys[key_state];
            double last_press = -1;
            for (int i = 0; i < keys.size(); i++) {
                if (i != key_state) {
                    last_press = std::max(last_press, keys[i].timer);
                }
            }
            if ((clock() - last_press) / CLOCKS_PER_SEC > BONGO_KEYPRESS_THRESHOLD) {
                on_key.draw();
            } else {
                window.draw(default_sprite);
            }
        }
    }
};

std::vector<key_container> key_containers;
sf::Sprite bg;

bool init() {
    // getting configs
    try {
        Json::Value custom = data::cfg["custom"];
        key_containers.clear();
        for (Json::Value& current_key_container : custom["keyContainers"]) {
            key_containers.push_back(key_container(current_key_container));
        }
        if (!custom.isMember("background") || !custom["background"].isString()) {
            data::error_msg("Custom background not found", "Error reading config");
            return false;
        }
        bg.setTexture(data::load_texture(custom["background"].asString()));
    } catch (...) {
        return false;
    }
    return true;
}

void draw() {
    window.draw(bg);

    for (key_container& current : key_containers) {
        current.draw();
    }
}
}; // namespace taiko
