# Screen Saver #
A cross platform demonstration of a traditional screensaver
using only open source tools (Excluding Windows).

## Windows Build Information ##
Uses MSYS2 as a build environment.  
https://www.msys2.org/  

The version used at the time of development was:  
https://github.com/msys2/msys2-installer/releases/download/2021-06-04/msys2-x86_64-20210604.exe  
MSYS2 should be installed to ```C:\msys64\```.

To setup the development environment, run the following commands from the MSYS2 MSYS terminal:  
Update installed packages with:  
```pacman -Syu```  

Update base packages with:  
```pacman -Su```

Install the MinGW Toolchain:  
```pacman -S --needed base-devel mingw-w64-x86_64-toolchain```  

Install the required libraries:  
```pacman -S mingw64/mingw-w64-x86_64-SDL2```  
```pacman -S mingw64/mingw-w64-x86_64-SDL2_image```

#### Build Process ####
To build the project run the following commands from MSYS2 MinGW 64-bit terminal:  
```g++ ./main.cpp -o main.exe -mwindows -lmingw32 -lSDL2main -lSDL2 -lSDL2_image```

To gather runtime dependencies for a standalone deployment:  
```ldd main.exe | grep '\/mingw.*\.dll' -o | xargs -I{} cp "{}" .```

## Linux Build Information ##

### Arch-based Linux ###
To update installed packages:  
```pacman -Syu```

The development environment will typically be installed by default.  
The main requirements are:  
A C++ compiler *(e.g. g++)*  
The SDL2 library. *(i.e. sdl2)* Installed using: ```sudo pacman -S sdl2```  
The SDL2_image library. *(i.e. sdl2_image)* Installed using: ```sudo pacman -S sdl2_image```

#### Build Process ####
To build the project run the following commands from a terminal:  
```g++ ./main.cpp -o main -lSDL2main -lSDL2 -lSDL2_image```

## Assets ##
Pre-built assets can be found in the ```release/assets/``` directory.

The built executable expects to find the assets in a directory called ```assets```.

*Assets were generated with Blender 2.79b by opening the blend file at:*   ```./rawassets/CompanyLogo.blend```  
*and clicking the "Animation" button from the "Render" section on the right pane.*