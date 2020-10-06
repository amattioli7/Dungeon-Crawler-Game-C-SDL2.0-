#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include "Character.h"

Character::Character(double xcoord, double ycoord, int width, int height) :x_pos{xcoord}, y_pos{ycoord}, x_size{width}, y_size{height} {
            x_vel = 0;
            y_vel = 0;
            x_deltav = 0;
            y_deltav = 0;
			//define a max velocity, can use absolute value of x/y_vel so we don't need a separate for negatives
			//max_vel = 5;
            dstrect = {static_cast<int>(x_pos), static_cast<int>(y_pos), x_size, y_size};
            //This is a placeholder value
            health = 100;
            flip=SDL_FLIP_NONE;
            spriteIndex=0;
}
void Character::setSpriteAndDirection(){
    if(x_vel < 0){
        spriteIndex=2;
        flip = SDL_FLIP_NONE;
    }
    else if(x_vel > 0){
        spriteIndex=2;
        flip = SDL_FLIP_HORIZONTAL;
    }
    else if(y_vel < 0){
        spriteIndex=1;
        flip = SDL_FLIP_NONE;
    }
    else if(y_vel > 0){
        spriteIndex=0;
        flip = SDL_FLIP_NONE;
    }
}
