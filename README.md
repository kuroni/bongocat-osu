# Description

This project is a fork of the [osu! Bongo Cat overlay](https://github.com/kuroni/bongocat-osu).

## Original description
An osu! Bongo Cat overlay with smooth paw movement and simple skinning ability, written in C++. Originally created by [HamishDuncanson](https://github.com/HamishDuncanson).

You can find how to configure the application in our [wiki](https://github.com/kuroni/bongocat-osu/wiki/Settings).

Download the program [here](https://github.com/kuroni/bongocat-osu/releases).

Hugs and kisses to [CSaratakij](https://github.com/CSaratakij) for creating the Linux port for this project!

Any suggestion and/or collaboration, especially that relating to sprites, is welcomed! Thank you!

[Original post](https://www.reddit.com/r/osugame/comments/9hrkte/i_know_bongo_cat_is_getting_old_but_heres_a_nicer/) by [Kuvster](https://github.com/Kuvster).

## License

The project is licensed under the GNU GPL v3 license (see [LICENSE](LICENSE)), however it contains parts from the original project licensed under
a MIT license. A copy of the full text of the original copyright and license is included in [LICENSE.ORIG.MIT](LICENSE.ORIG.MIT).

## Further information
In order to play with fullscreen on Windows 10, run both osu! and this application in Windows 7 compability mode.

Press Ctrl + R to reload configuration and images (will only reload configurations when the window is focused).

Supported operating system:
* Windows
* Linux (tested with Arch Linux with WINE Staging 5). Note: You **must** use WINE Staging, because for whatever reason on stable WINE bongocat-osu doesn't register keyboard input from other windows.

_Notice_: If you're using WINE on Linux, make sure that osu! and this application run in the same `WINEPREFIX`.

## For developers
This project uses [SFML](https://www.sfml-dev.org/index.php) and [JsonCpp](https://github.com/open-source-parsers/jsoncpp). JsonCpp libraries are directly included in the source using the provided `amalgamation.py` from the developers.

### Libraries and dependency

#### Windows and MinGW
To build the source, download the SFML libraries [here](https://www.sfml-dev.org/index.php), copy `Makefile.windows` to `Makefile`, then replace *`<SFML-folder>`* in `Makefile` with the desired folder.

#### Linux
You need to have these dependencies installed. Check with your package manager for the exact name of these dependencies on your distro:
- g++
- libxdo
- sdl2
- sfml
- x11
- xrandr

Then, copy `Makefile.linux` to `Makefile`.

### Building and testing
To build, run this command from the base directory:

```sh
make
```

To test the program, run this from the base directory:

```sh
make test
```

Alternatively, you can copy the newly-compiled `bin/bongo.exe` or `bin/bongo` into the base directory and execute it.

If you have troubles compiling, it can be due to version mismatch between your compiler and SFML. See [#43](https://github.com/kuroni/bongocat-osu/issues/43) for more information.

