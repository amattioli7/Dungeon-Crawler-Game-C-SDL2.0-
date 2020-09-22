# Canonical repo for our Dungeon Crawler game
For running on Windows
1. Install mingw32 (could use 64 but 32 was easier)
    Include MSYS so we have Make as well as gcc
    Also add to PATH
2. Download SDL2 and SDL2_image (image needs downloaded separately)
3. Move .a files from the lib folders of both downloads (i686 folder) to the lib folder of MinGW
4. Go to the include folders of the SDL downloads and combine the SDL2 folders found within (full of .h files)
5. Move the now combined SDL2 folder into the include folder of MinGW
6. Pull the most recent changes to the DungeonCrawler repo
7. Compile with the line g++ -I /usr/include/SDL2 main.cpp -lSDL2 -lSDL2_image
8. run "mingw32-make.exe" to use makefile


LINUX:
Make sure to have a linux env such as Ubuntu installed first then:
--DOWNLOAD https://sourceforge.net/projects/xming/ for WSL (XServer to map graphics)
1.  #install sdl2 <br />
    sudo apt install libsdl2-dev libsdl2-2.0-0 -y;

    #install sdl image <br />
    sudo apt install libjpeg-dev libwebp-dev libtiff5-dev libsdl2-image-dev libsdl2-image-2.0-0 -y;

    #install sdl mixer <br />
    sudo apt install libmikmod-dev libfishsound1-dev libsmpeg-dev liboggz2-dev libflac-dev libfluidsynth-dev libsdl2-mixer-dev libsdl2-mixer-2.0-0 -y;

    #install sdl ttf <br />
    sudo apt-get install libsdl2-ttf-dev

2. Navigate to directory where project is
3. run command "make"
4. run "./main.o
