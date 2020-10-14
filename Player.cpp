#include "Player.h"

void Player::moveCharacter(double x, double y, double map_height, double map_width, double timestep)
{
	//delta x and y for camera/character
	x_deltav = 0;
	y_deltav = 0;
	//detecting key states for movement
	const Uint8* keystate = SDL_GetKeyboardState(nullptr);
	if (keystate[SDL_SCANCODE_W])
		y_deltav -= (ACCEL * timestep);
	if (keystate[SDL_SCANCODE_A])
		x_deltav -= (ACCEL * timestep);
	if (keystate[SDL_SCANCODE_S])
		y_deltav += (ACCEL * timestep);
	if (keystate[SDL_SCANCODE_D])
		x_deltav += (ACCEL * timestep);

	//check if delta_v = 0 (no key pressed), if so decrement velocity
	//first checking x_deltav
	if(x_deltav == 0)
	{
		if(x_vel > 0)
		{
			if(x_vel < (ACCEL * timestep))
				x_vel = 0;
			else
				x_vel -= (ACCEL * timestep);
		}
		else if(x_vel < 0)
		{
			if(-x_vel < (ACCEL * timestep))
				x_vel = 0;
			else
				x_vel += (ACCEL * timestep);
		}
	}
	else
		x_vel += x_deltav;

	//now checking y_deltav
	if(y_deltav == 0)
	{
		if(y_vel > 0)
			if(y_vel < (ACCEL * timestep))
				y_vel = 0;
			else
				y_vel -= (ACCEL * timestep);
		else if(y_vel < 0)
			if(-y_vel < (ACCEL * timestep))
				y_vel = 0;
			else
				y_vel += (ACCEL * timestep);
	}
	else
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

	//update coordinates
	x_pos += (x_vel * timestep);
	y_pos += (y_vel * timestep);
	//check if out of bounds
	//x direction
	if (x_pos < 0)
		x_pos = 0;
	if (x_pos + x_size > map_width)
		x_pos = map_width - x_size;
	//y direction
	if (y_pos < 0)
		y_pos = 0;
	if (y_pos + y_size > map_height)
		y_pos = map_height - y_size;
	
	//update collision boxes
	updateCollision();
	
}

//decide which collision boxes to use
void Player::updateCollision()
{
	//using sprite indexes to tell what hitboxes to use for now, once sprite sheets are in use & animations improved can implement a direction variable
	
	//if front or back facing
	if(spriteIndex <= 17)
	{
		
		//tried making just one hitbox for a direction instead of complicating things
		characterTree->root->hitbox.x = x_pos;
		characterTree->root->hitbox.y = y_pos;
		characterTree->root->hitbox.w = 60;
		characterTree->root->hitbox.h = 100;
		
		/*
		//index 0 = head, 1 = torso, 2 = legs, 3 = arms
		//head box, position it at top left corner of dstrect + offset
		characterTree->root->hitbox.x = x_pos+48;
		characterTree->root->hitbox.y = y_pos+28;
		characterTree->root->hitbox.w = 35;
		characterTree->root->hitbox.h = 39;
		//torso box, position it at top left corner of dstrect + offset
		characterBox[1].x = x_pos+44;
		characterBox[1].y = y_pos+72;
		characterBox[1].w = 43;
		characterBox[1].h = 35;
		//legs box, position it at top left corner of dstrect + offset
		characterBox[2].x = x_pos+48;
		characterBox[2].y = y_pos+108;
		characterBox[2].w = 35;
		characterBox[2].h = 19;
		//arms box, position it at top left corner of dstrect + offset
		characterBox[3].x = x_pos+28;
		characterBox[3].y = y_pos+76;
		characterBox[3].w = 75;
		characterBox[3].h = 23;
		*/
	}
	//if left facing
	else if(spriteIndex >= 18 && (flip == SDL_FLIP_NONE))
	{
		characterTree->root->hitbox.x = x_pos;
		characterTree->root->hitbox.y = y_pos;
		characterTree->root->hitbox.w = 47;
		characterTree->root->hitbox.h = 100;
		/*
		//index 0 = head, 1 = torso, 2 = legs, 3 = arms
		//head box, position it at top left corner of dstrect + offset
		characterTree->root->hitbox.x = x_pos+40;
		characterTree->root->hitbox.y = y_pos+28;
		characterTree->root->hitbox.w = 23;
		characterTree->root->hitbox.h = 39;
		//torso box, position it at top left corner of dstrect + offset
		characterBox[1].x = x_pos+40;
		characterBox[1].y = y_pos+72;
		characterBox[1].w = 31;
		characterBox[1].h = 35;
		//legs box, position it at top left corner of dstrect + offset
		characterBox[2].x = x_pos+44;
		characterBox[2].y = y_pos+108;
		characterBox[2].w = 19;
		characterBox[2].h = 19;
		//arms box, position it at top left corner of dstrect + offset
		characterBox[3].x = x_pos+24;
		characterBox[3].y = y_pos+80;
		characterBox[3].w = 15;
		characterBox[3].h = 11;
		*/
	}
	//if right facing
	else if(spriteIndex >= 18 && (flip == SDL_FLIP_HORIZONTAL))
	{
		characterTree->root->hitbox.x = x_pos+24;
		characterTree->root->hitbox.y = y_pos+28;
		characterTree->root->hitbox.w = 47;
		characterTree->root->hitbox.h = 100;
		
		/*
		//index 0 = head, 1 = torso, 2 = legs, 3 = arms
		//head box, position it at top left corner of dstrect + offset
		characterTree->root->hitbox.x = x_pos+64;
		characterTree->root->hitbox.y = y_pos+28;
		characterTree->root->hitbox.w = 23;
		characterTree->root->hitbox.h = 39;
		//torso box, position it at top left corner of dstrect + offset
		characterBox[1].x = x_pos+40;
		characterBox[1].y = y_pos+72;
		characterBox[1].w = 31;
		characterBox[1].h = 35;
		//legs box, position it at top left corner of dstrect + offset
		characterBox[2].x = x_pos+44;
		characterBox[2].y = y_pos+108;
		characterBox[2].w = 19;
		characterBox[2].h = 19;
		//arms box, position it at top left corner of dstrect + offset
		characterBox[3].x = x_pos+24;
		characterBox[3].y = y_pos+80;
		characterBox[3].w = 15;
		characterBox[3].h = 11;
		*/
	}
		
}
