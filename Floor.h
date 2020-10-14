#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "BSPLeaf.h"

#ifndef FLOOR_H
#define FLOOR_H

class Floor{
    public:
        //Just generates 1 room empty room for now not really 
        Floor(int height, int width);
        Floor(int height, int width, unsigned int Seed);
        int** getFloorMap();
        int getStairX();
        int getStairY();
		int getSpawnX();
        int getSpawnY();
        void generate(int rMinHeight, int rMinWidth);
  
    private:
        bool seedFlag;
        unsigned int seed;
        int** floorMap;
        int floorHeight;
        int floorWidth;
        int stairX;
        int stairY;
		int spawnX;
		int spawnY;
};

#endif