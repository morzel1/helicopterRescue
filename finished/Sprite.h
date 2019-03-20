/*
 * William Blair
 * CS370
 * 02/20/17
 * Sprite.h - sprite class for heligame rescue
 * */
 
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <iostream>

#ifndef _SPRITE_H_
#define _SPRITE_H_

class Sprite
{
	public:
	
		/* Constructor */
		Sprite(const char *filename, SDL_Renderer *r, int numFrames=1, int animSpeed=10 );
		Sprite();
		
		/* DeConstructor */
		~Sprite();
		
		/* load a new or initial image */
		bool load(const char *filename, SDL_Renderer *r, int numFrames=1, int animSpeed=10);

		/* close a sprite (frees texture) */
		bool close(void);
		
		/* free the internal texture */
		void free(void);
		
		/* draw the texture to a renderer */
		bool draw(SDL_Renderer *r);
		
		/* increase spritesheet frames if applicable */
		bool animate(void);
		
		/* Getter Functions */
		SDL_Texture *getTexture(void);
		int          getX(void);
		int          getY(void);
		int          getW(void);
		int          getH(void);
		int          getNumFrames(void);
		int          getAnimSpeed(void);
		int          getFrameCounter(void);
		int          getFrameNum(void);
		
		/* setter functions */
		void setX(int x);
		void setY(int y);
		void setW(int w);
		void setH(int h);
		bool setNumFrames(int numFrames);
		bool setAnimSpeed(int animSpeed);
		void setFrameNum(int frameNum);
		//bool optimize(SDL_Surface *s); // optimize the image based on the display surface
	
	private:
	
		SDL_Texture *t;        // holds the image
		SDL_Rect     clipRect; // internal size/position of the image
		
		int x,y;               // the x and y position of the image
		
		int totalHeight;       // the height of the total spritesheet
		int numFrames;         // the number of different images on the sprite sheet
		int animSpeed;         // the number of frames until the next image is loaded
		int frameCounter;      // used to compare to animSpeed to calculate when the next frame should be loaded
		int frameNum;          // which frame the sprite is currently on
};

#endif 
