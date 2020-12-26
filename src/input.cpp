#include "header.hpp"
#include <sstream>
#include <cmath>
#include <iomanip>
#include <SFML/Window.hpp>

#if defined(__unix__) || defined(__unix)
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <X11/keysym.h>

extern "C" {
#include <xdo.h>
}
#else
#include <windows.h>
#endif

#define TOTAl_INPUT_TABLE_SIZE 256
#define JOYSTICK_AXIS_DEADZONE 10.0f
#define JOYSTICK_TRIGGER_DEADZONE 3.0f

namespace input {
int horizontal, vertical;
int osu_x, osu_y, osu_h, osu_v;
bool is_letterbox, is_left_handed;

int last_joystick_keycode = -1;

std::string debugMessage;
std::string debugBitMessage;

sf::RectangleShape debugBackground;
sf::Font debugFont;
sf::Text debugText;


enum JoystickInputMapRange {
    MinButton =     0,
    MaxButton =     31,
    LS_Left,
    LS_Right,
    LS_Up,
    LS_Down,
    RS_Left,
    RS_Right,
    RS_Up,
    RS_Down,
    DPad_Left,
    DPad_Right,
    DPad_Up,
    DPad_Down,
    LTrigger,
    RTrigger
};

#if defined(__unix__) || defined(__unix)
xdo_t* xdo;
Display* dpy;
Window foreground_window;

static int _XlibErrorHandler(Display *display, XErrorEvent *event) {
    return true;
}
#endif

int INPUT_KEY_TABLE[TOTAl_INPUT_TABLE_SIZE];

bool init() {
    for (int i = 0; i < TOTAl_INPUT_TABLE_SIZE; i++) {
        if (i >= 48 && i <= 57) {           // number
            INPUT_KEY_TABLE[i] = i - 48 + (int)sf::Keyboard::Key::Num0;
        } else if (i >= 65 && i <= 90) {    // english alphabet
            INPUT_KEY_TABLE[i] = i - 65 + (int)sf::Keyboard::Key::A;
        } else if (i >= 96 && i <= 105) {   // numpad
            INPUT_KEY_TABLE[i] = i - 96 + (int)sf::Keyboard::Key::Numpad0;
        } else if (i >= 112 && i <= 126) {  // function
            INPUT_KEY_TABLE[i] = i - 112 + (int)sf::Keyboard::Key::F1;
        } else {
            INPUT_KEY_TABLE[i] = (int)sf::Keyboard::Key::Unknown;
        }
    }

    INPUT_KEY_TABLE[27] = (int)sf::Keyboard::Key::Escape;
    INPUT_KEY_TABLE[17] = (int)sf::Keyboard::Key::LControl;
    INPUT_KEY_TABLE[16] = (int)sf::Keyboard::Key::LShift;
    INPUT_KEY_TABLE[18] = (int)sf::Keyboard::Key::LAlt;
    INPUT_KEY_TABLE[17] = (int)sf::Keyboard::Key::RControl;
    INPUT_KEY_TABLE[16] = (int)sf::Keyboard::Key::RShift;
    INPUT_KEY_TABLE[18] = (int)sf::Keyboard::Key::RAlt;
    INPUT_KEY_TABLE[93] = (int)sf::Keyboard::Key::Menu;
    INPUT_KEY_TABLE[219] = (int)sf::Keyboard::Key::LBracket;
    INPUT_KEY_TABLE[221] = (int)sf::Keyboard::Key::RBracket;
    INPUT_KEY_TABLE[186] = (int)sf::Keyboard::Key::Semicolon;
    INPUT_KEY_TABLE[188] = (int)sf::Keyboard::Key::Comma;
    INPUT_KEY_TABLE[190] = (int)sf::Keyboard::Key::Period;
    INPUT_KEY_TABLE[222] = (int)sf::Keyboard::Key::Quote;
    INPUT_KEY_TABLE[191] = (int)sf::Keyboard::Key::Slash;
    INPUT_KEY_TABLE[220] = (int)sf::Keyboard::Key::Backslash;
    INPUT_KEY_TABLE[192] = (int)sf::Keyboard::Key::Tilde;
    INPUT_KEY_TABLE[187] = (int)sf::Keyboard::Key::Equal;
    INPUT_KEY_TABLE[189] = (int)sf::Keyboard::Key::Hyphen;
    INPUT_KEY_TABLE[32] = (int)sf::Keyboard::Key::Space;
    INPUT_KEY_TABLE[13] = (int)sf::Keyboard::Key::Enter;
    INPUT_KEY_TABLE[8] = (int)sf::Keyboard::Key::Backspace;
    INPUT_KEY_TABLE[9] = (int)sf::Keyboard::Key::Tab;
    INPUT_KEY_TABLE[33] = (int)sf::Keyboard::Key::PageUp;
    INPUT_KEY_TABLE[34] = (int)sf::Keyboard::Key::PageDown;
    INPUT_KEY_TABLE[35] = (int)sf::Keyboard::Key::End;
    INPUT_KEY_TABLE[36] = (int)sf::Keyboard::Key::Home;
    INPUT_KEY_TABLE[45] = (int)sf::Keyboard::Key::Insert;
    INPUT_KEY_TABLE[46] = (int)sf::Keyboard::Key::Delete;
    INPUT_KEY_TABLE[107] = (int)sf::Keyboard::Key::Add;
    INPUT_KEY_TABLE[109] = (int)sf::Keyboard::Key::Subtract;
    INPUT_KEY_TABLE[106] = (int)sf::Keyboard::Key::Multiply;
    INPUT_KEY_TABLE[111] = (int)sf::Keyboard::Key::Divide;
    INPUT_KEY_TABLE[37] = (int)sf::Keyboard::Key::Left;
    INPUT_KEY_TABLE[39] = (int)sf::Keyboard::Key::Right;
    INPUT_KEY_TABLE[38] = (int)sf::Keyboard::Key::Up;
    INPUT_KEY_TABLE[40] = (int)sf::Keyboard::Key::Down;
    INPUT_KEY_TABLE[19] = (int)sf::Keyboard::Key::Pause;
    INPUT_KEY_TABLE[189] = (int)sf::Keyboard::Key::Dash;

    is_letterbox = data::cfg["resolution"]["letterboxing"].asBool();
    osu_x = data::cfg["resolution"]["width"].asInt();
    osu_y = data::cfg["resolution"]["height"].asInt();
    osu_h = data::cfg["resolution"]["horizontalPosition"].asInt();
    osu_v = data::cfg["resolution"]["verticalPosition"].asInt();
    is_left_handed = data::cfg["decoration"]["leftHanded"].asBool();

#if defined(__unix__) || defined(__unix)
    // Set x11 error handler
    XSetErrorHandler(_XlibErrorHandler);

    // Get desktop resolution
    int num_sizes;
    Rotation current_rotation;

    dpy = XOpenDisplay(NULL);
    Window root = RootWindow(dpy, 0);
    XRRScreenSize *xrrs = XRRSizes(dpy, 0, &num_sizes);

    XRRScreenConfiguration *conf = XRRGetScreenInfo(dpy, root);
    SizeID current_size_id = XRRConfigCurrentConfiguration(conf, &current_rotation);

    int current_width = xrrs[current_size_id].width;
    int current_height = xrrs[current_size_id].height;

    horizontal = current_width;
    vertical = current_height;

    xdo = xdo_new(NULL);
#else
    // getting resolution
    RECT desktop;
    const HWND h_desktop = GetDesktopWindow();
    GetWindowRect(h_desktop, &desktop);
    horizontal = desktop.right;
    vertical = desktop.bottom;
#endif

    // loading font
    if (!debugFont.loadFromFile("font/RobotoMono-Bold.ttf")) {
        data::error_msg("Cannot find the font : RobotoMono-Bold.ttf", "Error loading font");
        return false;
    }

    // initialize debug resource
    debugBackground.setSize(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    debugBackground.setFillColor(sf::Color(0, 0, 0, 128));

    debugText.setFont(debugFont);
    debugText.setCharacterSize(14);
    debugText.setFillColor(sf::Color::White);
    debugText.setPosition(10.0f, 4.0f);
    debugText.setString(debugMessage);

    return true;
}

sf::Keyboard::Key ascii_to_key(int key_code) {
    if (key_code < 0 || key_code >= TOTAl_INPUT_TABLE_SIZE) {
        // out of range
        return sf::Keyboard::Unknown;
    } else {
        return (sf::Keyboard::Key)(INPUT_KEY_TABLE[key_code]);
    }
}

// for some special cases of num dot and such
bool is_pressed_fallback(int key_code) {
#if defined(__unix__) || defined(__unix) // code snippet from SFML
    KeyCode keycode = XKeysymToKeycode(dpy, key_code);
    if (keycode != 0) {
        char keys[32];
        XQueryKeymap(dpy, keys);
        return (keys[keycode / 8] & (1 << (keycode % 8))) != 0;
    }
    else {
        return false;
    }
#else
    return (GetAsyncKeyState(key_code) & 0x8000) != 0;
#endif
}

bool is_pressed(int key_code) {
    if (key_code == 16) {
        return sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)
            || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
    } else if (key_code == 17) {
        return sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)
            || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
    } else {
        sf::Keyboard::Key selected = ascii_to_key(key_code);
        if (selected != sf::Keyboard::Key::Unknown) {
            return sf::Keyboard::isKeyPressed(selected);
        } else {
            return is_pressed_fallback(key_code);
        }
    }
}

bool is_joystick_connected() {
    return sf::Joystick::isConnected(0);
}

bool is_joystick_pressed(int key_code) {
    int id = 0;
    last_joystick_keycode = key_code;

    // joystick button, range 0 - 31
    if (key_code >= MinButton && key_code <= MaxButton) {
        return sf::Joystick::isButtonPressed(id, key_code);
    }
    // joystick axis, range 32 - 45
    else {
        float axis = 0.0f;

        switch (key_code) {
            case LS_Left:
                axis = sf::Joystick::getAxisPosition(id, sf::Joystick::X);
                return axis <= -JOYSTICK_AXIS_DEADZONE;
            break;

            case LS_Right:
                axis = sf::Joystick::getAxisPosition(id, sf::Joystick::X);
                return axis >= JOYSTICK_AXIS_DEADZONE;
            break;

            case LS_Up:
                axis = sf::Joystick::getAxisPosition(id, sf::Joystick::Y);
                return axis <= -JOYSTICK_AXIS_DEADZONE;
            break;

            case LS_Down:
                axis = sf::Joystick::getAxisPosition(id, sf::Joystick::Y);
                return axis >= JOYSTICK_AXIS_DEADZONE;
            break;

            case RS_Left:
                axis = sf::Joystick::getAxisPosition(id, sf::Joystick::U);
                return axis <= -JOYSTICK_AXIS_DEADZONE;
            break;

            case RS_Right:
                axis = sf::Joystick::getAxisPosition(id, sf::Joystick::U);
                return axis >= JOYSTICK_AXIS_DEADZONE;
            break;

            case RS_Up:
                axis = sf::Joystick::getAxisPosition(id, sf::Joystick::V);
                return axis <= -JOYSTICK_AXIS_DEADZONE;
            break;

            case RS_Down:
                axis = sf::Joystick::getAxisPosition(id, sf::Joystick::V);
                return axis >= JOYSTICK_AXIS_DEADZONE;
            break;

            case DPad_Left:
                axis = sf::Joystick::getAxisPosition(id, sf::Joystick::PovX);
                return axis <= -JOYSTICK_AXIS_DEADZONE;
            break;

            case DPad_Right:
                axis = sf::Joystick::getAxisPosition(id, sf::Joystick::PovX);
                return axis >= JOYSTICK_AXIS_DEADZONE;
            break;

            case DPad_Up:
                axis = sf::Joystick::getAxisPosition(id, sf::Joystick::PovY);
                return axis <= -JOYSTICK_AXIS_DEADZONE;
            break;

            case DPad_Down:
                axis = sf::Joystick::getAxisPosition(id, sf::Joystick::PovY);
                return axis >= JOYSTICK_AXIS_DEADZONE;
            break;

            case LTrigger:
                axis = sf::Joystick::getAxisPosition(id, sf::Joystick::Z);
                return axis >= JOYSTICK_TRIGGER_DEADZONE;
            break;

            case RTrigger:
                axis = sf::Joystick::getAxisPosition(id, sf::Joystick::R);
                return axis >= JOYSTICK_TRIGGER_DEADZONE;
            break;

            default:
                return false;
            break;
        }
    }

    return false;
}

// bezier curve for osu and custom
std::pair<double, double> bezier(double ratio, std::vector<double> &points, int length) {
    double fact[22] = {0.001, 0.001, 0.002, 0.006, 0.024, 0.12, 0.72, 5.04, 40.32, 362.88, 3628.8, 39916.8, 479001.6, 6227020.8, 87178291.2, 1307674368.0, 20922789888.0, 355687428096.0, 6402373705728.0, 121645100408832.0, 2432902008176640.0, 51090942171709440.0};
    int nn = (length / 2) - 1;
    double xx = 0;
    double yy = 0;

    for (int point = 0; point <= nn; point++) {
        double tmp = fact[nn] / (fact[point] * fact[nn - point]) * pow(ratio, point) * pow(1 - ratio, nn - point);
        xx += points[2 * point] * tmp;
        yy += points[2 * point + 1] * tmp;
    }

    return std::make_pair(xx / 1000, yy / 1000);
}

std::pair<double, double> get_xy() {
#if defined(__unix__) || defined(__unix)
    double letter_x, letter_y, s_height, s_width;
    bool found_window = (xdo_get_focused_window_sane(xdo, &foreground_window) == 0);

    if (found_window) {
        unsigned char* name_ret;
        int name_len_ret;
        int name_type;

        xdo_get_window_name(xdo, foreground_window, &name_ret, &name_len_ret, &name_type);
        bool can_get_name = (name_len_ret > 0);

        if (can_get_name) {

            std::string title = "";

            if (name_ret != NULL)
            {
                std::string foreground_title(reinterpret_cast<char*>(name_ret));
                title = foreground_title;
            }

            if (title.find("osu!") == 0) {
                if (!is_letterbox) {

                    int x_ret;
                    int y_ret;
                    unsigned int width_ret;
                    unsigned int height_ret;

                    bool can_get_location = (xdo_get_window_location(xdo, foreground_window, &x_ret, &y_ret, NULL) == 0);
                    bool can_get_size = (xdo_get_window_size(xdo, foreground_window, &width_ret, &height_ret) == 0);

                    bool can_get_rect = (can_get_location && can_get_size);

                    bool is_fullscreen_window = (horizontal == width_ret) && (vertical == height_ret);
                    bool should_not_resize_screen = (!can_get_rect || is_fullscreen_window);

                    if (should_not_resize_screen) {
                        s_width = horizontal;
                        s_height = vertical;

                        letter_x = 0;
                        letter_y = 0;
                    }
                    else {
                        s_height = osu_y * 0.8;
                        s_width = s_height * 4 / 3;

                        long left = x_ret;
                        long top = y_ret;
                        long right = left + width_ret;
                        long bottom = top + height_ret;

                        letter_x = left + ((right - left) - s_width) / 2;
                        letter_y = top + osu_y * 0.117;
                    }
                }
                else {
                    s_height = osu_y * 0.8;
                    s_width = s_height * 4 / 3;

                    double l = (horizontal - osu_x) * (osu_h + 100) / 200.0;
                    double r = l + osu_x;
                    letter_x = l + ((r - l) - s_width) / 2;
                    letter_y = (vertical - osu_y) * (osu_v + 100) / 200.0 + osu_y * 0.117;
                }
            }
            else {
                s_width = horizontal;
                s_height = vertical;
                letter_x = 0;
                letter_y = 0;
            }
        }
        else {
            s_width = horizontal;
            s_height = vertical;
            letter_x = 0;
            letter_y = 0;
        }
    }
    else {
        s_width = horizontal;
        s_height = vertical;
        letter_x = 0;
        letter_y = 0;
    }

    double x = 0, y = 0;
    int px = 0, py = 0;

    if (xdo_get_mouse_location(xdo, &px, &py, NULL) == 0) {

        if (!is_letterbox) {
            letter_x = floor(1.0 * px / osu_x) * osu_x;
            letter_y = floor(1.0 * py / osu_y) * osu_y;
        }

        double fx = (1.0 * px - letter_x) / s_width;

        if (is_left_handed) {
            fx = 1 - fx;
        }

        double fy = (1.0 * py - letter_y) / s_height;

        fx = std::min(fx, 1.0);
        fx = std::max(fx, 0.0);

        fy = std::min(fy, 1.0);
        fy = std::max(fy, 0.0);

        x = -97 * fx + 44 * fy + 184;
        y = -76 * fx - 40 * fy + 324;
    }
#else
    // getting device resolution
    double letter_x, letter_y, s_height, s_width;

    HWND handle = GetForegroundWindow();
    if (handle) {
        TCHAR w_title[256];
        GetWindowText(handle, w_title, GetWindowTextLength(handle));
        std::string title = w_title;
        if (title.find("osu!") == 0) {
            RECT oblong;
            GetWindowRect(handle, &oblong);
            s_height = osu_y * 0.8;
            s_width = s_height * 4 / 3;
            if (!is_letterbox) {
                letter_x = oblong.left + ((oblong.right - oblong.left) - s_width) / 2;
                letter_y = oblong.top + osu_y * 0.117;
            } else {
                double l = (horizontal - osu_x) * (osu_h + 100) / 200.0;
                double r = l + osu_x;
                letter_x = l + ((r - l) - s_width) / 2;
                letter_y = (vertical - osu_y) * (osu_v + 100) / 200.0 + osu_y * 0.117;
            }
        } else {
            s_width = horizontal;
            s_height = vertical;
            letter_x = 0;
            letter_y = 0;
        }
    } else {
        s_width = horizontal;
        s_height = vertical;
        letter_x = 0;
        letter_y = 0;
    }
    double x, y;
    POINT point;
    if (GetCursorPos(&point)) {
        if (!is_letterbox) {
            letter_x = floor(1.0 * point.x / osu_x) * osu_x;
            letter_y = floor(1.0 * point.y / osu_y) * osu_y;
        }
        double fx = (1.0 * point.x - letter_x) / s_width;
        if (is_left_handed) {
            fx = 1 - fx;
        }
        double fy = (1.0 * point.y - letter_y) / s_height;
        fx = std::min(fx, 1.0);
        fx = std::max(fx, 0.0);
        fy = std::min(fy, 1.0);
        fy = std::max(fy, 0.0);
        x = -97 * fx + 44 * fy + 184;
        y = -76 * fx - 40 * fy + 324;
    }
#endif

    return std::make_pair(x, y);
}

void drawDebugPanel() {
    if (!is_joystick_connected()) {
        debugText.setString("No joystick found...");
        window.draw(debugBackground);
        window.draw(debugText);
        return;
    }

    int joy_id = 0;

    std::stringstream result;
    sf::Joystick::Identification info = sf::Joystick::getIdentification(joy_id);

    result << "Joystick connected : " << info.name.toAnsiString() << std::endl;
    result << "Support button : " << sf::Joystick::getButtonCount(joy_id) << std::endl;

    int offset = 0;
    int counter = 0;
    int max_button = (int)sf::Joystick::ButtonCount;
    int max_row = 11;

    for (int i = 0; i < max_button; ++i) {
        int buttonID = (counter + offset);

        bool isPressed = sf::Joystick::isButtonPressed(joy_id, buttonID);
        std::string state = isPressed ? "PRESS" : "release";

        result << std::setw(10) << "Button#" << std::setw(2) << buttonID << std::setw(1) << ": " << std::setw(5) << state;

        counter += max_row;
        bool shouldPrintNextLine = ((i + 1) % 3) == 0;

        if (shouldPrintNextLine) {
            result << std::endl;
            counter = 0;
            offset += 1;
        }
    }

    result << std::endl;
    result << "Axis : " << std::endl;

    sf::Vector2f leftstick_axis;
    sf::Vector2f rightstick_axis;
    sf::Vector2f dpad_axis;
    float left_trigger_axis;
    float right_trigger_axis;

    leftstick_axis.x = sf::Joystick::getAxisPosition(joy_id, sf::Joystick::X);
    leftstick_axis.y = sf::Joystick::getAxisPosition(joy_id, sf::Joystick::Y);
    rightstick_axis.x = sf::Joystick::getAxisPosition(joy_id, sf::Joystick::U);
    rightstick_axis.y = sf::Joystick::getAxisPosition(joy_id, sf::Joystick::V);
    
    dpad_axis.x = sf::Joystick::getAxisPosition(joy_id, sf::Joystick::PovX);
    dpad_axis.y = sf::Joystick::getAxisPosition(joy_id, sf::Joystick::PovY);

    left_trigger_axis = sf::Joystick::getAxisPosition(joy_id, sf::Joystick::Z); 
    right_trigger_axis = sf::Joystick::getAxisPosition(joy_id, sf::Joystick::R); 

    result << "LStick : " << "( " << leftstick_axis.x << "," << leftstick_axis.y << " )" << std::endl;
    result << "RStick : " << "( " << rightstick_axis.x << "," << rightstick_axis.y << " )" << std::endl;
    result << "LTrigger : " << left_trigger_axis << std::endl;
    result << "RTrigger : " << right_trigger_axis << std::endl;
    result << "DPad : " << "( " << dpad_axis.x << "," << dpad_axis.y << " )" << std::endl;

    debugText.setString(result.str());

    window.draw(debugBackground);
    window.draw(debugText);
}

void cleanup() {
#if defined(__unix__) || defined(__unix)
    delete xdo;
    XCloseDisplay(dpy);
#endif
}

};

