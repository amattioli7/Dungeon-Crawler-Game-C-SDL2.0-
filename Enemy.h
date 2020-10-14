#include "Character.h"

#ifndef ENEMY_H
#define ENEMY_H

// Plan to implement an enemy
// First, give it the ability to exist
// How?  Well it has an x and y coord, so in the game loop just make a vector of enemies and push this on it.  Then, check the representation like a player would.

// Then, give it the ability to move

// Then, make it move under its own volition


class Enemy: public Character{
        public:
        using Character::Character;
        // This is a placeholder method just to differentiate the Enemy class vs the Character class, presumably this would be something that is handled on an enemy by enemy basis
        void attack();
        void moveCharacter(double x, double y, double map_height, double map_width, double timestep);
		//enemy seek is to move an enemy from A -> B, must pass in destination coordinates, return true once there
		bool enemySeek(double xdest, double ydest, double map_height, double map_width, double timestep);
		bool enemyFollow(double xdest, double ydest, double map_height, double map_width, double timestep);
		//to accel enemy in a direction
		void enemyAccel(double x_dir, double y_dir, double timestep);
		//to update enemy image/dstrect
		void updatePos(double x_vel, double y_vel, double map_height, double map_width, double timestep);
		//subtract health when hit
		void hit();
		bool enemyFlee(double xdest, double ydest, double map_height, double map_width, double timestep);
		void enemyAccelAway(double x_dir, double y_dir, double timestep);
		void facePlayer(double x, double y);
		//decide where the hitbox is depending on sprite in use
		void updateCollision();
};
#endif