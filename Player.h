#include "Character.h"

#ifndef PLAYER_H
#define PLAYER_H

// this is a class specifically for the playable
// character controlled by the user

class Player: public Character {
	public:
	using Character::Character;
	void moveCharacter(double x, double y, double map_height, double map_width, double timestep);
};

#endif