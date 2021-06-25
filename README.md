# Bongo Swap
This is a fork of the original Bongo Cat overlay that adds a feature for custom config allowing you to switch between drawing the mouse or drawing an additional hand when the mouse is left stationary for a customizable amount of time.
This would be useful for people who switch between games that use mouse & keyboard and games that use keyboard only, or just typing. Basically, if you just want a tiny cat that more accurately mimics anything at all you do with your mouse and keyboard for your entire stream, instead of just some inputs chosen for a single game, this will allow you to do that without needing to switch between different configs.

The provided config file has an example custom config in it that maps the main area of a keyboard and swaps between drawing the right hand or drawing the mouse. If the mouse is actively moving, then only keys on the left side of the keyboard will trigger animations. Once the mouse is left still for a couple seconds, the right hand idle animation will replace the mouse animation, and keys on the right side of the keyboard will trigger animations until the mouse is moved again.

### Additional custom config options:
*mousePause: Positive integer defined before keyContainers in the custom field. Defines the number of draw iterations for which the mouse must stay stationary in order to stop drawing its arm. If not provided, the mouse is always drawn. I found 100 to be a reasonable order of magnitude for me, giving a pause of about 3 seconds, but fine tune according to your needs.
*withMouse: Boolean, defined within each key container (**not** the keys structure though). "true" means that the keys will be animated while the mouse is moving. "false" means that keys will not be animated as long as the mouse is moving, but will be animated once it pauses long enough.

## Full list of custom config options:
Unless I missed something, the original Bongo Cat page didn't have a full list of every option available for custom configurations, so here it is:

custom options (defined before "keyContainers"):
*mouse: Boolean, defines whether the mouse is ever drawn in this configuration or not.
*mouseOnTop: Boolean, defines whether to draw the mouse on top of or underneath the paw.
*offsetX: Int, how many pixels to shift the mouse image right with respect to its normal point at the end of the paw.
*offsetY: Int, how many pixels to shift the mouse image down with respect to its normal point at the end of the paw.
*paw: Int array [r, g, b], paw fill color as an rbg triple.
*pawEdge: Int array [r, g, b], paw edge color as an rbg triple.
*mouseImage: String, path to custom mouse image.
*keyContainers: Array of `key_container` structures. Each `key_container` defines a default idle image and withMouse setting to be used for a set of `keys` structures.

key container options (defined inside each key container, before "keys"):
*defaultImage: String, path to custom image to show when none of the contained keys are being pressed.
*mousePause: Positive integer, defines the number of draw iterations for which the mouse must stay stationary in order to stop drawing its arm.
*keys: Array of `key` structures. Each `key` structure defines an image to be shown whenever any one of a set of keys is pressed. So, a `key_container` allows one idle image to be used for many keys that use different pressed animations.

key options (defined inside each key object):
*image: String, path to custom image to be shown when any one of the keys in the keyCodes array is pressed.
*keyCodes: Int array, list of [key codes](https://keycode.info/) that will cause `image` to be shown when they are pressed.
*joyCodes: Int array, list of joystick or controller button codes that will cause `image` to be shown when they are pressed.


## Troubleshooting
I did not run into any issues with my setup. I would expect mouse jiggle to be a potential annoyance for some people - depending on your mouse sensitivity, sensor, and mousepad surface/desk, your mouse might send inputs even after being left alone, due to vibrations from typing or just extreme sensitivity.

If you use AutoHotKey to remap buttons on your keyboard to other buttons (like Caps Lock to Control), the mapped key will trigger its animation, not the physical key pressed. For example, if I were to for some reason map Tab to Enter, when I hit the Tab button, the right hand would tap.

The provided custom config maps Control, Shift, Alt, and Space to the left hand, because that's the only hand I use to hit them, but you can easily change the keycodes if you want. Even if you hit the Shift key on the right side of your keyboard, the current config will make the left hand tap.

For help remapping buttons and finding keycodes: use [this website](https://keycode.info/), the config file uses the same keycodes as JavaScript.

## Original Bongo Cat Overlay Description
An osu! Bongo Cat overlay with smooth paw movement and simple skinning ability, written in C++.

You can find how to configure the application in our [wiki](https://github.com/kuroni/bongocat-osu/wiki/Settings).

Download the program [here](https://github.com/kuroni/bongocat-osu/releases).

Hugs and kisses to [CSaratakij](https://github.com/CSaratakij) for creating the Linux port for this project!

Any suggestion and/or collaboration, especially that relating to sprites, is welcomed! Thank you!

[Original post](https://www.reddit.com/r/osugame/comments/9hrkte/i_know_bongo_cat_is_getting_old_but_heres_a_nicer/) by [Kuvster](https://github.com/Kuvster).

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

