#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>

#ifndef FLOOR_H
#define FLOOR_H

class Floor{
    public:
        //Just generates 1 room empty room for now not really 
        Floor(int height, int width);
        int** getFloorMap();
        int getStairX();
        int getStairY();
    private:
        int** floorMap;
        int floorHeight;
        int floorWidth;
        int stairX;
        int stairY;
};

#endif