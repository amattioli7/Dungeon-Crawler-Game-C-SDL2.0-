#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#ifndef CHARACTER_H
#define CHARACTER_H

class Character{

    public:
        Character(double xcoord, double ycoord, int width, int height);
        void setSpriteAndDirection();
        double x_vel;
        double y_vel;
        double x_pos; //these are absolute
        double y_pos;
        double x_deltav;
        double y_deltav;
		//Speed limit -> for 5px/frame @60fps = 300px/s
		double SPEED_LIMIT = 300.0;
		//acceleration -> for 1px/frame^2 @60fps = 3600px/s^2
		double ACCEL = 3600.0;
        double health;
        int x_size;
        int y_size;
        int spriteIndex;
        SDL_RendererFlip flip;
        SDL_Rect dstrect;
        std::vector<SDL_Texture*> sprites; //0 faces forward, 1 faces backward, 2 faces left
        void moveCharacter(); //For enemies this will run the AI, for players this will run the player input routine
            // This should be virtual but I was having problems at compile time with it
};


#endif