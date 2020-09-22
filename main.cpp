#define SDL_MAIN_HANDLED
#include <iostream>
#include <vector>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;
constexpr int BOX_WIDTH = 80;
constexpr int BOX_HEIGHT = 100;
constexpr int BG_WIDTH = 1920;	// temporary dimensions for scrolltest.png
constexpr int BG_HEIGHT = 1080;
// Function declarations
bool init();
SDL_Texture* loadImage(std::string fname);
void close();
// Globals
SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
std::vector<SDL_Texture*> gTex;
std::vector<SDL_Texture*> sprites;
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
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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
int main() {
	if (!init()) {
		std::cout << "Failed to initialize!" << std::endl;
		close();
		return 1;
	}
	int gamestate = 0; //the current gamestate. 0 will be menu, 1 will be playing the game, 2 will be quit
	bool quit = false; //set to true if the game should be quit
	bool paused = false; //set to true if the game is paused
	SDL_Rect cameraRect = {750, 750, 1280, 720}; //setting the start coordinates for the map
	constexpr int PLAYER_WIDTH = 80;
	constexpr int PLAYER_HEIGHT = 100;
	SDL_Rect moveBox = {BG_WIDTH/2 - PLAYER_WIDTH/2, BG_HEIGHT/2 - PLAYER_HEIGHT/2, PLAYER_WIDTH, PLAYER_HEIGHT};
	//displaying the start menu
	gTex.push_back(loadImage("menuImages/mainmenu.png")); //load in the start menu image
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
	gTex.push_back(loadImage("scrolltest.png")); //loading the map png into the game
	int x_pos = SCREEN_WIDTH/2 - BOX_WIDTH/2;
	int y_pos = SCREEN_HEIGHT/2 - BOX_HEIGHT/2;
	int x_size = 80;
	int y_size = 100;
	//define x and y velocities and delta for smooth movements
	int x_vel = 0;
	int y_vel = 0;
	int x_deltav = 0;
	int y_deltav = 0;
	//loading the 4 different movement orientations for the character
	sprites.push_back(loadImage("sprites/mainCharacterfrontAlternate.png"));
	sprites.push_back(loadImage("sprites/mainCharacterBackWithoutWeapon.png"));
	sprites.push_back(loadImage("sprites/mainCharacterSidewaysLeftWithoutWeapon.png"));
	//The index in the sprite array of our current sprite direction
	int spriteIndex = 0;
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	bool gameon = true; //the game loop is now active
	while(gameon) 
	{
		SDL_Rect dstrect = { x_pos, y_pos, x_size, y_size }; //dstrect for the main character
		//first check for quit or pause
		while(SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				gameon = false;
			}else if(e.type == SDL_KEYDOWN){
				if(e.key.keysym.sym == SDLK_p)
					paused = true;
			}
		}
		//pause menu is rendered and waits for key press
		//pause menu is rendered and waits for key press
								while(paused){
									gTex.push_back(loadImage("menuImages/pausemenu.png")); //load in the start menu image
									//SDL_RenderClear(gRenderer);
									SDL_RenderCopy(gRenderer, gTex[2], NULL, NULL);
									SDL_RenderPresent(gRenderer);
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
		//camera velos
		//x_vel = 0;
		//y_vel = 0;
		//delta x and y for camera/character
		x_deltav = 0;
		y_deltav = 0;
		//detecting key states for movement
		const Uint8* keystate = SDL_GetKeyboardState(nullptr);
		if (keystate[SDL_SCANCODE_W])
			y_deltav -= 1;
		if (keystate[SDL_SCANCODE_A])
			x_deltav -= 1;
		if (keystate[SDL_SCANCODE_S])
			y_deltav += 1;
		if (keystate[SDL_SCANCODE_D])
			x_deltav += 1;
		//check if delta_v = 0 (no key pressed), if so set to -1 to decrement velocity
		if(x_deltav == 0)
		{
			if(x_vel > 0)
				x_deltav = -1;
			else if(x_vel < 0)
				x_deltav = 1;
		}
		if(y_deltav == 0)
		{
			if(y_vel > 0)
				y_deltav = -1;
			else if(y_vel < 0)
				y_deltav = 1;
		}
		//increment velocities
		x_vel += x_deltav;
		y_vel += y_deltav;
		//speed limiting
		if(x_vel < -5)
			x_vel = -5;
		else if(x_vel > 5)
			x_vel = 5;
		if(y_vel < -5)
			y_vel = -5;
		else if(y_vel > 5)
			y_vel = 5;
		// move the moveBox
		moveBox.y += y_vel;
		if (moveBox.y < 0 || (moveBox.y + PLAYER_HEIGHT > BG_HEIGHT))
			moveBox.y -= y_vel;
		moveBox.x += x_vel;
		if (moveBox.x < 0 || (moveBox.x + PLAYER_WIDTH > BG_WIDTH))
			moveBox.x -= x_vel;
		// make sure player is centered in camera
		cameraRect.x = (moveBox.x + moveBox.w/2) - SCREEN_WIDTH/2;
		if (cameraRect.x < 0)
			cameraRect.x = 0;
		else if (cameraRect.x + SCREEN_WIDTH > BG_WIDTH)
			cameraRect.x = BG_WIDTH - SCREEN_WIDTH;
		cameraRect.y = (moveBox.y + moveBox.h/2) - SCREEN_HEIGHT/2;
		if (cameraRect.y < 0)
			cameraRect.y = 0;
		else if (cameraRect.y + SCREEN_HEIGHT > BG_HEIGHT)
			cameraRect.y = BG_HEIGHT - SCREEN_HEIGHT;
		//switching the character orientation based on direction
		if(x_vel < 0){
			spriteIndex=2;
			flip = SDL_FLIP_NONE;
		}
		else if(x_vel > 0){
			spriteIndex=2;
			flip = SDL_FLIP_HORIZONTAL;
		}
		else if(y_vel < 0){
			spriteIndex=1;
			flip = SDL_FLIP_NONE;
		}
		else if(y_vel > 0){
			spriteIndex=0;
			flip = SDL_FLIP_NONE;
		}
			//drawing everything to the screen
			SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderClear(gRenderer);
			// draw portion of background currently highlighted by camera
			SDL_RenderCopy(gRenderer, gTex[1], &cameraRect, NULL);
			// draw player
			dstrect.x = moveBox.x - cameraRect.x;
			dstrect.y = moveBox.y - cameraRect.y;
			SDL_RenderCopyEx(gRenderer, sprites[spriteIndex], NULL, &dstrect, 0.0, nullptr, flip);
			SDL_RenderPresent(gRenderer);
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