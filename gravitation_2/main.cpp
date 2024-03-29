#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdio.h>
#include "time.h"
#include <vector>
#include <array>
#include <string>
#include "include/vector2d.h"
#include "include/math_utils.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
int framecount = 1;

const float G = 12;

bool DRAW_PLANETS = true;

const int PREV_POS_NBR = 10;

struct particle
{
    Vector2D pos;
	std::array<Vector2D, PREV_POS_NBR> prev_pos;
    Vector2D vel;
    Vector2D acc;

	particle(Vector2D arg_position)
	{
		pos = arg_position;
		for(int i = 0; i < PREV_POS_NBR; i++)
		{
			prev_pos[i] = pos;
		}
		vel = Vector2D(0, 0);
		acc = Vector2D(0, 0);
	}

    void upply_force(Vector2D force)
    {
        acc += force;
    }
    void update()
	{
		//store the previous positions
		prev_pos[0] = pos;
		for(int i = PREV_POS_NBR-1; i >= 1; i--)
		{
			prev_pos[i] = prev_pos[i-1];
		}
        vel += acc;
        pos += vel;
        acc *= 0;
    }
};

const float PLANET_SIZE = 4;
int PLANET_NBR = 12;
std::vector<Vector2D> planets;

const int PART_NBR = 120;
const float PARTICLE_SIZE = 3.5;
std::vector<particle> particles;

const float MAX_DIST = 140;
const float MIN_DIST = 15;
const float SAFE_DIST = 30;

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();


SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Surface* save_surface = NULL;

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 28, 28, 28, 255);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load PNG texture

	// gTexture = loadTexture( "07-media/texture.png" );
	// if( gTexture == NULL )
	// {
	// 	printf( "Failed to load texture image!\n" );
	// 	success = false;
	// }

	return success;
}

void close()
{
	//Free loaded image

	// SDL_DestroyTexture( gTexture );
	// gTexture = NULL;

	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int main( int argc, char* args[] )
{
    srand (time(NULL));
     //////////// SETUP
	 //planets initialazation
    for(int i = 0; i < PLANET_NBR; i++) {
        Vector2D position ((rand() % SCREEN_WIDTH)/2 + SCREEN_WIDTH/4, (rand() % SCREEN_HEIGHT)/2 + SCREEN_HEIGHT/4);
		planets.push_back(position);
    }

	//particle system setup
	for (int i = 0; i < PART_NBR; i++)
	{
        Vector2D position (rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
		particles.push_back(particle(position));
	}

	//Start up SDL and create window
	init();
    loadMedia();
			bool quit = false;

			SDL_Event e;

	while( !quit )
	{
		//Handle events on queue
		while( SDL_PollEvent( &e ) != 0 )
		{
			if( e.type == SDL_QUIT )
			{
				quit = true;
			}
		}
        //////////// UPDATE
		for (int i = 0; i < PART_NBR; i++)
		{
			Vector2D force(0, 0);
			for (int j = 0; j < PLANET_NBR; j++)
			{
				Vector2D dir = planets[j] - particles[i].pos;
				float distance = dir.length();
				float lenght = SDL_clamp(distance, MIN_DIST, MAX_DIST);
				float strenght = G/(lenght*lenght);
				dir.normalize();

				// force += (j % 2 == 0) ? (dir * strenght) : (dir * strenght * (-1));

				force += (distance > SAFE_DIST) ? (dir * strenght) : (dir * strenght * (-1));
			}
			
			particles[i].upply_force(force);
			particles[i].update();
		}
		
		
		SDL_SetRenderDrawColor( gRenderer, 28, 28, 28, 255);
		SDL_RenderClear( gRenderer );
        //////////// DRAW
		if(DRAW_PLANETS)
		{
			for (int i = 0; i < PLANET_NBR; i++)
			{
				filledCircleRGBA(gRenderer, planets[i].getX(), planets[i].getY(), PLANET_SIZE, 255,120,120,255);
			}
		}

		for(int i = 0; i < PART_NBR; i++)
		{
			thickLineRGBA(gRenderer, particles[i].pos.getX(), particles[i].pos.getY(), particles[i].prev_pos[PREV_POS_NBR-1].getX(), particles[i].prev_pos[PREV_POS_NBR-1].getY(), PARTICLE_SIZE, 255,255,255,255);

			// filledCircleRGBA(gRenderer, particles[i].prev_pos[PREV_POS_NBR-1].getX(), particles[i].prev_pos[PREV_POS_NBR-1].getY(), PARTICLE_SIZE, 255,0,0,255);
			// filledCircleRGBA(gRenderer, particles[i].pos.getX(), particles[i].pos.getY(), PARTICLE_SIZE, 255,255,255,255);
		}
		
		//Update screen
		SDL_RenderPresent( gRenderer );

		//record rection
		// save_surface = SDL_GetWindowSurface(gWindow);
		// // IMG_SaveJPG(save_surface, std::format("movio/{}.png",framecount));
		// IMG_SavePNG(save_surface, "movio/mmm.png");

		// SDL_Surface* sshot = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
		// SDL_RenderReadPixels(gRenderer, NULL, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);

		// std::string filename = "movio/" + std::to_string(framecount) + ".png";
		// int lenght = filename.length();
		// char* char_filename = new char[lenght + 1];
		// strcpy(char_filename, filename.c_str());
		// IMG_SavePNG(sshot, char_filename);
		// SDL_FreeSurface(sshot);
		// delete[] char_filename;

		framecount++;
	}


	//Free resources and close SDL
	close();

	return 0;
}