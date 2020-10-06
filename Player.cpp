#include "Player.h"
#include "Character.h"

void Player::moveCharacter(double x, double y, double map_height, double map_width, double timestep) {

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
}