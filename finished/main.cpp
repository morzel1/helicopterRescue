/*
 * William Blair
 * CS370
 * 02/16/17
 * main.cpp - skeleton for
 * Helicopter Rescue Game */

////////////////////////////////////
//     HEADER FILES / INCLUDES    //
////////////////////////////////////
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <stdlib.h>
#include <unistd.h> // for parsing args
//#include <iostream>
#include <vector>

// bug with mingw versions where std::to_string doesn't exist

#include <string>
#include <sstream>

namespace patch
{
	template < typename T > std::string to_string( const T& n )
	{
		std::ostringstream stm ;
		stm << n ;
		return stm.str() ;
	}
}

#include <iostream>

#include "Sprite.h"

/* MiniAT includes */
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <stdint.h>
#include <inttypes.h>

#include <miniat.h>
#include "peripherals.h"
#include "ports.h"

/* define from robots example code */
#define MAX_CYCLES UINT64_MAX

/* temporary windows fix for bj compiling */
#ifdef __WIN32
	#undef main
#endif

////////////////////////////////////
//      DEFINES / CONSTANTS       //
////////////////////////////////////

const int S_WIDTH  = 1024; // window dimensions (1024)
const int S_HEIGHT = 768;   //786

const Uint8 *keyState = NULL;

const int BG_SCROLL_SPEED =  2;
const int SCROLL_SPEED = 3;

const int NUM_BUILDINGS = 20;

/* BJ SWITCHED - TESTING */
const int FPS = 60;


const char title[] = "Heli Rescue"; // title of the window
const int MAX_THRUST = 100; // the max miniat function value
                            // for thrust we're planning to use

const double MAX_VEL  = 5.0;  // the most amount of pixels the sprite can move per frame

const int INVINCIBILITY_FRAMES = 150; // how long the player has after they hit an object
                                      // until they lose another life

/////////////////////////////////////
//       FUNCTION DECLARATIONS     //
/////////////////////////////////////

bool initSDL(void);
bool createWindow(SDL_Window **w);

void calcVelocity(Sprite &heli, int thrustX, int thrustY, double &vX, double &vY);
bool parseArgs(miniat **m, bool &skipScreens, int argc, char *argv[]);

/* BJ added 04/08/17 - Untested */
bool check_building_collision(Sprite &heli, unsigned int &lives, Sprite *objarray, int objarray_size);

void check_victim_collision(Sprite &heli, std::vector<Sprite> &victims, unsigned int &saved_victims, int &score, Mix_Chunk *sperson);

/* modified from robots example */
static void cleanMiniAT(miniat **m);
static void signal_handler(int sig);

/* mike's scrolling background */
const int cameraWidth = 1024;
const int cameraHeight = 768;

SDL_Rect cameraRect = {0,0,cameraWidth,cameraHeight};

//This block is what gives the value on line 55 its value
SDL_Texture *LoadTexture(std::string filePath, SDL_Renderer *renderTarget){
    SDL_Texture *texture = NULL;
    SDL_Surface *surface = SDL_LoadBMP(filePath.c_str());
    if(surface == NULL){
        std::cout << "error" << std::endl;
    }
    else{
        texture = SDL_CreateTextureFromSurface(renderTarget,surface);
        if(texture==NULL){
        std::cout << "error" << std::endl;
    }
    SDL_FreeSurface(surface);
    return texture;
	}
}

bool drawText(TTF_Font *font, SDL_Renderer *r, const char *message, unsigned int x, unsigned int y)
{
	SDL_Surface *textSurface = NULL; // the rendered text
	SDL_Color color = {255, 255, 255}; // just draw red for now

	/* render the text */
	textSurface = TTF_RenderText_Solid(font, message, color);
	/* error check */
	if( !textSurface ){
		std::cerr << "Failed to render text: " << TTF_GetError() << std::endl;
		return false;
	}

	/* create an sdl texture from the surface */
	SDL_Texture *t = SDL_CreateTextureFromSurface(r, textSurface);
	if( !t ){
		std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
		return false;
	}

	/* hold the position of the text */
	SDL_Rect rect;
	rect.x = x; rect.y = y;
	rect.w = textSurface->w; rect.h = textSurface->h;

	/* draw the text to the screen */
	SDL_RenderCopy(r,t,NULL,&rect); //copies new render

	/* free memory */
	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(t);

	return true;
}

//////////////////////////////////////
//              MAIN                //
//////////////////////////////////////
int main(int argc, char *argv[])
{
	//VARIABLES FOR ALTERNATE SCREEN
    int EndingChoice = 0;
    bool smallLoop = true;
    bool BigGame = true;
    
    /* miniat variables */
    miniat *m       = NULL; // the main 'miniat' object
	int cycles      = 0;    // the number of cycles the miniat has run
    
    bool skipScreens = false; // true if the user wanted to not have
							  // start screen
    
    /* BJ added - initialize miniat */
	if( !parseArgs(&m, skipScreens, argc, argv) ){
		return -1;
	}

	/* initialze SDL, IMG, and TTF Libraries
		 * quit the program on failure*/
	if( !initSDL() ){
		return -1;
	}

    while(BigGame)
    {
		SDL_Event event;
		keyState = SDL_GetKeyboardState(NULL);
 
        SDL_Rect cameraRect = {0,0,cameraWidth,cameraHeight};

		/////////////////////////////////
		//       LOCAL VARIABLES       //
		/////////////////////////////////
		SDL_Window   *window   = NULL;  // the application window
		SDL_Renderer *renderer = NULL;  // renders the screen
		TTF_Font     *font     = NULL;  // holds the loaded ttf font
		
		SDL_Texture *texture   = NULL;  // Mike's BG
		
		Mix_Music    *bgmusic  = NULL; // background music to play during the level
		Mix_Music    *tmusic   = NULL; // music during the title screen
		Mix_Chunk    *xplode   = NULL; // plays when an explosion happens
		Mix_Chunk    *sperson  = NULL; // plays when you rescue a victim
		
		//Create x and y location arrays.
		int victimLocY[5];
		int victimLocX[5];

		/*Nested for loop to "randomly generate"
		x and y coordinates for each
		victim to spawn at
		*/
		for(int i = 0; i <5; i++)
		{
			for(int j=0; j<5; j++)
			{
				victimLocX[i] = rand() % 1025 - 23;
				victimLocY[j] = rand() % 769 - 55;
			}
		}

		bool isRunning = true;          // false when the game exits
		//SDL_Event event;                // holds events like keypresses, etc.

		Sprite heli; // use the no args constructor since no libraries have been initted
		unsigned int lives = 5; // how many freebies the helicopter has
		int score = 0;
		unsigned int saved_victims = 0; // how many victims the helicopter has picked up
		int    thrustX = 0; // the thrust to apply to the helicopter
		int    thrustY = 0;
		double    vX      = 0; // the helicopter's velocity
		double    vY      = 0;
		int       invince_timer = 0; // countdown until end of invincibility frames

		Sprite explosion; // to be drawn when the helicopter hits a building
		int    explosionCounter = 100; // so the explosion only gets drawn when the helicopter first hits the building

		Sprite pause;     // the pause screen to be drawn when the game is frozen

		Sprite objarray[NUM_BUILDINGS]; //amount of building generated


		/* load the font to draw text with */
		std::string fontChar = "fonts/Ubuntu-R.ttf";
		font = TTF_OpenFont(fontChar.c_str(), 28); // font size is 18
		if( !font ){
			std::cerr << "Failed to open " << fontChar << std::endl;
			std::cerr << TTF_GetError() << std::endl;
			IMG_Quit();
			TTF_Quit();
			Mix_CloseAudio();
			Mix_Quit();
			SDL_Quit();
			return -1;
		}

		/* create the window for the application */
		if( !createWindow(&window) ){
			TTF_CloseFont(font);
			IMG_Quit();
			TTF_Quit();
			Mix_CloseAudio();
			Mix_Quit();
			SDL_Quit();
			return -1;
		}


		/* create the renderer for the window */
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		if( !renderer ) {
			std::cerr << "Failed to create renderer: " << SDL_GetError()
													   << std::endl;
			TTF_CloseFont(font);
			SDL_DestroyWindow(window);
			IMG_Quit();
			TTF_Quit();
			Mix_CloseAudio();
			Mix_Quit();
			SDL_Quit();
			return -1;
		}
		
		/* load and play music */
		bgmusic = Mix_LoadMUS("music/final1.wav");
		tmusic  = Mix_LoadMUS("music/titlescreen.wav");
		if(!bgmusic || !tmusic){
			std::cerr << "Failed to load music: " << Mix_GetError()
												  << std::endl;
		}
		/*else{
			if( Mix_PlayMusic(bgmusic, -1) == -1 ){
				std::cerr << "Failed to play music: " << Mix_GetError()
													  << std::endl;
			}
		}*/
		
		/* load the sound effect music */
		xplode = Mix_LoadWAV("music/explosion_quiet2.wav");
		if( !xplode ){
			std::cerr << "Failed to load sound effect: " << Mix_GetError()
														 << std::endl;
		}
		
		/* load the save sound effect */
		sperson = Mix_LoadWAV("music/save_personlouder.wav");
		if( !sperson ){
			std::cerr << "Failed to load sound effect: " << Mix_GetError()
			                                             << std::endl;
		}

		/* load the background for the text */
		Sprite textBackground0;
		Sprite textBackground1;
		if( !textBackground0.load("images/text_background_2.png", renderer) ||
			!textBackground1.load("images/text_background_2.png", renderer)
		  ){
			TTF_CloseFont(font);
			IMG_Quit();
			TTF_Quit();
			if( bgmusic ) Mix_FreeMusic(bgmusic);
			if( xplode )  Mix_FreeChunk(xplode);
			if( sperson )  Mix_FreeChunk(sperson);
			Mix_CloseAudio();
			Mix_Quit();
			SDL_Quit();
			return -1;
		}

		/* set the location of the text backgrounds */
		textBackground0.setX(0);
		textBackground0.setY(0);
		textBackground1.setX(S_WIDTH - textBackground1.getW());
		textBackground1.setY(0);

		/* Mike's scrolling background code */
		//SDL_Texture *texture = LoadTexture("backgroundv2.bmp", renderer);
		texture = LoadTexture("images/backgroundv2.bmp", renderer);
		
		/* what color to clear the background */
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);

		/* create heli sprite */
		// edited for presentation
		//if( !heli.load("helicopter2.png", renderer) ){
		if( !heli.load("images/helicopter2-spritesheet.png", renderer, 4, 5 )){
			TTF_CloseFont(font);
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			IMG_Quit();
			TTF_Quit();
			if( bgmusic ) Mix_FreeMusic(bgmusic);
			if( xplode )  Mix_FreeChunk( xplode );
			if( sperson )  Mix_FreeChunk(sperson);
			Mix_CloseAudio();
			Mix_Quit();
			SDL_Quit();
		}

		/* create the explosion sprite */
		if( !explosion.load("images/explosion.png", renderer, 9, 5)){
			TTF_CloseFont(font);
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			IMG_Quit();
			TTF_Quit();
			if(bgmusic) Mix_FreeMusic(bgmusic);
			if(xplode)  Mix_FreeChunk(xplode);
			if( sperson )  Mix_FreeChunk(sperson);
			Mix_CloseAudio();
			Mix_Quit();
			SDL_Quit();
		}
		explosion.setW(0);
		explosion.setH(0);
		
		/* create the pause screen sprite */
		if( !pause.load("images/pause.png", renderer)){
			TTF_CloseFont(font);
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			IMG_Quit();
			TTF_Quit();
			if(bgmusic) Mix_FreeMusic(bgmusic);
			if(xplode)  Mix_FreeChunk(xplode);
			if( sperson )  Mix_FreeChunk(sperson);
			Mix_CloseAudio();
			Mix_Quit();
			SDL_Quit();
		}
		pause.setX(0);
		pause.setY(0);

		Sprite extra; // space holder for failed to render error...
		std::vector<Sprite> victimVec;

		for(int i=0; i<NUM_BUILDINGS; i++){
			victimVec.push_back(*(new Sprite));
		}

		/* added for workaround where last sprite would fail to render
		 * this is just a placeholder that isn't drawn */
		victimVec.push_back(extra);
		
		/* set the heli position */
		heli.setX(50);
		/* change me back afterwards!*/
		heli.setY(S_HEIGHT - heli.getH());

		int locx = 350;
		int locy = 0;
		int upordown = 0;   //will building be coming from up or down
		int bigorsmall = 0; //will gap be big or small
		int whichbuilding = 0; //what building sprite to use
		int locationx [NUM_BUILDINGS]; //holds x locations
		int locationy [NUM_BUILDINGS]; //holds y locations

		/* set the location of building objects */
		/* ADD ME BACK IN WHEN NOT TESTING!!!!! */
		srand(time(0));
		/****************************************/
		for(int i = 0; i<NUM_BUILDINGS; i++){
			upordown = (rand()%2); //generates random number 1 is up, 0 is down
			if (upordown == 1){
				whichbuilding=(rand()%4);
				if (whichbuilding == 0) {
					objarray[i].load("images/108pxbuildingflip.png", renderer);
						locy = 0;
						victimVec[i].load("images/TopVictim.png", renderer);
						victimVec[i].setY(objarray[i].getH());
				}
				if (whichbuilding == 1) {
					objarray[i].load("images/124pxbuildingflip.png", renderer);
						locy = 0;
						victimVec[i].load("images/TopVictim.png", renderer);
						victimVec[i].setY(objarray[i].getH());
				}
				if (whichbuilding == 2) {
					objarray[i].load("images/241pxbuildingflip.png", renderer);
						locy = 0;
						victimVec[i].load("images/TopVictim.png", renderer);
						victimVec[i].setY(objarray[i].getH());
				}
				if (whichbuilding == 3) {
					objarray[i].load("images/254pxbuildingflip.png", renderer);
						locy = 0;
						victimVec[i].load("images/TopVictim.png", renderer);
						victimVec[i].setY(objarray[i].getH());
				}
			}
			else {
				whichbuilding = (rand()%4);
				if (whichbuilding == 0) {
					objarray[i].load("images/108pxbuilding.png", renderer);
						locy = 568;
						victimVec[i].load("images/BottomVictim.png", renderer);
						victimVec[i].setY(locy - victimVec[i].getH());
				}
				if (whichbuilding == 1) {
					objarray[i].load("images/124pxbuilding.png", renderer);
						locy = 538;
						victimVec[i].load("images/BottomVictim.png", renderer);
						victimVec[i].setY(locy - victimVec[i].getH());
				}
				if (whichbuilding == 2) {
					objarray[i].load("images/241pxbuilding.png", renderer);
						locy = 322;
						victimVec[i].load("images/BottomVictim.png", renderer);
						victimVec[i].setY(locy - victimVec[i].getH());
					}

				if (whichbuilding == 3) {
					objarray[i].load("images/254pxbuilding.png", renderer);
						locy = 298;
						victimVec[i].load("images/BottomVictim.png", renderer);
						victimVec[i].setY(locy - victimVec[i].getH());
					}

			}
			locationx[i]=locx;
			/* set the obj position*/
			locationy[i]=locy;
			objarray[i].setX(locx);
			objarray[i].setY(locy);

			std::cout<<"Building at location X: "<<locationx[i];
			std::cout<<" Y: "<<locationy[i]<<std::endl;

			bigorsmall = (rand()%2); //1 is 400 away from next, 0 is 350
			if (bigorsmall == 1){
				locx=locx+350;

			}
			else{
				locx=locx+400;
			}
		}



		/* set the victim position */
		for(int i=0; i<NUM_BUILDINGS; i++){
			victimVec.at(i).setX(locationx[i]+50);
		}

		////////////////////////////////
		//         GAME LOOP          //
		////////////////////////////////
		
		/* the start screen, if the user didn't specify to ignore it */
		if( !skipScreens )
		{
			/* play the title screen music */
			if( Mix_PlayMusic(tmusic, -1) == -1 ){
				std::cerr << "Failed to play music: " << Mix_GetError()
													  << std::endl;
			}
			
			bool startisrun = true;
			Sprite startbackground;
			startbackground.load("images/TitleScreenV1.png",renderer);

			while(startisrun){
				startbackground.draw(renderer);
				SDL_RenderPresent(renderer);
				SDL_Delay(1000.0/FPS);
				int x=0; int y=0;
				while(SDL_PollEvent(&event)){
					
					if(event.type == SDL_QUIT){
						TTF_CloseFont(font);
						IMG_Quit();
						TTF_Quit();
						if( bgmusic ) Mix_FreeMusic(bgmusic);
						if( xplode )  Mix_FreeChunk(xplode);
						if( sperson )  Mix_FreeChunk(sperson);
						Mix_CloseAudio();
						Mix_Quit();
						SDL_DestroyTexture(texture);
						SDL_DestroyRenderer(renderer);
						SDL_DestroyWindow(window);
						SDL_Quit();
						
						cleanMiniAT(&m);
						return 0;
					}
					
					if (event.type == SDL_KEYDOWN){
						switch (event.key.keysym.sym){
							case SDLK_s:
								startisrun = false;
						}
					}
					
					if (event.type == SDL_MOUSEBUTTONDOWN){
						if((event.button.button == SDL_BUTTON_LEFT)){
							x = event.button.x;
							y = event.button.y;
							if((x>288) && (x<758) && (y>253) && (y<360)){
								startisrun = false;
							}
						}
					}
				}
			}
			
			/* stops the music */
			Mix_HaltMusic();
		}

		/* start the game music */
		if( Mix_PlayMusic(bgmusic, -1) == -1 ){
			std::cerr << "Failed to play music: " << Mix_GetError()
												  << std::endl;
		}
		
		SDL_Event Pause;
		SDL_Event unpause;
        bool gamePause = false;

		while(isRunning)
		{
			/* update events */
			while( SDL_PollEvent(&event) )
			{
				/* close the window out */
				if( event.type == SDL_QUIT ){
						TTF_CloseFont(font);
						IMG_Quit();
						TTF_Quit();
						if( bgmusic ) Mix_FreeMusic(bgmusic);
						if( xplode )  Mix_FreeChunk(xplode);
						if( sperson )  Mix_FreeChunk(sperson);
						Mix_CloseAudio();
						Mix_Quit();
						SDL_DestroyTexture(texture);
						SDL_DestroyRenderer(renderer);
						SDL_DestroyWindow(window);
						SDL_Quit();
						
						cleanMiniAT(&m);
						return 0;
				}
				
				if(event.type == SDL_KEYDOWN){
                    switch(event.key.keysym.sym){
                case  SDLK_p:
                    gamePause = true;
                    }
                }

			}
			
			bool drawnOnce = false;
            while(gamePause == true){
				/* draw the pause screen */
				if( !drawnOnce ){
					pause.draw(renderer);
					SDL_RenderPresent(renderer);
					drawnOnce = true;
				}
                while(SDL_PollEvent(&unpause)){
                    if(unpause.type == SDL_KEYDOWN){
                        switch(unpause.key.keysym.sym){
                        case SDLK_p:
                        gamePause = false;
                        }
                    }
                }
            }
			
			/* apply velocity, friction, etc... to the helicopter */
			calcVelocity(heli, thrustX, thrustY, vX, vY);

			/* Clear the screen */
			SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0xff);
			SDL_RenderClear(renderer);

			/* move each victim backward */
			for(std::vector<Sprite>::iterator it = victimVec.begin(); it < victimVec.end()-1; it++){
				(*it).setX((*it).getX()-SCROLL_SPEED);
			}

			/* move the buildings each frame */
			for(int i=0;i<NUM_BUILDINGS;i++){
				objarray[i].setX(objarray[i].getX()-SCROLL_SPEED);
			}

			/* mike's background scrolling */
			cameraRect.x += BG_SCROLL_SPEED; //Makes it an auto scroller

			/* BJ EDITED HERE TO MAKE LEVEL LAST LONGER! */
			if(cameraRect.x > 6000-S_WIDTH) {
				cameraRect.x = 50; // restart the scrolling
				//std::cout << "Reached End of Level!!!\n";
				//cameraRect.x = 3976;
			}
			
			/* BJ ADDED TO END LEVEL HERE */
			if( cycles >= 2600 ){
				EndingChoice = 2;
				break;
			}

			/* adapted mike's playerRect to heli sprite */
			if(heli.getX() >= 1024-30){
				heli.setX(1024-31);
				cameraRect.x += 30;
			}
			if(heli.getX() < 15) {
				heli.setX(15);
			}
				//heli.getX() = cameraRect.x+15;
			if(heli.getY() > cameraRect.y+738) {
				heli.setY(cameraRect.y+738);
			}
			if(heli.getY() < cameraRect.y) {
				heli.setY(cameraRect.y);
			}


			/* check collision - bj added 04/08/17 */
			/* if there arent any invincibility frames left
			 * and there is a collision, reset the timer */
			if( invince_timer == 0 ) {
				
				if( check_building_collision(heli, lives, objarray, NUM_BUILDINGS) ){
					invince_timer = INVINCIBILITY_FRAMES;
					heli.setNumFrames(4); // start the animation of the helicopter

					/* start the explosion */
					explosion.setW(100);
					explosion.setH(100);
					explosion.setNumFrames(9);
					explosionCounter = 0;
					
					/* play sound effect 
					 * -1 - first default channel 
					 * xplode - Mix_Chunk * 
					 * 0 - number of loops */
					Mix_PlayChannel(-1, xplode, 0);
				}
			}
			
			/*ADDED FOR ALTERNATE SCREENS*/
			if(lives > 5){
				EndingChoice = 1;
				break;
			}

			/* countdown the invincibility frames timer if it
			 * hasn't stopped */
			if( invince_timer > 0 ){
				invince_timer--;
			}
			/* check if the helicopter has hit a victim */
			check_victim_collision(heli, victimVec, saved_victims, score, sperson);

			/* draw's mike's background to the screen */
			SDL_RenderCopy(renderer,texture,&cameraRect,NULL); //copies new render

			/*draw the buildings to the screen*/
			for (int i=0;i<NUM_BUILDINGS;i++){
				objarray[i].draw(renderer);
			}

			/* if there are still victims left to be saved,
			 * loop through and draw them, skipping the last
			 * placeholder sprite since it isn't drawn */
			if( !victimVec.empty() ) {
					for(std::vector<Sprite>::iterator it = victimVec.begin(); it<victimVec.end()-1; it++){
						(*it).draw(renderer);
					}
				}

			/* draw the number of lives to the screen */
			std::string livesStr = "Lives = ";
			// edited for windows mingw bug
			//livesStr += std::to_string(lives);
			livesStr += patch::to_string(lives);

			/* draw the text background */
			textBackground1.draw(renderer);

			if ( !drawText(font, renderer, livesStr.c_str(), S_WIDTH - 150, 10) ){
				std::cerr << "Failed to draw text!\n";
			}

			/* draw the score saved to the screen */
			std::string scoreStr = "Score = ";

			// edited for windows mingw bug

			if (cameraRect.x == 1000 || cameraRect.x == 2000 || cameraRect.x == 3000 || cameraRect.x == 4000){
				score += (cameraRect.x/20);
			}

			scoreStr += patch::to_string(score);
			//victimsStr += std::to_string(saved_victims);

			/* draw the background for the text */
			textBackground0.draw(renderer);

			/* draw the score */
			if( !drawText(font, renderer, scoreStr.c_str(), 10, 10) ){
				std::cerr << "Failed to draw text!\n";
			}

			/* if the helicopter is still running on invincibility frames,
			 * update its flashing animation */
			if( invince_timer > 0 ) {
				/* update the image based on the sprite sheet and animation speed */
				heli.animate();
			}
			/* otherwise give the sprite only 1 frame so it doesn't animate,
			 * and clip out the rest of the image */
			else{
				heli.setNumFrames(1);
				heli.setH(31);
			}

			/* keep track of the number of frames the explosion has animated so
			 * it only goes through its cycle once */
			if(explosionCounter < (explosion.getNumFrames()-1)*explosion.getAnimSpeed()) {
				explosionCounter++;
				explosion.setX(heli.getX());
				explosion.setY(heli.getY());
				explosion.animate();
			}
			else{
				explosion.setW(0);
				explosion.setH(0);
			}

			/* draw the helicopter to the screen */
			heli.draw(renderer);

			/* draw the explosion */
			explosion.draw(renderer);

			/* Update the screen */
			SDL_RenderPresent(renderer);

			/* update miniat */
			if( cycles < MAX_CYCLES ) cycles++;
			miniat_clock(m); // update miniat clock cycle
			clock_peripherals(m, heli, thrustY, vY, lives,
							  victimVec,
							  objarray, NUM_BUILDINGS); // read or write addresses to/from miniat peripherals

			clock_ports(m, vY, thrustY, MAX_VEL); // set port values based on if the helicopter is moving and accelerating

			/* show current number of cycles 
			 * DEBUG - BJ - to end the level */
			//std::cout << "Cycles = " << cycles << std::endl;

			/* regulate FPS */
			SDL_Delay(1000.0f/FPS);
		}

		int x,y;
		SDL_Event decision;
		Uint32 SDL_GetMouseState(int* x, int* y);
		/*ADDED FOR ALTERNATE SCREENS*/
		if(EndingChoice == 1)
		{
			//Lose Screen

			Sprite LoseScreen;
			if(!LoseScreen.load("images/LoseScreenPNG.png", renderer)){
			   printf("Error Loading Image\n");
			}
			std::cout << "you lose" <<std::endl;
			
			/* halt currently playing music */
			Mix_HaltMusic();
			
			/* load victory music */
			Mix_Music *loseTheme = NULL;
			loseTheme = Mix_LoadMUS("music/death_music2.wav");
			if( !loseTheme ){
				std::cerr << "Error loading music: " << Mix_GetError()
				                                     << std::endl;
			}
			else{
				/* if no issues play the new music */
				Mix_PlayMusic(loseTheme, 1);
			}
			
			while(smallLoop  == true)
			{

				LoseScreen.draw(renderer);
				SDL_RenderPresent(renderer);
				//if statements for the buttons
				while(SDL_PollEvent(&decision))
				{
					if(decision.type == SDL_QUIT ){
						TTF_CloseFont(font);
						IMG_Quit();
						TTF_Quit();
						if( bgmusic ) Mix_FreeMusic(bgmusic);
						if( xplode )  Mix_FreeChunk(xplode);
						Mix_CloseAudio();
						Mix_Quit();
						SDL_DestroyTexture(texture);
						SDL_DestroyRenderer(renderer);
						SDL_DestroyWindow(window);
						SDL_Quit();
						
						cleanMiniAT(&m);
						return 0;
					}
					
					if(decision.type == SDL_MOUSEBUTTONDOWN)
					{
						if((decision.button.button == SDL_BUTTON_LEFT)){
								x = decision.button.x;
								y = decision.button.y;

								if((x>370) && (x < 550) && (y>140) && y<200){
									smallLoop = false;
								}

								if((x>370) && (x<550) && (y>290) && (y<350)){
									TTF_CloseFont(font);
									IMG_Quit();
									TTF_Quit();
									if( bgmusic ) Mix_FreeMusic(bgmusic);
									if( xplode )  Mix_FreeChunk(xplode);
									Mix_CloseAudio();
									Mix_Quit();
									SDL_DestroyTexture(texture);
									SDL_DestroyRenderer(renderer);
									SDL_DestroyWindow(window);
									SDL_Quit();
									
									cleanMiniAT(&m);
									return 0;
								}
						}
					}
				}

			}
			
			/* free lose music */
			if( loseTheme ) Mix_FreeMusic(loseTheme);
			
			EndingChoice = 0;
			smallLoop = true;
		}

		if(EndingChoice == 2)
		{
			Sprite LoseScreen;
			if(!LoseScreen.load("images/WinScreenPNG.png", renderer)){
			   printf("Error Loading Image");
			   }
			std::cout << "you win!" <<std::endl;
			
			/* halt currently playing music */
			Mix_HaltMusic();
			
			/* load victory music */
			Mix_Music *vicTheme = NULL;
			vicTheme = Mix_LoadMUS("music/win.wav");
			if( !vicTheme ){
				std::cerr << "Error loading music: " << Mix_GetError()
				                                     << std::endl;
			}
			else{
				/* if no issues play the new music */
				Mix_PlayMusic(vicTheme, 1);
			}
			
			while(smallLoop  == true)
			{

				LoseScreen.draw(renderer);
				SDL_RenderPresent(renderer);
				//if statements for the buttons
				while(SDL_PollEvent(&decision))
				{
					if( decision.type == SDL_QUIT ){
						TTF_CloseFont(font);
						IMG_Quit();
						TTF_Quit();
						if( bgmusic ) Mix_FreeMusic(bgmusic);
						if( xplode )  Mix_FreeChunk(xplode);
						Mix_CloseAudio();
						Mix_Quit();
						SDL_DestroyTexture(texture);
						SDL_DestroyRenderer(renderer);
						SDL_DestroyWindow(window);
						SDL_Quit();
						
						cleanMiniAT(&m);
						return 0;
					}
					
					if(decision.type == SDL_MOUSEBUTTONDOWN)
					{
						if((decision.button.button == SDL_BUTTON_LEFT))
						{
							x = decision.button.x;
							y = decision.button.y;


							if((x>5) && (x<120) && (y>450) && (y<530)){
								TTF_CloseFont(font);
								IMG_Quit();
								TTF_Quit();
								if( bgmusic ) Mix_FreeMusic(bgmusic);
								if( xplode )  Mix_FreeChunk(xplode);
								Mix_CloseAudio();
								Mix_Quit();
								SDL_DestroyTexture(texture);
								SDL_DestroyRenderer(renderer);
								SDL_DestroyWindow(window);
								SDL_Quit();
								
								cleanMiniAT(&m);
								return 0;
							}
						}
					}
				}

			}
			
			/* free music */
			if(vicTheme) Mix_FreeMusic(vicTheme);
			
			EndingChoice = 0;
			smallLoop = true;
		}

		/*END OF ADD*/
		
		lives = 5; // reset the number of lives the helicopter has

		/* Free memory and exit libraries */
		TTF_CloseFont(font);
		SDL_DestroyTexture(texture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);

		Mix_FreeMusic(bgmusic);
		Mix_FreeMusic(tmusic);
		Mix_FreeChunk(xplode);
		Mix_FreeChunk(sperson);
		
		skipScreens = true; // so we don't go to the start menu agains
		
		miniat_reset(m);    // restart the miniat simulator
		
		cycles = 0;         // reset the number of cycles ran for
		                    // miniat for that simulation
	
	} // end bigGame loop
	
	Mix_CloseAudio();
	IMG_Quit();
	TTF_Quit();
	Mix_Quit();		
	SDL_Quit();
		
	/* print how many clock cycles the
	 * miniat accumulated */
	std::cout << "Cycles Ran: " << cycles << std::endl;	

	/* free miniat stuff */
	cleanMiniAT(&m);
		
	return 0;
}


///////////////////////////////////////
//        FUNCTION DEFINITIONS       //
///////////////////////////////////////
/*
 * initSDL - initializes SDL Library,
 * SDL_ttf library, SDL_image library
 * William Blair
 * 02/16/17
 * */
bool initSDL(void)
{
	/* initialize SDL and check for errors */
	if( SDL_Init(SDL_INIT_VIDEO) < 0 ){
		std::cerr << "Failed to init SDL: " << SDL_GetError()
		                                    << std::endl;
		return false;
	}

	/* Initialze TTF and check for errors */
	if( TTF_Init() == -1 ){
		std::cerr << "Failed to init TTF: " << TTF_GetError()
		                                    << std::endl;
		return false;
	}

	/* Initialize IMG and check for errors */
	int flags = IMG_INIT_JPG | IMG_INIT_PNG;
	int initted = IMG_Init(flags);
	if( initted & flags != flags ){
		std::cerr << "Failed to init IMG: " << IMG_GetError()
		                                    << std::endl;
		return false;
	}
	
	/* Initialize Music library and check for errors */
	flags = MIX_INIT_MP3;
	initted = Mix_Init(flags);
	if( initted&flags != flags ){
		std::cerr << "Failed to init Mix: " << Mix_GetError()
		                                    << std::endl;
		return false;
	}
	
	/* open the audio channels on the computer */
	if( Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1 ){
		std::cerr << "Failed to open audio: " << Mix_GetError()
		                                      << std::endl;
		return -1;
	}

	/* if nothing above failed, return success */
	return true;
}

/*
 * createWindow - initializes a SDL Window
 * and checks for errors
 * William Blair
 * 02/16/17
 * */
bool createWindow(SDL_Window **w)
{
	/* create a window */
	*w = SDL_CreateWindow(title, // title of the window
	                          SDL_WINDOWPOS_UNDEFINED, //x on screen
	                          SDL_WINDOWPOS_UNDEFINED, //y on screen
	                          S_WIDTH,                 // window width
	                          S_HEIGHT,                // window height
	                          SDL_WINDOW_SHOWN ); // display the window

	/* make sure there were no errors creating the window */
	if( !(*w) ){
		std::cerr << "Failed to create window: " << SDL_GetError()
		                                         << std::endl;
		return false;
	}

	/* if no error, return true */
	return true;
}
/*
 * calcVelocity - applies thrust
 * and calculates the helicopter's speed
 * William Blair
 * 02/22/17
 * */
void calcVelocity(Sprite &heli, int thrustX, int thrustY, double &vX, double &vY)
{
	double A_SCALER = 0.004; // scales the thrust into acceleration
	double FRICTION = 0.95; // scales the velocity; simulates air current
	double closeEnough = 0.1; // the minumum vX and vY of the copter to totally stop it


	/* calculate acceleration */
	double aX = thrustX * A_SCALER;
	double aY = thrustY * A_SCALER;

	/* calculate velocity based on acceleration */
	vX += aX;
	vY += aY;

	/* apply friction */
	vX *= FRICTION;
	vY *= FRICTION;

	/* apply max velocity */
	if( vX > MAX_VEL ) vX = MAX_VEL;
	else if( vX < -MAX_VEL ) vX = -MAX_VEL;
	if( vY > MAX_VEL ) vY = MAX_VEL;
	else if( vY < -MAX_VEL ) vY = -MAX_VEL;


	/* if no thrust applied stop the helicopter if its close enough */
	if( thrustX == 0 && vX > 0){
		if( vX < closeEnough ) vX = 0;
	}
	else if( thrustX == 0 && vX < 0){
		if( vX > -closeEnough ) vX = 0;
	}
	if( thrustY == 0 && vY > 0){
		if( vY < closeEnough ) vY = 0;
	}
	else if( thrustY == 0 && vY < 0){
		if( vY > -closeEnough ) vY = 0;
	}


	/* set the new position of the helicopter, while preventing
	 * it from going offscreen */
	if( ((heli.getX() < S_WIDTH - heli.getW()) && vX > 0) ||
	    (heli.getX() > 0 && vX < 0)) {
		heli.setX( heli.getX() + vX );

		/* if the helicopter has moved past the edge of the screen at all
		 * force it on screen, also reset velocity */
		if( heli.getX() > S_WIDTH - heli.getW() ) {
			heli.setX( S_WIDTH - heli.getW() );
			vX = 0;
		}
		else if(heli.getX() < 0 ) {
			heli.setX(0);
			vX = 0;
		}
	}
	/*if trying to push against either the top or bottom of the screen set the velocity to 0*/
	else{
		vX = 0;
	}
	if( ((heli.getY() < S_HEIGHT - heli.getH()) && vY > 0) ||
	    (heli.getY() > 0 && vY < 0)) {
		heli.setY( heli.getY() + vY );

		/* if the helicopter has moved past the edge of the screen at all */
		if( heli.getY() > S_HEIGHT - heli.getH() ) {
			heli.setY( S_HEIGHT - heli.getH() );
			vY = 0;
		}
		else if( heli.getY() < 0 ){
			heli.setY(0);
			vY = 0;
		}
	}
	/*if trying to push against either the top or bottom of the screen set the velocity to 0*/
	else{
		vY = 0;
	}

	return;
}

/*
 * initMiniAT - creates a new miniat system with the given
 * miniat pointer, from compiling the given assembly file
 * William Blair
 * 03/01/17
 * */
bool parseArgs(miniat **m, bool &skipScreens, int argc, char *argv[])
{
	FILE *binFile = NULL; // the .bin file generated from the asm file
	char *asmFile = NULL; // the .asm file to convert to the binFile
	int   opt;            // the command line option

	/* signal handlers for if the program ctrl c's */
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	/* cleanup is a function that frees miniat and other stuff */
	// not necessarily needed right now
	//atexit(cleanup);

	/* compile asm file if given */
	if( argc < 2 ){
		std::cerr << "No MiniAT assembly file given!\n";
		std::cout << "Usage: " << argv[0] << " [-s] <-f<asmfile>>\n";
		return false;
	}

	/* delete prviously generated binary file if there is one */
#ifdef __WIN32
	system("del *.bin"); // delete for windows
#else
	system("rm -f *.bin"); // delete for anything else (*NIX)
#endif

	/* get command line options 
	 * -f[file] - the miniat .asm file 
	 * -s       - skips title screen */
	while((opt = getopt(argc, argv, "f:s")) != -1)
	{
		switch(opt)
		{
			case 's':
				skipScreens = true;
				break;
			case 'f':
				asmFile = optarg;
				break;
			case '?':
				if(optopt == 'f'){
					std::cerr << "Option -f requires a .asm arg!\n";
					std::cout << "Usage: " << argv[0] << " [-s] <-f<asmfile>>\n";
				}
				break;
			default:
				std::cout << "Usage: " << argv[0] << " [-s] <-f<asmfile>>\n";
		}
	}

	if( !asmFile ){
		std::cerr << "No assemby file given!\n";
		std::cout << "Usage: " << argv[0] << " [-s] <-f<asmfile>>\n";
		return false;
	}
	
	std::string asmCommand = "mash ";
	asmCommand += asmFile;
	system(asmCommand.c_str()); // mash is the assembler program that generates the .bin file

	/* get the new bin file's name
	 * subtracts the '.asm' suffix from the argument assembly file
	 * then adds the .bin */
	std::string binStr = "";
	for(int i=0; i<strlen(asmFile)-4; i++ ){
		binStr += asmFile[i];
	}
	binStr += ".bin";
	std::cout << "Bin File Name = " << binStr << std::endl;


	/* open the generated .bin file */
	binFile = fopen(binStr.c_str(), "r+b");
	if( !binFile ){
		std::cerr << "Failed to open " << binStr << ": " << strerror(errno)
		                                                 << std::endl;
		std::cerr << "Mash failed to compile?\n";
		return false;
	}

	/* init the miniat with the bin file */
	*m = miniat_new(binFile, NULL);
	if( !(*m) ){
		std::cerr << "Failed to create miniat!\n";
		return false;
	}
	std::cout << "Initialized MiniAT!\n";

	return true;
}

/* Copied from Confer's example miniat integration (robots) */
static void cleanMiniAT(miniat **m) {

	std::cout << "In free miniat!\n";
	if(*m) {
		/* MiniAT also closes the binary file it was passed on miniat_new */
		std::cout << "Freeing MiniAT!\n";
		miniat_free(*m);
	}

	/*
	 * Call all the other cleanup functions
	 */
	//peripherals_cleanup();
	//ports_cleanup();

	//if(cycles < MAX_CYCLES) {
		//printf("\n%"PRIu64" cycles executed\n", cycles);
	//}
	//else {
		//printf("Runtime exceeded %"PRIu64" cycles!\n", MAX_CYCLES);
	//}

	//return;
}

/* copied from confer's example miniat integration (robots)
 * BJ added SDL quit and other stuff */
static void signal_handler(int sig) {

	if(sig == SIGINT || sig == SIGTERM) {

		/* close SDL and stuff */
		if( SDL_WasInit(SDL_INIT_VIDEO) != 0){
			SDL_Quit();

			/* SDL Image doesn't have a was init function
			 * so we're assuming if SDL was initted IMG
			 * was as well */
			IMG_Quit();
			
			/* same for mix */
			Mix_CloseAudio();
			Mix_Quit();
		}
		if( TTF_WasInit() ){
			TTF_Quit();
		}

		exit(EXIT_SUCCESS);
	}

	return;
}

/*
 * William Blair
 * 04/08/17
 * check_building_collision:
 *
 * takes the helicopter sprite and buildings and
 * checks if the helicopter is hitting them;
 * if so it decreases the player's lives
 *
 * does not count for 'invincibility frames' - should be
 * added above */
bool check_building_collision(Sprite &heli, unsigned int &lives, Sprite *objarray, int objarray_size)
{
	bool result = false; // true if there is a collision

	/* loop through each building and see if the helicopter is hitting it*/
	for(int i=0; i<objarray_size; i++)
	{
		/* test to make sure the helicopter is in between the left/right of the building,
		 * and that it is vertically touching the building as well */
		if( (heli.getX() + heli.getW()) >= (objarray+i)->getX() &&          // the helicopter is at least touching the left side of or in front of the bulding
		    heli.getX() < ((objarray+i)->getX() + (objarray+i)->getW()) &&  // the helicopter isn't past the building
			(
			  ( heli.getY() + heli.getH() >= (objarray+i)->getY() &&        // the helicopter's bottom is at least touching or lower than the top of a ground building
				(objarray+i)->getY() > heli.getH()                          // the building is a ground building
			  ) ||                                                          // the above 2 or
			  (
			    heli.getY() <= (objarray+i)->getY() + (objarray+i)->getH() && // the top of the helicopter is higher up than or touching the bottom of a ceiling building
				(objarray+i)->getY() <= 0                                     // the building is a ceiling building
			  )
			)
		  ){
			/* finally if the helicopter is touching a building decrease its lives by 1 */
			if( lives >= 0 ) {
				lives -= 1;
				std::cout << "Hit! Lives = " << lives << std::endl;
				result = true;
			}
			if(lives < 0){
				std::cout << "You have died!\n";
			}
		}
	}


	return result;
}

/*
 * William Blair
 * 04/13/17
 * check_victim_collision - loops through the currently
 * unsaved sprites and sees if they are hit by the helicopter,
 * if so remove them from the victim vector and increase
 * the number of saved victims by 1
 * */
void check_victim_collision(Sprite &heli, std::vector<Sprite> &victims, unsigned int &saved_victims, int &score, Mix_Chunk *sperson)
{
	/* loop through each member of the vector */
	for(std::vector<Sprite>::iterator it = victims.begin(); it<victims.end()-1; it++)
	{
		/* test that the helicopter is touches the victim */
		if( heli.getX() + heli.getW() >= (*it).getX() &&       // the helicopter's front is at least touching or past the left side of the victim
		    heli.getX() < (*it).getX() + (*it).getW() &&  // the helicopter's back is as at least to the left of the right side of the victim
		    heli.getY() + heli.getH() >= (*it).getY() &&       // the helicopter's bottom is at least touching or below the top of the victim
		    heli.getY() <= (*it).getY() + (*it).getH()    // the helicopter's top is at least touching or above the bottom of the victim
		  )
		{
			/* if the helicopter touched the victim, remove it from the vector
			 * and increase the number of saved victims */
			victims.erase( it ); // access to member to erase using the iterator
			saved_victims++;
			score += 50;
			
			/* play the save person sound effect */
			Mix_PlayChannel(-1, sperson, 0);

			std::cout << "Saved a victim!\n";
		}
	}

	return;
}
