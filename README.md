# Description
An osu! Bongo Cat overlay with smooth paw movement and simple skinning ability, written in C++.

You can find how to configure the application in our [wiki](https://github.com/kuroni/bongocat-osu/wiki/Configuration).

Releases can be found [here](https://github.com/kuroni/bongocat-osu/releases).

Any suggestion and/or collaboration is welcomed! Thank you!

[Original post](https://www.reddit.com/r/osugame/comments/9hrkte/i_know_bongo_cat_is_getting_old_but_heres_a_nicer/) by [u/Kuvster](https://github.com/Kuvster).

## Further information
In order to play with fullscreen on Windows 10, run both osu! and this application in Windows 7 compability mode.

Press ESC to reload configuration and images (ESC will only reload configurations when the window is focused).

Supported operating system:
* Windows
* Linux (tested with Arch Linux 4.18.9 with Wine 3.16)

_Notice_: If you're using Wine on Linux, make sure that osu! and this application run in the same `WINEPREFIX`.

## Todo
* Support other modes.
* Optimize further for consistency and speed.
* Allow even more customizations.

## For developers
This project uses [SFML](https://www.sfml-dev.org/index.php) and [JsonCpp](https://github.com/open-source-parsers/jsoncpp). JsonCpp libraries are directly included in the source.

Use these following flags to compile a standalone executable:

`-DSFML_STATIC -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lfreetype -lwinmm -lgdi32 -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic`
