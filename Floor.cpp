#include "Floor.h"
#include <stdlib.h>
#include <time.h>
    
    //constant values to represent the codes for each tile type
    const int FLOOR_TILE = 0;
    const int WALL_TILE = 1;
    const int STAIR_TILE = 2;

    int** floorMap;
    int floorHeight;
    int floorWidth;
    int stairX;
    int stairY;

    Floor::Floor(int height,int width){
        floorHeight=height;
        floorWidth=width;
        floorMap=new int*[floorWidth];

        srand(time(0));
        int xRand = rand() % 19; //generate random x value for stairs
        int yRand = rand() % 9; //generates random y value for stairs

        //Should generate a floor but for now creates a blank map of only floor tiles
        for(int x=0;x<floorWidth;x++){
            floorMap[x]=new int[floorHeight];
            for(int y=0;y<floorHeight;y++){
                //here, we are setting the actual tile values
                //0 is a floor tile
                //1 is a wall tile
                //2 is a stair tile
                if(x == xRand && y == yRand){
                    //generate a stair tile here
                    floorMap[x][y] = STAIR_TILE;
                    //set the floors stair coordinates for later use
                    stairX = x * 100;
                    stairY = y * 100;
                }
                else{
                    floorMap[x][y] = FLOOR_TILE;
                }

            }
        }
    }

    int** Floor::getFloorMap(){
        int** ret;
        ret=new int*[floorWidth];
        for(int x=0;x<floorWidth;x++){
            ret[x]=new int[floorHeight];
            for(int y=0;y<floorHeight;y++){
                ret[x][y]=floorMap[x][y];
            }
        }

        return ret;
    }

    //getter for x coord of the stairs
    int Floor::getStairX(){
        return stairX;
    }
    
    //getter for the y coord of the stairs
    int Floor::getStairY(){
        return stairY;
    }