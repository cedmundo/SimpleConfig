# SimpleConfig

SimpleConfiguration module for SDL3 applications. This handles common settings such as:

* Display
* Video
* Audio
* Gameplay
* etc...

## Building and testing

Use CMake, no configuration just yet
```
cmake -B build -S . -G "Ninja"
ninja -C build
ctest --test-dir build --verbose
```
