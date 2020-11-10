# Debian
## Dependencies:

Install GL: sudo apt-get install build-essential libgl1-mesa-dev  
Install SFML: sudo apt-get install libsfml-dev

## Build with meson and ninja:

> meson build  
cd build/  
ninja

# Windows:
## Dependencies:

Find [SFML Binaries](https://www.sfml-dev.org/download/sfml/2.5.1/) or 
[compile it yourself](https://www.sfml-dev.org/tutorials/2.5/compile-with-cmake.php)

## Build with VS:
If you have problems, make sure that you use the same compiler for all builds. 

> meson build --backend vs

# Run from build/

> ./swone

