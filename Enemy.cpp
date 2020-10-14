#include "Enemy.h"
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
	
	//update collision boxes
	updateCollision();
	
	/*
	//check if collision with player or enemy, will need to update for when multiple entities in game
	SDL_Rect result;
	if(SDL_IntersectRect(&dstrect, &player, &result))
	{
		for(int i = 0; i <= 3; i++)
		{
			for(int j = 0; j <= 3; j++)
			{
				if(SDL_IntersectRect(&characterBox[i], &playerColl[j], &result))
				{
					x_pos -= (x_vel * timestep);
					y_pos -= (y_vel * timestep);
					break;
				}
			}
		}
	}
	if(SDL_IntersectRect(&dstrect, &enemy, &result))
	{
		for(int i = 0; i <= 3; i++)
		{
			for(int j = 0; j <= 3; j++)
			{
				if(SDL_IntersectRect(&characterBox[i], &enemyColl[j], &result))
				{
					x_pos -= (x_vel * timestep);
					y_pos -= (y_vel * timestep);
					break;
				}
			}
		}
	}
	*/
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

//update collision box for enemy - tailored only for skeleton enemy right now
void Enemy::updateCollision()
{
	//using sprite indexes to tell what hitboxes to use for now, once sprite sheets are in use & animations improved can implement a direction variable
	
	//if front or back facing
	if(spriteIndex <= 1)
	{
		characterTree->root->hitbox.x = x_pos+24;
		characterTree->root->hitbox.y = y_pos+13;
		characterTree->root->hitbox.w = 31;
		characterTree->root->hitbox.h = 86;
		
		/*
		//index 0 = head, 1 = torso, 2 = legs, 3 = arms
		// root = head, children[0] = torso, children[1] = legs, children[2] = arms
		//head box, position it at top left corner of dstrect + offset
		characterTree->root->hitbox.x = x_pos+24;
		characterTree->root->hitbox.y = y_pos+13;
		characterTree->root->hitbox.w = 30;
		characterTree->root->hitbox.h = 24;
		//torso box, position it at top left corner of dstrect + offset
		characterBox[1].x = x_pos+24;
		characterBox[1].y = y_pos+41;
		characterBox[1].w = 31;
		characterBox[1].h = 35;
		//legs box, position it at top left corner of dstrect + offset
		characterBox[2].x = x_pos+30;
		characterBox[2].y = y_pos+77;
		characterBox[2].w = 19;
		characterBox[2].h = 22;
		//arms box, position it at top left corner of dstrect + offset
		characterBox[3].x = x_pos+24;
		characterBox[3].y = y_pos+41;
		characterBox[3].w = 31;
		characterBox[3].h = 35;
		*/
	}
	//if left facing
	else if(spriteIndex == 2)
	{
		characterTree->root->hitbox.x = x_pos;//+25;
		characterTree->root->hitbox.y = y_pos;//+13;
		characterTree->root->hitbox.w = 29;
		characterTree->root->hitbox.h = 86;
		/*
		//index 0 = head, 1 = torso, 2 = legs, 3 = arms
		//head box, position it at top left corner of dstrect + offset
		characterTree->root->hitbox.x = x_pos+25;
		characterTree->root->hitbox.y = y_pos+13;
		characterTree->root->hitbox.w = 29;
		characterTree->root->hitbox.h = 23;
		//torso box, position it at top left corner of dstrect + offset
		characterBox[1].x = x_pos+27;
		characterBox[1].y = y_pos+41;
		characterBox[1].w = 20;
		characterBox[1].h = 27;
		//legs box, position it at top left corner of dstrect + offset
		characterBox[2].x = x_pos+33;
		characterBox[2].y = y_pos+68;
		characterBox[2].w = 12;
		characterBox[2].h = 31;
		//arms box, position it at top left corner of dstrect + offset
		characterBox[1].x = x_pos+27;
		characterBox[1].y = y_pos+41;
		characterBox[1].w = 20;
		characterBox[1].h = 27;
		*/
	}
	//if right facing
	else if(spriteIndex == 3)
	{
		characterTree->root->hitbox.x = x_pos;//+25;
		characterTree->root->hitbox.y = y_pos;//+13;
		characterTree->root->hitbox.w = 29;
		characterTree->root->hitbox.h = 86;
		/*
		//index 0 = head, 1 = torso, 2 = legs, 3 = arms
		//head box, position it at top left corner of dstrect + offset
		characterTree->root->hitbox.x = x_pos+25;
		characterTree->root->hitbox.y = y_pos+13;
		characterTree->root->hitbox.w = 29;
		characterTree->root->hitbox.h = 23;
		//torso box, position it at top left corner of dstrect + offset
		characterBox[1].x = x_pos+32;
		characterBox[1].y = y_pos+41;
		characterBox[1].w = 20;
		characterBox[1].h = 27;
		//legs box, position it at top left corner of dstrect + offset
		characterBox[2].x = x_pos+34;
		characterBox[2].y = y_pos+68;
		characterBox[2].w = 12;
		characterBox[2].h = 31;
		//arms box, position it at top left corner of dstrect + offset
		characterBox[1].x = x_pos+32;
		characterBox[1].y = y_pos+41;
		characterBox[1].w = 20;
		characterBox[1].h = 27;
		*/
	}
}