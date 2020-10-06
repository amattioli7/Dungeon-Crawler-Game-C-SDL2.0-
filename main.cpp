#define SDL_MAIN_HANDLED
#include <iostream>
#include <vector>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include "Character.h"
#include "Player.h"
#include "Enemy.h"
#include "Weapon.h"
#include "Floor.h"
#include "Archer.h"

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;
constexpr int BOX_WIDTH = 80;
constexpr int BOX_HEIGHT = 100;
constexpr int BG_WIDTH = 1920;	// temporary dimensions for scrolltest.png
constexpr int BG_HEIGHT = 1080;
bool weaponPickedUp;

//FPS cap for game
const int SCREEN_FPS = 60;
//amount of display time per frame
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

//Speed limit -> for 5px/frame @60fps = 300px/s
constexpr double SPEED_LIMIT = 300.0;
//acceleration -> for 1px/frame^2 @60fps = 3600px/s^2
constexpr double ACCEL = 3600.0;
int spriteIndex;
// Function declarations
bool init();
SDL_Texture* loadImage(std::string fname);
void close();
// Globals
SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
std::vector<SDL_Texture*> gTex;
std::vector<SDL_Texture*> sprites;
std::vector<SDL_Texture*> maps; //the maps vector
std::vector<SDL_Texture*> tiles;	//Assets for map drawing (Currerntly all tiles are 32x32 pixels, stairs included)
std::vector<Weapon*> all_weapons;	//All of the weapons currently in game (including on floor and in inventory)
std::vector<Weapon*> inventory;	//Player inventory of weapons, could/should be moved to Player class when created. subset of all_weapons

bool init() {
	// Flag what subsystems to initialize
	// For now, just video
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		return false;
	}
	// Set texture filtering to linear
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
		std::cout << "Warning: Linear texture filtering not enabled!" << std::endl;
	}
	gWindow = SDL_CreateWindow("Dungeon Crawler", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (gWindow == nullptr) {
		std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		return false;
	}
	/* Create a renderer for our window
	 * Use hardware acceleration (last arg)
	 * Choose first driver that can provide hardware acceleration
	 *   (second arg, -1)
	 */
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);// | SDL_RENDERER_PRESENTVSYNC);
	if (gRenderer == nullptr) {
		std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		return false;
	}
	// Set renderer draw/clear color
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
	// Initialize PNG loading via SDL_image extension library
	int imgFlags = IMG_INIT_PNG;
	int retFlags = IMG_Init(imgFlags);
	if (retFlags != imgFlags) {
		std::cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
		return false;
	}
	return true;
}


SDL_Texture* loadImage(std::string fname) {
	SDL_Texture* newText = nullptr;
	SDL_Surface* startSurf = IMG_Load(fname.c_str());
	if (startSurf == nullptr) {
		std::cout << "Unable to load image " << fname << "! SDL Error: " << SDL_GetError() << std::endl;
		return nullptr;
	}
	newText = SDL_CreateTextureFromSurface(gRenderer, startSurf);
	if (newText == nullptr) {
		std::cout << "Unable to create texture from " << fname << "! SDL Error: " << SDL_GetError() << std::endl;
	}
	SDL_FreeSurface(startSurf);
	return newText;
}


void close() {
	for (auto i : gTex) {
		SDL_DestroyTexture(i);
		i = nullptr;
	}
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;
	gRenderer = nullptr;
	// Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}
//Are the two objects in range/collide with each other. (In progress)
/*bool inRange( SDL_Rect A, SDL_Rect B )
{
   //checking if any sides of the rectangles touch
    if( (A.y + A.h) < B.y )
        return false;
    
    if( A.y > (B.y + B.h) )
        return false;

    if((A.x + A.w) < B.x)
        return false;

    if( A.x > (B.x + B.w) )
        return false;

    //there is a collision
    return true;
} */
// cameraRect is passed in so we know if we need to draw or not.
void drawEnemy(Enemy* c, SDL_Rect cameraRect){
	// c->setSpriteAndDirection();

	// sanity check
	/*std::cout<<"skeleton x_vel, y_vel: " << c->x_vel << ", " << c->y_vel
	   << std::endl;*/

	c->dstrect.x = c->x_pos-cameraRect.x;
	c->dstrect.y = c->y_pos-cameraRect.y;

	SDL_RenderCopyEx(gRenderer, c->sprites[c->spriteIndex], NULL, &(c->dstrect), 0.0, nullptr, c->flip);
}

// cameraRect is passed in so we know if we need to draw or not
void drawWeapon(std::vector<Weapon*> all_weapons, SDL_Rect cameraRect){
	for (auto w : all_weapons){
		//only draw if weapon is currently on the ground
		if (!w->picked_up) {
			w->dstrect.x = w->x_pos-cameraRect.x;
			w->dstrect.y = w->y_pos-cameraRect.y;

			//switches which png to push to renderer, can easily be extended for more weapons
			switch(w->weapon_type) {
				case 1: //is a sword
					SDL_RenderCopyEx(gRenderer, w->sprites[0], NULL, &(w->dstrect), 0.0, nullptr, SDL_FLIP_NONE);
					break;
				case 2: //is an axe
					SDL_RenderCopyEx(gRenderer, w->sprites[0], NULL, &(w->dstrect), 0.0, nullptr, SDL_FLIP_NONE);
					break;
				default:	//defaults to stickman for now
					SDL_RenderCopyEx(gRenderer, loadImage("sprites/stick.png"), NULL, &(w->dstrect), 0.0, nullptr, SDL_FLIP_NONE);
			}
		}
	}
}

//mouse click pickup of weapon on floor, adds weapon to inventory
//only call if mouse was confirmed to have been left clicked
void checkWeaponPickup(std::vector<Weapon*> p_all_weapons, SDL_Event e, std::vector<Weapon*> p_inventory, SDL_Rect cameraBox){
	//declaring variables for the mouse click locations we will be tracking
	int mouseClickX = e.button.x;
	int mouseClickY = e.button.y;

	for (auto w : p_all_weapons){

			//continues immediately if this weapon is not on the floor
			if (w->picked_up) {
				continue;
			}

			int weapon_x = (int) w->x_pos - cameraBox.x;
			int weapon_y = (int) w->y_pos - cameraBox.y;

			//check to see if mouse click was within bounds of the weapon
			if (mouseClickX > weapon_x && mouseClickX < weapon_x + w->x_size && mouseClickY > weapon_y && mouseClickY < weapon_y + w->y_size) {
				p_inventory.push_back(w);
				w->picked_up = true;	//weapon is now picked up
				break;	//stops us from picking up more than one weapon at a time
			}
	}
}

void save_texture(const char* file_name, SDL_Renderer* renderer, SDL_Texture* texture) {
    SDL_Texture* target = SDL_GetRenderTarget(renderer);
    SDL_SetRenderTarget(renderer, texture);
    int width, height;
    SDL_QueryTexture(texture, NULL, NULL, &width, &height);
    SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    SDL_RenderReadPixels(renderer, NULL, surface->format->format, surface->pixels, surface->pitch);
    IMG_SavePNG(surface, file_name);
    SDL_FreeSurface(surface);
    SDL_SetRenderTarget(renderer, target);
}

// draw the map based on the 2d array of bits
void drawMap(int** bitArray){

	int arrayWidth = sizeof(bitArray)/sizeof(int*);
	int arrayHeight = sizeof(bitArray[0])/sizeof(int);

	SDL_Rect currTile = {0, 0, 100, 100};
	SDL_Texture * mTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1920, 1080);
	SDL_SetRenderTarget(gRenderer, mTexture);

	for(int y = 0; y < 20; y++){

		for(int x = 0; x < 20; x++){

			//SDL_RenderCopyEx(gRenderer, tiles[0], NULL, &currTile, 0.0, nullptr, SDL_FLIP_NONE);
			if(bitArray[x][y] == 2){
				//draw stairs
				SDL_RenderCopy(gRenderer, tiles[5], NULL, &currTile);
			}
			else{
				//draw floor
				SDL_RenderCopy(gRenderer, tiles[0], NULL, &currTile);
			}
			
			//SDL_RenderCopy(gRenderer, tiles[0], NULL, )
			//SDL_RenderPresent(gRenderer);
			currTile.x += 100; //tile width

		}

		currTile.y += 100; //tile height
		currTile.x = 0;

	}

	save_texture("testMapImg.png", gRenderer, mTexture);

	SDL_SetRenderTarget(gRenderer, NULL);
	//SDL_RenderPresent(gRenderer);

}


int main() {


	if (!init()) {
		std::cout << "Failed to initialize!" << std::endl;
		close();
		return 1;
	}
			
	//gamestate variables
	int gamestate = 0; //the current gamestate. 0 will be menu, 1 will be playing the game, 2 will be quit
	bool quit = false; //set to true if the game should be quit
	bool paused = false; //set to true if the game is paused
	
	//player dimensions
	constexpr int PLAYER_WIDTH = 80;
	constexpr int PLAYER_HEIGHT = 100;
	
	//camera box & thirds of camera
	SDL_Rect cameraRect = {750, 750, 1280, 720}; //setting the start coordinates for the map
	
	//x and y coordinates/velocity/deltav
	double x_coord = SCREEN_WIDTH/2 - BOX_WIDTH/2;//cameraRect.w/2;;//SCREEN_WIDTH/2;// - PLAYER_WIDTH/2;
	double y_coord = SCREEN_HEIGHT/2 - BOX_HEIGHT/2;//SCREEN_HEIGHT/2;// - (PLAYER_HEIGHT * 2);
	double x_vel = 0.0;
	double x_deltav = 0.0;
	double y_vel = 0.0;
	double y_deltav = 0.0;
	//SDL_Rect moveBox = {BG_WIDTH/2 - PLAYER_WIDTH/2, BG_HEIGHT/2 - PLAYER_HEIGHT/2, PLAYER_WIDTH, PLAYER_HEIGHT};
	//displaying the start menu
	gTex.push_back(loadImage("menuImages/mainmenu.png")); //load in the start menu image
	gTex.push_back(loadImage("menuImages/pausemenu.png")); //load in the start menu image
	SDL_RenderClear(gRenderer);
	SDL_RenderCopy(gRenderer, gTex[0], NULL, NULL);
	SDL_RenderPresent(gRenderer);
	SDL_Event e; //declaring the SDL event type we will be using to detect specific events


	while(gamestate == 0) { //while we are on the start menu
		//declaring variables for the mouse click locations we will be tracking
		int mouseClickX;
		int mouseClickY;
		while(SDL_PollEvent(&e)){
			if(e.type == SDL_MOUSEBUTTONDOWN){
				if(e.button.button == SDL_BUTTON_LEFT){ //if the event type is a left mouse click
					//obtain the clicks x and y coordinates
					mouseClickX = e.button.x;
					mouseClickY = e.button.y;
					//check if button clicked is start
					if( (mouseClickX > 390) && (mouseClickX < 890) && (mouseClickY > 432) && (mouseClickY < 532) ){
						gamestate = 1; //enter the game, since start was pressed
					}
					//check if button clicked is quit
					else if( (mouseClickX > 391) && (mouseClickX < 891) && (mouseClickY > 562) && (mouseClickY < 662) ){
						quit = true; //setting the quit boolean to true
						gamestate = 2; //gamestate for quit
					}
				}
			}
		}
	}
	//check if the game was quit from the menu
	if(quit == true){
		close();
		return 0;
	}
	//gTex.push_back(loadImage("scrolltest.png")); //loading the map png into the game
	//int mapNumber = 0; //starting map number
	//maps.push_back(loadImage("map1.png")); //first map
	//maps.push_back(loadImage("map2.png")); //second map

	int tileNum=0;	//Starting tile num;
	//Begins loading tiles (0 is meant for easier floors 4 for harder ones)
	tiles.push_back(loadImage("mapAssets/floorTileL0.png"));
	tiles.push_back(loadImage("mapAssets/floorTileL1.png"));
	tiles.push_back(loadImage("mapAssets/floorTileL2.png"));
	tiles.push_back(loadImage("mapAssets/floorTileL3.png"));
	tiles.push_back(loadImage("mapAssets/floorTileL4.png"));

	tiles.push_back(loadImage("mapAssets/stairs.png"));

	//generating and loading the map
	Floor* testRoom = new Floor(20,20); //create a new floor object
	drawMap(testRoom->getFloorMap());
	maps.push_back(loadImage("testMapImg.png"));
	int stairsXCoord = testRoom->getStairX();
	int stairsYCoord = testRoom->getStairY();
	//int mapNumber = 0;

	//variables for door/stairs on map
	//int mapxleft = 870;
	//int mapxright = 1100;
	//int mapytop = 270;
	//int mapybottom = 280;

	//loading the 4 different movement orientations for the character
	sprites.push_back(loadImage("sprites/mainCharacterfrontWithWeapon.png"));
	sprites.push_back(loadImage("sprites/mainCharacterBackWithWeapon.png"));
	sprites.push_back(loadImage("sprites/mainCharacterSidewaysLeftWithWeapon.png"));
	sprites.push_back(loadImage("sprites/swingStart.png"));
	sprites.push_back(loadImage("sprites/swingEnd.png"));
	sprites.push_back(loadImage("sprites/swingfront.png"));
	sprites.push_back(loadImage("sprites/swingleft.png"));
	sprites.push_back(loadImage("sprites/swingright.png"));
	sprites.push_back(loadImage("sprites/swingUpFronnt.png"));
	sprites.push_back(loadImage("sprites/sidewaysSwingUp.png"));
	sprites.push_back(loadImage("sprites/mainCharacterfrontWithWeaponEquipped.png"));
	sprites.push_back(loadImage("sprites/mainCharacterBackWithWeaponEquipped.png"));
	sprites.push_back(loadImage("sprites/leftWithSwordEquipped.png"));
	sprites.push_back(loadImage("sprites/swingUpFrontEquipped.png"));
	sprites.push_back(loadImage("sprites/swingFrontDownEquipped.png"));
	sprites.push_back(loadImage("sprites/swingStartEquipped.png"));
	sprites.push_back(loadImage("sprites/swingEndEquipped.png"));
	sprites.push_back(loadImage("sprites/sidewaysSwingUpEquipped.png"));
	sprites.push_back(loadImage("sprites/swingleftEquipped.png"));

	//The index in the sprite array of our current sprite direction
	int spriteIndex = 0;
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	bool gameon = true; //the game loop is now active

	//enemy variables/declarations
	Enemy* badGuy = new Enemy(30, 30, PLAYER_WIDTH, PLAYER_HEIGHT);
	badGuy->health = 30;
	badGuy->sprites.push_back(loadImage("sprites/skeleton.png"));
	badGuy->sprites.push_back(loadImage("sprites/skeleton_back.png"));
	badGuy->sprites.push_back(loadImage("sprites/skeleton_left.png"));
	badGuy->sprites.push_back(loadImage("sprites/skeleton_right.png"));

	Archer* kite = new Archer(30, 30, PLAYER_WIDTH, PLAYER_HEIGHT);
	kite->health = 30;
	kite->sprites.push_back(loadImage("sprites/skeleton.png"));
	kite->sprites.push_back(loadImage("sprites/skeleton_back.png"));
	kite->sprites.push_back(loadImage("sprites/skeleton_left.png"));
	kite->sprites.push_back(loadImage("sprites/skeleton_right.png"));


	double enemyPos = 0;
	bool arrived = false;
	bool enemyFollow = false;
	bool enemyAlive = true;
	//attack animation variables
    bool attack = false;
    int frames = 0;

	//define a weapon to be on floor
	//setting up tester weapons currently on floor
	Weapon *sword;
	Weapon *axe;
	axe = new Weapon(310.0, 310.0, 75, 75, 2);
	sword = new Weapon(110.0, 110.0, 60, 60, 1);
	axe->sprites.push_back(loadImage("sprites/axe.png"));
	sword->sprites.push_back(loadImage("sprites/sword.png"));
	all_weapons.push_back(sword);
	all_weapons.push_back(axe);
  
	// Keep track of time for framerate/movements
	Uint32 fps_last_time = SDL_GetTicks();
	Uint32 fps_cur_time = 0;
	Uint32 move_last_time = SDL_GetTicks();
	//Uint32 anim_last_time = SDL_GetTicks();
	double timestep = 0.0;
	
	//dst rect for containing player display
	SDL_Rect dstrect = { static_cast<int>(x_coord), static_cast<int>(y_coord), PLAYER_WIDTH, PLAYER_HEIGHT }; //dstrect for the main character
	
	// creating player object
	Player *p;
	p = new Player(x_coord, y_coord, PLAYER_WIDTH, PLAYER_HEIGHT);

	//MAIN GAME LOOP
	while(gameon)
	{	
		
		//first check for quit or pause
		while(SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				gameon = false;
			}else if(e.type == SDL_KEYDOWN){
				if(e.key.keysym.sym == SDLK_p)
					paused = true;
			}
			if(e.type == SDL_MOUSEBUTTONDOWN){
		         if(e.button.button == SDL_BUTTON_LEFT){
							 			checkWeaponPickup(all_weapons, e, inventory, cameraRect);


		                attack = true;
		                //check if enemy is in range (In progress)
		               /* SDL_Rect playRec;
		                SDL_Rect badRec;
		                playRec.x=x_pos-100;
		                playRec.y=y_pos-100;
		                playRec.w=x_size+100;
		                playRec.h=y_size+100;

		                badRec.x=badGuy->x_pos;
		                badRec.y=badGuy->y_pos;
		                badRec.w=badGuy->x_size;
		                badRec.h=badGuy->y_size;

		                if(check_collision(playRec,badRec)) */
		                if (enemyFollow)
		                	badGuy->hit();
		                //erase enemy if health depleted
		                if (badGuy->health <= 0)
		                {
		                	SDL_DestroyTexture(badGuy->sprites[badGuy->spriteIndex]);
		                	enemyAlive=false;
		                }
		          }
		    }
		}
		
		//grab timestep since last ticks - need this before pause menu or else issues arise with timestep calculations
		timestep = ((SDL_GetTicks() - move_last_time) / 1000.0);
		
		//pause menu is rendered and waits for key press
		while(paused){			
			//gTex.push_back(loadImage("menuImages/pausemenu.png")); //load in the start menu image
			//SDL_RenderClear(gRenderer);
			SDL_RenderCopy(gRenderer, gTex[1], NULL, NULL);
			SDL_RenderPresent(gRenderer);
			//allows unpausing with p press
			// while(SDL_PollEvent(&e)){
			// 	if(e.type == SDL_KEYDOWN){
			// 		if(e.key.keysym.sym == SDLK_p){
			// 			paused = false;
			// 		}
			// 	}
			// }
			int mouseClickX;
			int mouseClickY;
			while(SDL_PollEvent(&e)){
				if(e.type == SDL_MOUSEBUTTONDOWN){
					if(e.button.button == SDL_BUTTON_LEFT){ //if the event type is a left mouse click
						//obtain the clicks x and y coordinates
						mouseClickX = e.button.x;
						mouseClickY = e.button.y;
						// SDL_Log(mouseClickX);
						// SDL_Log(mouseClickY);

						//check if button clicked is resume
						if( (mouseClickX > 435) && (mouseClickX < 588) && (mouseClickY > 245) && (mouseClickY < 280) ){
							gamestate = 1; //enter the game, since start was pressed
							paused = false;
						}

						//check if button clicked is save
						// else if( (mouseClickX > 391) && (mouseClickX < 891) && (mouseClickY > 562) && (mouseClickY < 662) ){

						// }

						//check if button clicked is quit
						else if( (mouseClickX > 435) && (mouseClickX < 532) && (mouseClickY > 349) && (mouseClickY < 379) ){
							quit = true; //setting the quit boolean to true
							gamestate = 2; //gamestate for quit
						}
					}
				}
			}
			if(quit == true){
				close();
				return 0;
			}
		}
		
		const Uint8* keystate = SDL_GetKeyboardState(nullptr);

		p->moveCharacter(p->x_pos, p->y_pos, BG_HEIGHT, BG_WIDTH, timestep);
		
		//update camera rect
		cameraRect.x = (p->x_pos + PLAYER_WIDTH/2) - SCREEN_WIDTH/2;
		if (cameraRect.x < 0)
			cameraRect.x = 0;
		else if (cameraRect.x + SCREEN_WIDTH > BG_WIDTH)
			cameraRect.x = BG_WIDTH - SCREEN_WIDTH;
		cameraRect.y = (p->y_pos + PLAYER_HEIGHT/2) - SCREEN_HEIGHT/2;
		if (cameraRect.y < 0)
			cameraRect.y = 0;
		else if (cameraRect.y + SCREEN_HEIGHT > BG_HEIGHT)
			cameraRect.y = BG_HEIGHT - SCREEN_HEIGHT;
		
		//grab last time player coordinates moved
		move_last_time = SDL_GetTicks();
		
		
		//switching the character orientation based on direction
		if(p->x_vel < 0){
			spriteIndex=2;
			flip = SDL_FLIP_NONE;
		}
		else if(p->x_vel > 0){
			spriteIndex=2;
			flip = SDL_FLIP_HORIZONTAL;
		}
		else if(p->y_vel < 0){
			spriteIndex=1;
			flip = SDL_FLIP_NONE;
		}
		else if(p->y_vel > 0){
			spriteIndex=0;
			flip = SDL_FLIP_NONE;
		}
		
		
		dstrect.x = p->x_pos - cameraRect.x;
		dstrect.y = p->y_pos - cameraRect.y;
		/*
		//now figure out if the character has entered the door
		if(p->y_pos > mapytop && p->y_pos < mapybottom){
			if(p->x_pos > mapxleft && p->x_pos < mapxright){
				//increase map number
				mapNumber++;
				//also change player location to corner position
				p->x_pos = 100;
				p->y_pos = 100;
				//and change new door location
				mapxleft = 870;
				mapxright = 970;
				mapytop = 800;
				mapybottom = 810;

				SDL_Delay(1000);
			}
		}

		if(mapNumber == 2){
			//out of maps, quit the game
			SDL_Delay(1000); //delay for 1 second
			break; //break out of game loop and quit
		}
		*/

		//drawing everything to the screen
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(gRenderer);
		// draw the current map on the screen
		SDL_RenderCopy(gRenderer, maps[0], &cameraRect, NULL);
		// draw player


		//determine if the player entered the stairs

		//stairsXCoord
		//stairsYCoord

		
		//move the enemy around the map - top corners for now
		if(enemyAlive)
		{
			if(enemyPos == 0 && !enemyFollow)
			{
				arrived = badGuy->enemySeek(1500 , 250, BG_HEIGHT, BG_WIDTH, timestep);
				//if enemy arrived at destination, change course
				if(arrived)
					enemyPos++;
			}
			else if(enemyPos == 1 && !enemyFollow)
			{
				arrived = badGuy->enemySeek(1500 , 900, BG_HEIGHT, BG_WIDTH, timestep);
				//if enemy arrived at destination, change course
				if(arrived)
					enemyPos++;
			}
			else if(enemyPos == 2 && !enemyFollow)
			{
				arrived = badGuy->enemySeek(250 , 900, BG_HEIGHT, BG_WIDTH, timestep);
				//if enemy arrived at destination, change course
				if(arrived)
					enemyPos++;
			}
			else if(enemyPos == 3 && !enemyFollow)
			{
				arrived = badGuy->enemySeek(250 , 250, BG_HEIGHT, BG_WIDTH, timestep);
				//if enemy arrived at destination, change course
				if(arrived)
					enemyPos = 0;;
			}

			
			//if spacebar is pressed, enemy will chase player - TESTING PURPOSES
			if (keystate[SDL_SCANCODE_SPACE])
				enemyFollow = true;
			
			//as long as enemy should chase, update destination locataions/check if reached
			if(enemyFollow)
			{
				arrived = badGuy->enemyFollow(p->x_pos, p->y_pos, BG_HEIGHT, BG_WIDTH, timestep);
				if(arrived)
					enemyFollow = false;
			}
		}

		//draw the enemy and weapon
		drawEnemy(badGuy, cameraRect);
		drawWeapon(all_weapons, cameraRect);
		
		//animation for attacking
		    int temp = spriteIndex;
		          if(frames <= 60 && attack){
				            switch(spriteIndex){
							case 0:
				 			if(weaponPickedUp){
				 				spriteIndex = 13;
							frames++;
							}else{
								spriteIndex = 8;
					            frames++;
							}
							break;
							case 1:
							if(weaponPickedUp){
							spriteIndex = 15;
							frames++;
							}
							else{
							spriteIndex = 3;
				            frames++;
							}
							break;
							case 2:
							if(weaponPickedUp){
							spriteIndex = 17;
							frames++;
							}
							else{
							spriteIndex = 9;
				            frames++;
				        	}
							break;
							default:
							break;
							}
		            }
		            else if(frames <= 120 && attack){
				            switch(spriteIndex){
							case 0:
							if(weaponPickedUp){
								spriteIndex = 14;
				            frames++;
							}
							else{
								spriteIndex = 5;
				            frames++;
							}
							break;
							case 1:
							if(weaponPickedUp){
							spriteIndex = 16;
				            frames++;
							}
							else{
							spriteIndex = 4;
				            frames++;
				        	}
							break;
							case 2:
							if(weaponPickedUp){
							spriteIndex = 18;
				            frames++;
							}
							else{
							spriteIndex = 6;
				            frames++;
				        	}
				            break;
							default:
							break;
							}
		            }
		            else if(frames != 0 && attack)
		            {
		                frames = 0;
		                attack = false;
		            }
    
		    for(auto w: all_weapons){
		    	if(w->picked_up){
		    		weaponPickedUp = true;
		    		switch(spriteIndex){
						case 0:
						spriteIndex = 10;
						break;
						case 1:
						spriteIndex = 11;
						break;
						case 2:
						spriteIndex = 12;
						break;
						}
		    	}
		    }

			kite->moveCharacter(p->x_pos, p->y_pos, BG_HEIGHT, BG_WIDTH, timestep);


			drawEnemy(kite, cameraRect);
    
			SDL_RenderCopyEx(gRenderer, sprites[spriteIndex], NULL, &dstrect, 0.0, nullptr, flip);
			SDL_RenderPresent(gRenderer);
			spriteIndex = temp;

			//determine if the player entered the stairs
			if(p->x_pos > stairsXCoord && p->x_pos < (stairsXCoord + 100) && p->y_pos > stairsYCoord && p->y_pos < (stairsYCoord + 100)){
				//we have an intersection with the stairs
				//so we need to regenerate the map
				delete testRoom;
				testRoom = new Floor(20,20); //create a new floor object
				maps.clear(); //clear current map out of array
				drawMap(testRoom->getFloorMap());
				maps.push_back(loadImage("testMapImg.png"));
				stairsXCoord = testRoom->getStairX();
				stairsYCoord = testRoom->getStairY();
				SDL_Delay(500);

			}

	}

	//bool to activate credits & if statement for it
	bool credits = false;
	if(credits)
	{

				// Load media
			gTex.push_back(loadImage("credits/ConnorAllington.png"));
			gTex.push_back(loadImage("credits/TristinButz.png"));
			gTex.push_back(loadImage("credits/bickerstaff.png"));
			gTex.push_back(loadImage("credits/SebastianMarcano.png"));
			gTex.push_back(loadImage("credits/DestinyThompson.png"));
			gTex.push_back(loadImage("credits/ZachStata.png"));
			gTex.push_back(loadImage("credits/AndrewPreston.png"));
			gTex.push_back(loadImage("credits/AdamMattioli.png"));
			gTex.push_back(loadImage("credits/BryceRyan.png"));
			gTex.push_back(loadImage("credits/tristanpossessky.png"));
			gTex.push_back(loadImage("credits/MConlon.png"));
			SDL_RenderClear(gRenderer);
			//Add this chunk of code for each image - start
			// Render the image- change gTex[i] for i = image number
			SDL_RenderCopy(gRenderer, gTex[1], NULL, NULL);
			// Display rendering
			SDL_RenderPresent(gRenderer);
			// Wait 3 seconds
			SDL_Delay(3000);
			//************************************* - end
			//Add this chunk of code for each image - start
			// Render the image
			SDL_RenderCopy(gRenderer, gTex[2], NULL, NULL);
			// Display rendering
			SDL_RenderPresent(gRenderer);
			// Wait 3 seconds
			SDL_Delay(3000);
			//************************************* - end
			//Add this chunk of code for each image - start
			// Render the image
			SDL_RenderCopy(gRenderer, gTex[3], NULL, NULL);
			// Display rendering
			SDL_RenderPresent(gRenderer);
			// Wait 3 seconds
			SDL_Delay(3000);
			//************************************* - end
			//Add this chunk of code for each image - start
			// Render the image
			SDL_RenderCopy(gRenderer, gTex[4], NULL, NULL);
			// Display rendering
			SDL_RenderPresent(gRenderer);
			// Wait 3 seconds
			SDL_Delay(3000);
			//************************************* - end
			//Add this chunk of code for each image - start
			// Render the image
			SDL_RenderCopy(gRenderer, gTex[5], NULL, NULL);
			// Display rendering
			SDL_RenderPresent(gRenderer);
			// Wait 3 seconds
			SDL_Delay(3000);
			//************************************* - end
			//Add this chunk of code for each image - start
			// Render the image- change gTex[i] for i = image number
			SDL_RenderCopy(gRenderer, gTex[6], NULL, NULL);
			// Display rendering
			SDL_RenderPresent(gRenderer);
			// Wait 3 seconds
			SDL_Delay(3000);
			//************************************* - end
			//Add this chunk of code for each image - start
			// Render the image- change gTex[i] for i = image number
			SDL_RenderCopy(gRenderer, gTex[7], NULL, NULL);
			// Display rendering
			SDL_RenderPresent(gRenderer);
			// Wait 3 seconds
			SDL_Delay(3000);
			//************************************* - end
			//Add this chunk of code for each image - start
			// Render the image- change gTex[i] for i = image number
			SDL_RenderCopy(gRenderer, gTex[8], NULL, NULL);
			// Display rendering
			SDL_RenderPresent(gRenderer);
			// Wait 3 seconds
			SDL_Delay(3000);
			//************************************* - end
			//Add this chunk of code for each image - start
			// Render the image- change gTex[i] for i = image number
			SDL_RenderCopy(gRenderer, gTex[9], NULL, NULL);
			// Display rendering
			SDL_RenderPresent(gRenderer);
			// Wait 3 seconds
			SDL_Delay(3000);
			//************************************* - end
			//Add this chunk of code for each image - start
			// Render the image- change gTex[i] for i = image number
			SDL_RenderCopy(gRenderer, gTex[10], NULL, NULL);
			// Display rendering
			SDL_RenderPresent(gRenderer);
			// Wait 3 seconds
			SDL_Delay(3000);
			//************************************* - end
			//Add this chunk of code for each image - start
			// Render the image- change gTex[i] for i = image number
			SDL_RenderCopy(gRenderer, gTex[11], NULL, NULL);
			// Display rendering
			SDL_RenderPresent(gRenderer);
			// Wait 3 seconds
			SDL_Delay(3000);
			//************************************* - end
	}
	close();
	return 0;
}
