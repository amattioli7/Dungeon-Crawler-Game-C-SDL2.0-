#include "Projectile.h"
#include <string>
#include <cmath>
#include <iostream>

Projectile::Projectile(double x, double y): x_pos{x}, y_pos{y}{
    dstrect={static_cast<int>(x_pos), static_cast<int>(y_pos), x_size, y_size};

    flip=SDL_FLIP_NONE;
    spriteIndex=0;

}

void Projectile::target(double x, double y){
    double xdist = x-x_pos;
    double ydist = y-y_pos;
    double dist = std::sqrt(std::pow((xdist), 2)+std::pow((ydist),2));

    
    x_vel=SPEED*xdist/dist;
    y_vel=SPEED*ydist/dist;

    if(std::abs(x-x_pos) >= std::abs(y-y_pos)){

		//To the left of the player
		if(x_pos > x){
			spriteIndex=2;
			flip = SDL_FLIP_NONE;
		}
		//To the right of the player
		else if(x_pos < x){
			spriteIndex=2;
			flip = SDL_FLIP_HORIZONTAL;
		}
	}
	else{

		//Underneath the player
		if(y_pos > y){
			spriteIndex=1;
			flip = SDL_FLIP_NONE;
		}
		//Above the player
		else if(y_pos < y){
			spriteIndex=0;
			flip = SDL_FLIP_NONE;
		}
	}
    
}

std::vector<SDL_Texture*> Projectile::sprites;

int Projectile::moveProjectile(double map_height, double map_width, double timestep){
    y_pos += (y_vel * timestep);
	if (y_pos < 0){
		y_pos = 0;
        return -1;
    }
	else if(y_pos + y_size > map_height){
		y_pos = map_height - y_size;
        return -1;
    }
	x_pos += (x_vel * timestep);
	if (x_pos < 0){
		x_pos = 0;
        return -1;
    }
	else if(x_pos + x_size > map_width){
		x_pos = map_width - x_size;
        return -1;
    }

    // Move the hitbox accordingly
    setHitbox();
    return 0;
}


void Projectile::setHitbox(){
    if(spriteIndex==2) hitBox={static_cast<int>(x_pos+2), static_cast<int>(y_pos+12), 21, 5};
    else hitBox={static_cast<int>(x_pos+12), static_cast<int>(y_pos+2), 5, 21};
}

SDL_Texture* Projectile::getSprite(){
    return sprites[spriteIndex];
}
