#include "Character.h"
#include "Weapon.h"

#ifndef PLAYER_H
#define PLAYER_H

// this is a class specifically for the playable
// character controlled by the user

class Player: public Character {
	public:
	using Character::Character;
	void moveCharacter(double x, double y, double map_height, double map_width, double timestep);
	std::vector<Weapon*> weapons;
	int currWeapon;
	//decide where the hitbox is depending on sprite in use
	void updateCollision();
	//player hitbox/collision box
	//std::vector<SDL_Rect*> characterBox;
};

#endif