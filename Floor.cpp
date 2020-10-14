#include "Floor.h"
#include <stdlib.h>
#include <time.h>
    
    //constant values to represent the codes for each tile type
    const int FLOOR_TILE = 1;
    const int WALL_TILE = 0;
    const int STAIR_TILE = 2;
    const int SPAWN_TILE = 3;   //Tile where player will spawn (if 2 people connect generate 2 and so on)

    const int MAX_LEAF_SIZE=20; //Max leaf size is defined here but min is defined in the leaf class.

    int** floorMap;
    int floorHeight;
    int floorWidth;
    int stairX;
    int stairY;
	int spawnX;
	int spawnY;

    std::vector<struct Room*> rooms;

    bool seedFlag;
    unsigned int seed;

    Floor::Floor(int height,int width){
        seedFlag=false;
        seed=0;
        srand(time(NULL));

        floorHeight=height;
        floorWidth=width;
        floorMap=new int*[floorWidth];

        srand(time(0));
        int xRand = rand() % 19; //generate random x value for stairs
        int yRand = rand() % 9; //generates random y value for stairs
		
        int xSpawn = rand() % 19; //generate random x value for spawn
        int ySpawn = rand() % 9; //generates random y value for spawn
		
		while(xRand == xSpawn && yRand == ySpawn) { //in case the spawn coordinate overlaps with the stair coordinate
			xSpawn = rand() % 19;
			ySpawn = rand() % 9;
		}

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
				else if(x == xSpawn && y == ySpawn) {
					//generate a stair tile here
                    floorMap[x][y] = SPAWN_TILE;
					spawnX = x * 100;
                    spawnY = y * 100;
				}
                else{
                    floorMap[x][y] = FLOOR_TILE;
                }

            }
        }
    }

    Floor::Floor(int height,int width, unsigned int Seed){
        seedFlag=true;
        seed=Seed;
        srand(Seed);

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

    void Floor::generate(int rMinHeight, int rMinWidth){
        std::vector<BSPLeaf*> leafs;    //List of all leafs created in generation

        BSPLeaf* root = new BSPLeaf(0,0,floorWidth,floorHeight);   //Root leaf that the loop will start with.
        leafs.push_back(root);
        
        bool split=true;    //Flag to monitor if the loop is done or not.
        while(split){
            split=false;
            BSPLeaf* cur;
            //This will go over each leaf in Leafs
            for(std::vector<BSPLeaf*>::iterator it=leafs.begin(); it != leafs.end(); it++){
                cur=*it;
                if(cur->leftChild == NULL && cur->rightChild == NULL){    //leaf is not split
                    //Will split if the current leaf is too big or a 75% chance occurs
                    if(cur->width > MAX_LEAF_SIZE || cur->height > MAX_LEAF_SIZE || rand()%100 < 75){
                        if(cur->split()){
                            leafs.push_back(cur->leftChild);
                            leafs.push_back(cur->rightChild);
                            split=true;
                        }
                    }
                }
            }
        }

        /*
        *   End of leaf generation segment.
        *   Start of room Generation
        */

        root->createRooms(rMinHeight, rMinWidth, rooms);
        
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
	
	    //getter for x coord of the spawn
    int Floor::getSpawnX(){
        return spawnX;
    }
    
    //getter for the y coord of the spawn
    int Floor::getSpawnY(){
        return spawnY;
    }
