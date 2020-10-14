#include "Enemy.h"
#include "Arrow.h"
#ifndef ARCHER_H
#define ARCHER_H

class Archer:public Enemy{
    public:
        const int RUN_DISTANCE=200;
        const int CHASE_DISTANCE=400;
        //The last time this enemy fired a shot
        double shotTime = -1;
        using Enemy::Enemy;
        //try and maintain a set distance away from the player 
		void kitePlayer(double x, double y, double map_height, double map_width, double timestep);
        Projectile* moveCharacter(double x, double y, double map_height, double map_width, double timestep);
        Projectile* fireArrow(double x, double y);
};





#endif