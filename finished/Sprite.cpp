/*
 * William Blair
 * CS370
 * 02/20/17
 * Sprite.cpp - sprite class for heligame rescue
 * */
 
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <iostream>

#include "Sprite.h"

/* Constructor */
Sprite::Sprite(const char *filename, SDL_Renderer *r, int numFrames, int animSpeed)
{	
	/* Temporary surface to use */
	SDL_Surface *s = NULL;
	
	/* if a file to load was given */
	if( filename != NULL && r != NULL ){
		
		/* load the image */
		s = IMG_Load(filename);
		
		/* if failed tell the user */
		if( !s ){
			std::cerr << "Failed to load " << filename << ": "
			          <<  IMG_GetError() << std::endl;
		}
		else {
			/* create the texture */
			t = SDL_CreateTextureFromSurface(r, s);
			
			/* if failed tell the user */
			if( !t ){
				std::cerr << "Failed to create texture: "
				          << SDL_GetError() << std::endl;
			}
		}
	}
	else {
		t = NULL; // be safe
	}
	
	/* initialize the SDL rect */
	clipRect.w = ((s == NULL) ? 0 : s->w); // shorthand if else - if s is NULL, then '0',
	clipRect.h = ((s == NULL) ? 0 : s->h); // else s->w/h
	clipRect.x = 0;
	clipRect.y = 0;
	
	if( s != NULL) {
		totalHeight = s->h;
	}
	else{
		totalHeight = 0;
	}
	
	clipRect.h /= numFrames; // reduce the height by the number of frames in the image
	
	/* initializes the position of the sprite to 0 */
	x = 0;
	y = 0;
	
	frameCounter = 0;
	frameNum = 0;
	
	/* set the number of frames the sprite has */
	this->numFrames = numFrames;
	
	/* set how fast the sprite animates */
	this->animSpeed = animSpeed;
	
	/* free the temp surface */
	if( s != NULL ) {
		SDL_FreeSurface(s);
	}
}

/* no args constructor */
Sprite::Sprite()
{
	t = NULL; // be safe with texture pointer
	clipRect = {0};  // sets all values of the rect to 0

	/* initializes the position of the sprite to 0 */
	x = 0;
	y = 0;
	
	numFrames = 0; // no frames to draw if there is no image
	animSpeed = 0;
	frameCounter = 0;
	totalHeight = 0;
	frameNum = 0;
}

/* deconstructor */
Sprite::~Sprite()
{
	//std::cout << "Freeing sprite!" << std::endl;
	if( t != NULL ) {
		SDL_DestroyTexture(t);
	}
}

/* load an image */
bool Sprite::load(const char *filename, SDL_Renderer *r, int numFrames, int animSpeed)
{
	bool outcome = true; // false if anything fails

	/* Temporary surface to use */
	SDL_Surface *s = NULL;
	
	/* if a file to load was given */
	if( filename != NULL && r != NULL ){
		
		/* load the image */
		s = IMG_Load(filename);
		
		/* if failed tell the user */
		if( !s ){
			std::cerr << "Failed to load " << filename << ": "
			          <<  IMG_GetError() << std::endl;

			outcome = false;
		}
		else {
			/* free the sprite if already loaded */
			if( t != NULL){
				SDL_DestroyTexture(t);
				t = NULL;                 // just to be safe...
			}
			
			/* create the texture */
			t = SDL_CreateTextureFromSurface(r, s);
			
			/* if failed tell the user */
			if( !t ){
				std::cerr << "Failed to create texture: "
				          << SDL_GetError() << std::endl;
				outcome = false;
			}
		}
	}
	
	/* initialize the SDL rect */
	clipRect.w = ((s == NULL) ? 0 : s->w); // shorthand if else - if s is NULL, then '0',
	clipRect.h = ((s == NULL) ? 0 : s->h); // else s->w/h
	clipRect.x = 0;
	clipRect.y = 0;
	
	/* set the overall height of the spritesheet if applicable */
	if( s != NULL) {
		totalHeight = s->h;
	}
	else{
		totalHeight = 0;
	}
	
	/* split the height of the image into the height of each frame */
	clipRect.h /= numFrames;
	
	/* set the internal variables */
	this->numFrames = numFrames;
	this->animSpeed = animSpeed;
	
	frameCounter = 0;
	frameNum = 0;
	
	std::cout << "Clip Rect Width: " << clipRect.w << std::endl;
	std::cout << "Clip Rect Height: " << clipRect.h << std::endl;
	
	/* free the temp surface */
	if( s != NULL ) {
		SDL_FreeSurface(s);
	}
	
	return outcome;
}

bool Sprite::close(void)
{
	std::cout << "Freeing sprite!" << std::endl;
	if( t != NULL ) {
		SDL_DestroyTexture(t);
	}

	return true;
}

/* draw the texture to a renderer */
bool Sprite::draw(SDL_Renderer *r)
{
	/* rect to specify the location of the sprite */
	SDL_Rect screenRect;
	screenRect.w = clipRect.w;
	screenRect.h = clipRect.h;
	screenRect.x = x;
	screenRect.y = y;
	
	/* draw the sprite to the renderer */
	if( SDL_RenderCopy(r, t, &clipRect, &screenRect) < 0){
		std::cerr << "Failed to render copy: " << SDL_GetError()
		                                       << std::endl;
		return false;
	}

	/* if haven't returned false then it was successful */
	return true;
}

/* move to the next image in the sprite sheet - assumed stacked vertically */
bool Sprite::animate(void)
{
	frameCounter++;
	
	/* if the limiter has reached the desired delay */
	if(frameCounter >= animSpeed)
	{
		clipRect.y += clipRect.h; // move down in the image
		
		frameNum++;               // increase which frame number we are on
		
		/* move back to the top of the spritesheet if the last frame has been past */
		if( clipRect.y > clipRect.h * (numFrames-1)) {
			clipRect.y = 0;
			frameNum = 0;
		}
		
		/* reset the limiter */
		frameCounter = 0;
	}
	
	return true;
}

/* free memory */
void Sprite::free(void)
{
	if( t != NULL ) {
		SDL_DestroyTexture(t);
	}
	return;	
}

/* getter/setter functions */
SDL_Texture *Sprite::getTexture(void)
{
	return t;
}

void Sprite::setX(int x){
	this->x = x;
}
void Sprite::setY(int y){
	this->y = y;
}

void Sprite::setW(int w){
	if(w >= 0)
		this->clipRect.w = w;
	return;
}

void Sprite::setH(int h){
	if( h >= 0 )
		this->clipRect.h = h;
	return;
}

void Sprite::setFrameNum(int frameNum){
	if( frameNum >= 0 ){
		this->frameNum = frameNum;
	}
	return;
}

int Sprite::getX(void){
	return x;
}
int Sprite::getY(void){
	return y;
}

int Sprite::getW(void){
	return clipRect.w;
}

int Sprite::getH(void){
	return clipRect.h;
}

int Sprite::getAnimSpeed(void){
	return animSpeed;
}

bool Sprite::setAnimSpeed(int animSpeed){
	if(animSpeed >= 1){
		this->animSpeed = animSpeed;
		return true;
	}
	std::cerr << "Invalid anim speed: " << animSpeed << std::endl;
	std::cerr << "Must be >= 1!\n";
	return false;
}

int Sprite::getNumFrames(void){
	return numFrames;
}

int Sprite::getFrameNum(void){
	return frameNum;
}

bool Sprite::setNumFrames(int numFrames){
	if(numFrames >= 1){
		
		/* set the new value */
		this->numFrames = numFrames;
		
		/* update the desired height of the image */
		clipRect.h = totalHeight; // reset the height of the image
		clipRect.h /= numFrames;
		
		clipRect.y = 0; // move to the beginning of the sheet
		
		return true;
	}
	std::cerr << "Invalid num frames: " << numFrames << std::endl;
	std::cerr << "Must be >= 1!\n";
	return false;
}

int Sprite::getFrameCounter(void){
	return frameCounter;
}
