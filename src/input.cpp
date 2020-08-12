#include "header.hpp"
#include <SFML/Window.hpp>

#define TOTAl_INPUT_TABLE_SIZE 128

namespace input {

int INPUT_KEY_TABLE[TOTAl_INPUT_TABLE_SIZE];

void init() {
    for (int i = 0; i < TOTAl_INPUT_TABLE_SIZE; ++i) {

        bool is_number = (i >= 48 && i <= 57);
        bool is_uppercase_eng_alphabet = (i >= 65 && i <= 90);
        bool is_lowercase_eng_alphabet = (i >= 97 && i <= 122);

        if (is_number) {
            INPUT_KEY_TABLE[i] = (i - 22);
        }
        else if (is_uppercase_eng_alphabet) {
            INPUT_KEY_TABLE[i] = (i - 65);
        }
        else if (is_lowercase_eng_alphabet) {
            INPUT_KEY_TABLE[i] = (i - 97);
        }
        else {
            INPUT_KEY_TABLE[i] = -1;
        }
    }

    INPUT_KEY_TABLE[8] = (int)(sf::Keyboard::Key::Backspace);
    INPUT_KEY_TABLE[9] = (int)(sf::Keyboard::Key::Tab);
    INPUT_KEY_TABLE[11] = (int)(sf::Keyboard::Key::Tab);
    INPUT_KEY_TABLE[13] = (int)(sf::Keyboard::Key::Enter);
    INPUT_KEY_TABLE[16] = (int)(sf::Keyboard::Key::LShift);
    INPUT_KEY_TABLE[27] = (int)(sf::Keyboard::Key::Escape);
    INPUT_KEY_TABLE[32] = (int)(sf::Keyboard::Key::Space);
    INPUT_KEY_TABLE[34] = (int)(sf::Keyboard::Key::Quote);
    INPUT_KEY_TABLE[37] = (int)(sf::Keyboard::Key::Left);
    INPUT_KEY_TABLE[39] = (int)(sf::Keyboard::Key::Right);
    INPUT_KEY_TABLE[44] = (int)(sf::Keyboard::Key::Comma);
    INPUT_KEY_TABLE[45] = (int)(sf::Keyboard::Key::Hyphen);
    INPUT_KEY_TABLE[46] = (int)(sf::Keyboard::Key::Period);
    INPUT_KEY_TABLE[59] = (int)(sf::Keyboard::Key::Semicolon);
    INPUT_KEY_TABLE[61] = (int)(sf::Keyboard::Key::Equal);
    INPUT_KEY_TABLE[91] = (int)(sf::Keyboard::Key::LBracket);
    INPUT_KEY_TABLE[92] = (int)(sf::Keyboard::Key::Backslash);
    INPUT_KEY_TABLE[93] = (int)(sf::Keyboard::Key::RBracket);
    INPUT_KEY_TABLE[126] = (int)(sf::Keyboard::Key::Tilde);
}

sf::Keyboard::Key ascii_to_key(int ascii_key) {
    bool is_out_of_range = (ascii_key < 0) || (ascii_key > (TOTAl_INPUT_TABLE_SIZE - 1));

    if (is_out_of_range) {
        return sf::Keyboard::Unknown;
    }

    return (sf::Keyboard::Key)(INPUT_KEY_TABLE[ascii_key]);
}

bool is_pressed(int ascii_key) {
    sf::Keyboard::Key key = ascii_to_key(ascii_key);
    return is_pressed(key);
}

bool is_pressed(sf::Keyboard::Key key) {
    return sf::Keyboard::isKeyPressed(key);
}

};

