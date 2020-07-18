#include "header.hpp"
#define BONGO_ERROR 1

namespace data {
Json::Value cfg;
std::map<std::string, sf::Texture> img_holder;

void create_config() {
    const char *s =
        R"V0G0N({
    "mode": 1,
    "resolution": {
        "letterboxing": false,
        "width": 1920,
        "height": 1080,
        "horizontalPosition": 0,
        "verticalPosition": 0
    },
    "decoration": {
        "leftHanded": false,
        "rgb": [255, 255, 255],
        "offsetX": [0, 11],
        "offsetY": [0, -65],
        "scalar": [1.0, 1.0]
    },
    "osu": {
        "mouse": true,
        "paw": [255, 255, 255],
        "pawEdge": [0, 0, 0],
        "key1": [90],
        "key2": [88],
        "smoke": [67],
        "wave": []
    },
    "taiko": {
        "leftCentre": [88],
        "rightCentre": [67],
        "leftRim": [90],
        "rightRim": [86]
    },
    "catch": {
        "left": [37],
        "right": [39],
        "dash": [16]
    },
    "mania": {
        "4K": true,
        "key4K": [68, 70, 74, 75],
        "key7K": [83, 68, 70, 32, 74, 75, 76]
    },
    "custom": {
        "keyContainers": []
    }
})V0G0N";
    std::string error;
    Json::CharReaderBuilder cfg_builder;
    Json::CharReader *cfg_reader = cfg_builder.newCharReader();
    cfg_reader->parse(s, s + strlen(s), &cfg, &error);
    delete cfg_reader;
}

void error_msg(std::string error, std::string title) {
    if (MessageBoxA(NULL, error.c_str(), title.c_str(), MB_ICONERROR | MB_RETRYCANCEL) == IDCANCEL) {
        exit(BONGO_ERROR);
    }
}

bool update(Json::Value &cfg_default, Json::Value &cfg) {
    bool is_update = true;
    for (const auto &key : cfg.getMemberNames()) {
        if (cfg_default.isMember(key)) {
            if (cfg_default[key].type() != cfg[key].type()) {
                error_msg("Value type error in config.json", "Error reading configs");
                return false;
            }
            if (cfg_default[key].isArray() && !cfg_default[key].empty()) {
                for (Json::Value &v : cfg[key]) {
                    if (v.type() != cfg_default[key][0].type()) {
                        error_msg("Value type in array error in config.json", "Error reading configs");
                        return false;
                    }
                }
            }
            if (cfg_default[key].isObject()) {
                is_update &= update(cfg_default[key], cfg[key]);
            } else {
                cfg_default[key] = cfg[key];
            }
        } else {
            cfg_default[key] = cfg[key];
        }
    }
    return is_update;
}

bool init() {
    while (true) {
        create_config();
        std::ifstream cfg_file("config.json", std::ifstream::binary);
        if (!cfg_file.good()) {
            break;
        }
        std::string cfg_string((std::istreambuf_iterator<char>(cfg_file)), std::istreambuf_iterator<char>()), error;
        Json::CharReaderBuilder cfg_builder;
        Json::CharReader *cfg_reader = cfg_builder.newCharReader();
        Json::Value cfg_read;
        if (!cfg_reader->parse(cfg_string.c_str(), cfg_string.c_str() + cfg_string.size(), &cfg_read, &error)) {
            delete cfg_reader;
            error_msg("Syntax error in config.json:\n" + error, "Error reading configs");
        } else if (update(cfg, cfg_read)) {
            delete cfg_reader;
            break;
        }
    }

    img_holder.clear();

    int mode = data::cfg["mode"].asInt();

    switch (mode) {
    case 1:
        return osu::init();
    case 2:
        return taiko::init();
    case 3:
        return ctb::init();
    case 4:
        return mania::init();
    case 5:
        return custom::init();
    default:
        error_msg("Mode value is not correct", "Error reading configs");
        return false;
    }
}

sf::Texture &load_texture(std::string path) {
    if (img_holder.find(path) == img_holder.end()) {
        while (!img_holder[path].loadFromFile(path)) {
            error_msg("Cannot find file " + path, "Error importing images");
        }
    }
    return img_holder[path];
}
}; // namespace data
