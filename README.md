# Description

This repository expands on [bongocat-osu ](https://github.com/kuroni/bongocat-osu) by making the mouse work in 3D FPS games such as Counter Strike. 
These previously didn't work with bongocat because these games take control of your cursor and center it on your screen.

First, it detects if your mouse is being centered on your screen. If it is, then the application switches to "raw input" - relative movements of the mouse affect the position of bongocat's mouse, as opposed to the "absolute" position of your cursor. Because we can't detect if mouse has been lifted, if you let go of the mouse, the virtual mouse drifts back to the center of the mousepad. 
<p align="center">
  <img height="400" src="fps.gif">
</p>

# Config

```json
"fpsSensitivity" : 2.0,
"fpsReturnSpeed" : 50
```

"fpsSensitivity" controls how sensitive the bongocat's mouse is to your movements.

"fpsReturnSpeed" controls how fast the bongocat's mouse returns to the center after you let go of your mouse.
