#include "Enemy.h"
#include "Character.h"
#include <cmath>
#include <iostream>

void Enemy::attack() { return; }
void Enemy::moveCharacter(double x, double y, double map_height, double map_width, double timestep) {return;} //TODO: Implement some sort of movement code for the enemy


//method to move enemy from present position -> destination
bool Enemy::enemySeek(double xdest, double ydest, double map_height, double map_width, double timestep)
{
	//define x and y directions to travel in
	double x_dir = xdest - x_pos;
	double y_dir = ydest - y_pos;
	
	//if there, reset x/y velocities and deltav and return true
	if((int) x_dir == 0 && (int) y_dir == 0)
	{
		x_vel = 0;
		y_vel = 0;
		x_deltav = 0;
		y_deltav = 0;
		//update position
		updatePos(x_vel, y_vel, map_height, map_width, timestep);
		return true;
	}

	//call on enemyAccel to move enemy
	this->enemyAccel(x_dir, y_dir, timestep);
	
	//update enemy image/position
	updatePos(x_vel, y_vel, map_height, map_width, timestep);
	
	return false;
}

//method to follow player
bool Enemy::enemyFollow(double xdest, double ydest, double map_height, double map_width, double timestep)
{
	//define x and y directions to travel in
	double x_dir = xdest - x_pos;
	double y_dir = ydest - y_pos;
	
	//if there, reset x/y velocities and deltav and return true, eventually make it so if collision = true, do something
	if(((x_dir <= 15 && x_dir > 0) || (x_dir >= -15 && x_dir < 0))  && ((y_dir <= 15 && y_dir > 0) || (y_dir >= -15 && y_dir < 0)))
	{
		x_vel = 0;
		y_vel = 0;
		x_deltav = 0;
		y_deltav = 0;
		return true;
	}
	
	//call on enemyAccel to move enemy
	this->enemyAccel(x_dir, y_dir, timestep);
	
	//update enemy image/position
	updatePos(x_vel, y_vel, map_height, map_width, timestep);
	
	return false;	
}

bool Enemy::enemyFlee(double xdest, double ydest, double map_height, double map_width, double timestep)
{
	//define x and y directions to travel in
	double x_dir = xdest - x_pos;
	double y_dir = ydest - y_pos;
	
	//call on enemyAccel to move enemy
	this->enemyAccelAway(x_dir, y_dir, timestep);
	
	//update enemy image/position
	updatePos(x_vel, y_vel, map_height, map_width, timestep);
	
	return false;	
}

//method to accelerate enemy towards the character
void Enemy::enemyAccel(double x_dir, double y_dir, double timestep)
{
	//set x and y deltav to 0
	x_deltav = 0;
	y_deltav = 0;
	//check if we should move in the positive or negative x direction
	if((int) x_dir < 0)
		x_deltav -= (ACCEL * timestep);
	else if((int) x_dir > 0)
		x_deltav += (ACCEL * timestep);
	if((int) y_dir < 0)
		y_deltav -= (ACCEL * timestep);
	else if((int) y_dir > 0)
		y_deltav += (ACCEL * timestep);
	
	//check if delta_v = 0 (reached x/y destination), if so decrement velocity
	//first checking x_deltav
	if(x_deltav == 0)
	{
		if(x_vel > 0){
			if(x_vel < (ACCEL * timestep))
				x_vel = 0;
			else
				x_vel -= (ACCEL * timestep);
		}
		else if(x_vel < 0){
			if(-x_vel < (ACCEL * timestep)){
				x_vel = 0;
			}
			else {
				x_vel  += (ACCEL * timestep);
			}
		}
	}
	else
		x_vel += x_deltav;
	//now checking y_deltav
	if(y_deltav == 0){
	{
		if(y_vel > 0){
			if(y_vel < (ACCEL * timestep))
				y_vel = 0;
			else {
				y_vel -= (ACCEL * timestep);
			}
		}
		else if(y_vel < 0){
			if(-y_vel < (ACCEL * timestep))
				y_vel = 0;
			else {
				y_vel  += (ACCEL * timestep);
			}
		}
	}
}
	else {
		y_vel += y_deltav;
	}
		
	
	//check if speed limit is exceeded
	//x direction
	if(x_vel < -SPEED_LIMIT)
		x_vel = -SPEED_LIMIT;
	else if(x_vel > SPEED_LIMIT)
		x_vel = SPEED_LIMIT;
	//y direction
	if(y_vel < -SPEED_LIMIT)
		y_vel = -SPEED_LIMIT;
	else if(y_vel > SPEED_LIMIT)
		y_vel = SPEED_LIMIT;

	return;
}

//method to accelerate the enemy away from the player
void Enemy::enemyAccelAway(double x_dir, double y_dir, double timestep)
{
	//set x and y deltav to 0
	x_deltav = 0;
	y_deltav = 0;
	//check if we should move in the positive or negative x direction
	
	if((int) x_dir < 0)
		x_deltav += (ACCEL * timestep);
	else if((int) x_dir > 0)
		x_deltav -= (ACCEL * timestep);
	if((int) y_dir < 0)
		y_deltav += (ACCEL * timestep);
	else if((int) y_dir > 0)
		y_deltav -= (ACCEL * timestep);
	

	x_vel += x_deltav;

	y_vel += y_deltav;
		
	
	//check if speed limit is exceeded
	//x direction
	if(x_vel < -SPEED_LIMIT)
		x_vel = -SPEED_LIMIT;
	else if(x_vel > SPEED_LIMIT)
		x_vel = SPEED_LIMIT;
	//y direction
	if(y_vel < -SPEED_LIMIT)
		y_vel = -SPEED_LIMIT;
	else if(y_vel > SPEED_LIMIT)
		y_vel = SPEED_LIMIT;

	return;
}

//method to update enemy x/y positioning
void Enemy::updatePos(double x_vel, double y_vel, double map_height, double map_width, double timestep)
{
	setSpriteAndDirection();
	// move the sdl_rect
	y_pos += (y_vel * timestep);
	if (y_pos < 0)
		y_pos = 0;
	else if(y_pos + y_size > map_height)
		y_pos = map_height - y_size;
	x_pos += (x_vel * timestep);
	if (x_pos < 0)
		x_pos = 0;
	else if(x_pos + x_size > map_width)
		x_pos = map_width - x_size;
}
//subtract health when hit
void Enemy::hit()
{	
	health-=10;
}

//Face the player
void Enemy::facePlayer(double x, double y){
    
	//Check if the x/y difference is greater
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