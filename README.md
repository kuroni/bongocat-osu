# This is Linux native port
- for more information on how to use, please see the [upstream](https://github.com/kuroni/bongocat-osu),
[see how it perform](https://youtu.be/OylY3CnOpKs)

# Change
- Dynamic link sfml
- Add toggle smoke key in osu!
- reduce keyboard press rate timeout in osu!, ctb and taiko mode to 0.002 to reduce the feeling of input lag
- Xrandr for detect monitor refresh rate
- SDL2 for message box
- libxdo for retreive the cursor point and retreive the foreground window
- SFML for detect keypress and the rest

# Dependencies
- g++
- libxdo
- sdl2
- sfml
- x11
- xrandr

# Getting start
1. make sure you have all dependencies and its lib install
2. clone this repository
```sh
git clone "https://github.com/CSaratakij/bongocat-osu" && cd bongocat-osu
```
3. checkout 'feature-linux-native-port' branch
```sh
git checkout origin/feature-linux-native-port
```
4. copy Makefile.linux to Makefile
```sh
cp Makefile.linux Makefile
```
5. run make command
```sh
make
```
6. copy or symbolic link executable from 'bin/bongo' to root directory
```sh
cp bin/bongo .
```
7. make it executable
```sh
chmod +x bongo
```
8. run
```sh
./bongo
```

# Known issue
- Currently, it find image by relative path. So make sure to run executable inside the directory that have 'img' dir in it.

