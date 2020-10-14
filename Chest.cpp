#include "Chest.h"
#include "Weapon.h"
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>

//private data
int xCoord;
int yCoord;
Weapon* chestInventory;

//constructor
Chest::Chest(int x, int y){
	xCoord = x;
	yCoord = y;
	chestInventory = new Weapon[5];
}

Chest::Chest(){

}

//getter for x coord of the chest
int Chest::getXCoord(){
    return xCoord;
}

//getter for y coord of the chest
int Chest::getYCoord(){
    return yCoord;
}

//getter for the inventory of the chest
Weapon* Chest::getChestInventory(){
    return chestInventory;
}

//takes in a vector of weapons and will randomly place them into a chest i guess
void Chest::fillChestInventory(std::vector<Weapon*> weaponVector){

	int x;
	for(x = 0; x < 5; x++){
		chestInventory[x] = *(weaponVector.at(x));
	}

	//very basic method, but should fill the chest with weapons from the weapon array

}

bool Chest::chestClicked(SDL_Event e, int xOffset, int yOffset){

	int newXCoord = xCoord - xOffset;
	int newYCoord = yCoord - yOffset;

	int xEnd = newXCoord + 64;
	int yEnd = newYCoord + 64;

	int mouseClickX = e.button.x;
	int mouseClickY = e.button.y;

	if(mouseClickX > newXCoord && mouseClickX < xEnd && mouseClickY > newYCoord && mouseClickY < yEnd){
		//then we have clicked on the chest
		return true;
	}

	return false; //if we did not click on the chest
}




