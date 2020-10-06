#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include "Archer.h"
#include <cmath>
#include <iostream>

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

void Archer::moveCharacter(double x, double y, double map_height, double map_width, double timestep){
    kitePlayer(x, y, map_height, map_width, timestep);
    facePlayer(x, y);
}