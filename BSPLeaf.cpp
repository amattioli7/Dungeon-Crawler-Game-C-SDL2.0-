#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "BSPLeaf.h"

    const int MIN_LEAF_SIZE=6;  //Min leaf size is defined here but max is defined in the floor class.
    int x, y, width, height;    //Position and size of leaf
    unsigned int seedBSP;
    bool seedFlagBSP;

    BSPLeaf* leftChild;
    BSPLeaf* rightChild;
    struct Room* room;   //Room the leaf contains.
    std::vector<struct Hall*> hallways;
    //TODO Some vector to handle hallways


    BSPLeaf::BSPLeaf(int X,int Y, int w, int h){
        room=NULL;
        x=X;
        y=Y;
        width=w;
        height=h;
        srand(time(NULL));
        seedBSP=0;
        seedFlagBSP=false;
    }
    //A clone of the normal construtor but with a seed for the randomization.
    BSPLeaf::BSPLeaf(int X,int Y, int w, int h, unsigned int Seed){
        room=NULL;
        x=X;
        y=Y;
        width=w;
        height=h;
        srand(Seed);
        seedBSP=Seed;
        seedFlagBSP=true;
    }

    bool BSPLeaf::split(){
        if(leftChild!=NULL||rightChild!=NULL){
            return false;   //Already split
        }
        bool splitH = (rand() % 2) == 0;   //50/50 on whether we split vertically or horizontally
        if(width > height && (float)width / (float)height >= 1.25){
            splitH=false;   //Width is 25% larger than height so split horizontally
        }
        else if(height > width && (float)height / (float)width >= 1.25){
            splitH=true;   //Height is 25% larger than width so split vertically
        }
        //If neither of these are true it'll be the result of the random earlier.

        int max;
        if(splitH){
            max=height-MIN_LEAF_SIZE;   //Max size of a leaf
        }
        else{
            max=width-MIN_LEAF_SIZE;
        }

        if(max<=MIN_LEAF_SIZE){
            return false;   //Leaf is too small to split further
        }

        int split = (rand() % max) + MIN_LEAF_SIZE;

        if(splitH){ //Spliting along height
            //Checks if seed was used in generation.
            if(seedFlagBSP){
                leftChild= new BSPLeaf(x,y,width,split, seedBSP);
                rightChild= new BSPLeaf(x+split,y,width,height-split,seedBSP);
            }
            else{
                leftChild= new BSPLeaf(x,y,width,split);
                rightChild= new BSPLeaf(x+split,y,width,height-split);
            }
        }
        else{   //Spliting along the width
            if(seedFlagBSP){
                leftChild= new BSPLeaf(x,y,split,height, seedBSP);
                rightChild= new BSPLeaf(x,y+split,width-split,height,seedBSP);
            }
            else{
                leftChild= new BSPLeaf(x,y,split,height);
                rightChild= new BSPLeaf(x,y+split,width-split,height);
            }
        }

        return true;
    }

    void BSPLeaf::createRooms(int rMinHeight, int rMinWidth, std::vector<struct Room*> rooms){    //Long function but stuff important to make adding rooms to the map easier.
        if(leftChild!=NULL||rightChild!=NULL){
            //Go down until it has reached the leaves
            if(leftChild!=NULL){
                leftChild->createRooms(rMinHeight, rMinWidth, rooms);
            }
            if(rightChild!=NULL){
                rightChild->createRooms(rMinHeight, rMinWidth, rooms);
            }
        }
        else{   //This leaf is a... leaf, the binary tree kind
            //Sets rooms pos but not on the edge of the leaf to avoid merging rooms
            room->width=(rand() % (width - 2 - rMinWidth)) + rMinWidth;
            room->height=(rand() % (height - 2 - rMinHeight)) + rMinHeight;
            room->x=(rand() % (width - room->width - 2)) + 1 + x;
            room->y=(rand() % (height - room->height - 2)) + 1 + y;
            rooms.push_back(room);  //Adds room to a vector the floor class can access and then later handle drawing it to the map

        }
    }

    struct Room* BSPLeaf::getRoom(){
        if(room!=NULL){
            return room;
        }
        else{
            struct Room* left;
            struct Room* right;
            if(leftChild!=NULL){
                left=leftChild->getRoom();
            }
            if(rightChild!=NULL){
                right=rightChild->getRoom();
            }
            if(left==NULL && right==NULL){
                return NULL;    //This should never happen seriously for the love of god hope it never hits this point
            }
            else if(right==NULL){
                return left;
            }
            else if(left==NULL){
                return right;
            }
            else if(rand() % 2 == 0){
                return left;
            }
            else{
                return right;
            }
        }
    }

    void BSPLeaf::createHall(struct Room* left, struct Room* right){
        struct Hall* oats;
        oats->x1=rand();
    }

