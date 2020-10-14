#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include "Archer.h"
#include <cmath>
#include <iostream>
#include "Arrow.h"

//x and y are the coords of the player
void Archer::kitePlayer(double x, double y, double map_height, double map_width, double timestep){  
	//Find the closest point in a certain radius around the player where the enemy can go
    //Accelerate the archer towards that point

    //If we are >= the radius in distance, seek.  Else, flee.

    //This is the distance formula in 2 dimensions

    double distance = std::sqrt(std::pow((x-x_pos), 2)+std::pow((y-y_pos),2));

    
    if(distance>=CHASE_DISTANCE){
        enemyFollow(x, y, map_height, map_width, timestep);
    }
    else if(distance<=RUN_DISTANCE){
        enemyFlee(x, y, map_height, map_width, timestep);
    }


}

Projectile* Archer::moveCharacter(double x, double y, double map_height, double map_width, double timestep){
	kitePlayer(x, y, map_height, map_width, timestep);
    facePlayer(x, y);
    double curTime=SDL_GetTicks();
    Projectile* a = nullptr;

    //I don't want it to shoot immediately - give it a few seconds
    if(shotTime==-1){
        shotTime=curTime;
    }

    if(curTime-shotTime>=3000){
        a=fireArrow(x, y);
        shotTime=curTime;
    }

    return a;
}

//Fire an arrow towards (x,y)
Projectile* Archer::fireArrow(double x, double y){
    Projectile* a = new Projectile(x_pos+x_size/2, y_pos+y_size/2);
    a->target(x,y);
    return a;
}