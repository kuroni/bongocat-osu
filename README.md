# Description
An osu! Bongo Cat overlay with smooth paw movement and simple skinning ability, written in C++.

You can find how to configure the application in our [wiki](https://github.com/kuroni/bongocat-osu/wiki/Settings).

Releases can be found [here](https://github.com/kuroni/bongocat-osu/releases).

Any suggestion and/or collaboration, especially that relating to sprites, is welcomed! Thank you!

[Original post](https://www.reddit.com/r/osugame/comments/9hrkte/i_know_bongo_cat_is_getting_old_but_heres_a_nicer/) by [u/Kuvster](https://github.com/Kuvster).

## Further information
In order to play with fullscreen on Windows 10, run both osu! and this application in Windows 7 compability mode.

Press Ctrl + R to reload configuration and images (will only reload configurations when the window is focused).

Supported operating system:
* Windows
* Linux (tested with Arch Linux 4.18.9 with WINE 3.16)

_Notice_: If you're using WINE on Linux, make sure that osu! and this application run in the same `WINEPREFIX`.

## Todo
* Support other modes.
* Optimize further for consistency and speed.
* Allow even more customizations.

## For developers
This project uses [SFML](https://www.sfml-dev.org/index.php) and [JsonCpp](https://github.com/open-source-parsers/jsoncpp). JsonCpp libraries are directly included in the source using the provided `amalgamation.py` from the developers.

To build the source, download the SFML libraries, replace *`<SFML-folder>`* in `Makefile` with the desired folder, and run these commands from the base directory:

```
mkdir bin
make
```

To test the program run this from the base directory:

```
make test
```

Alternatively, you can copy the newly-compiled `bin/bongocat.exe` into the base directory and execute it.
