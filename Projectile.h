#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <iostream>

#ifndef PROJECTILE_H
#define PROJECTILE_H


//TODO: This currently implements the Arrow class.  For some reason I could not get the drawProjectiles function to work how I wanted to with the inheritance structure.
//This should be fixed down the road


class Projectile{
    
    public:
        double SPEED=350;
        double DAMAGE;
        int x_size=25;
        int y_size=25;
        int spriteIndex;
        SDL_RendererFlip flip;
        SDL_Rect dstrect;
        //Static to save space
        static std::vector<SDL_Texture*> sprites;
        SDL_Rect hitBox;
        double x_vel;
        double y_vel;
        double x_pos;
        double y_pos;
        bool friendly;

        virtual int moveProjectile(double map_height, double map_width, double timestep);
        Projectile(double x, double y);
        //In this method the sprite is set as well as the x and y size
        
        virtual void setHitbox();

        //set the velocity towards a certain coordinate
        virtual void target(double x, double y);

        //Getter to preserve inheritance structure with the static sprite list
        virtual SDL_Texture* getSprite();
};

#endif