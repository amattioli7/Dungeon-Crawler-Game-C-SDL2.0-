#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <cstdlib>
#include <ctime>

#ifndef BSPLEAF_H
#define BSPLEAF_H

struct Room{
    int x;
    int y;
    int height;
    int width;
};

struct Hall{
    int x1,y1;
    int x2,y2;
};

class BSPLeaf{
    public:
        int x, y, width, height;    //Position and size of leaf
        unsigned int seedBSP;
        bool seedFlagBSP;

        BSPLeaf* leftChild;
        BSPLeaf* rightChild;
        struct Room* room;   //Room the leaf contains.
        //Need to find some equivilant for halls.

        BSPLeaf(int X,int Y, int w, int h);
        BSPLeaf(int X,int Y, int w, int h, unsigned int seed);

        bool split();
        void createRooms(int rMinHeight, int rMinWidth, std::vector<struct Room*> rooms);
        struct Room* getRoom();
        void createHall(struct Room* left, struct Room* right);

    private:

};

#endif