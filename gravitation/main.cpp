#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdio.h>
#include "time.h"
#include <vector>
#include <array>
#include "include/vector2d.h"
#include "include/math_utils.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int CELL_SIZE = 12;
const int COLS = floor(SCREEN_WIDTH / CELL_SIZE);
const int ROWS = floor(SCREEN_HEIGHT / CELL_SIZE);

float G = 4;

const float MAX_MASS = 10000;
const float MAX_PLANET_SIZE = 12;
int PLANET_NBR = 16;
std::vector<Vector2D> pl_pos;
std::vector<float> pl_mass;

const int PART_NBR = 200;
const float PARTICLE_SIZE = 2;
float MAX_SPEED = 12;
Vector2D MAX_SPEED_VEC (MAX_SPEED, MAX_SPEED);
std::vector<Vector2D> part_pos;
std::vector<Vector2D> part_vel;
std::vector<Vector2D> part_acc;

float FORCE_FIELD_DRAW_SCALE = 1;

struct _world
{
	std::vector<Vector2D> planets_pos;
	std::vector<float> planets_mass;
	std::array<std::array<Vector2D, ROWS>, COLS> force_field;

	_world(std::vector<Vector2D> arg_planets_pos, std::vector<float> arg_planets_mass) 
	{
		planets_pos = arg_planets_pos;
		planets_mass = arg_planets_mass;
	}

	void campute() 
	{
		for (int i = 0; i < COLS; i++)
		{
			for (int j = 0; j < ROWS; j++)
			{
				Vector2D force (0, 0);
				Vector2D cell_pos ((i+0.5)*CELL_SIZE, (j+0.5)*CELL_SIZE);
				for (int index = 0; index < planets_pos.size(); index++)
				{
					Vector2D dir = planets_pos[index] - cell_pos;
					float strenght = G * planets_mass[i] / (dir.length()*dir.length());

					dir.normalize();
					force += dir * strenght;
				}

				force_field[i][j] = force;
			}
			
		}
		
	}

	Vector2D get_force(Vector2D pos)
	{
		int x_index =  floor(pos.getX() / CELL_SIZE);
		int y_index =  floor(pos.getY() / CELL_SIZE);

		return force_field[x_index][y_index];
	}

	void draw_grid(SDL_Renderer* arg_renderer)
	{
		for (int i = 0; i < COLS; i++)
		{
			for (int j = 0; j < ROWS; j++)
			{
				rectangleRGBA(arg_renderer, i*CELL_SIZE, j*CELL_SIZE, (i+1)*CELL_SIZE, (j+1)*CELL_SIZE,255,255,255,255);
			}
			
		}
	}

	void draw_planets(SDL_Renderer* arg_renderer)
	{
		for (int i = 0; i < planets_pos.size(); i++)
		{
			float raduis = remap(planets_mass[i], 0, MAX_MASS,0, MAX_PLANET_SIZE);
			filledCircleRGBA(arg_renderer, planets_pos[i].getX(), planets_pos[i].getY(), raduis,255,255,255,255);
		}
	}

	void draw_force_field(SDL_Renderer* arg_renderer)
	{
		for (int i = 0; i < COLS; i++)
		{
			for (int j = 0; j < ROWS; j++)
			{
				lineRGBA(arg_renderer, (i+0.5)*CELL_SIZE, (j+0.5)*CELL_SIZE, (i+0.5)*CELL_SIZE + force_field[i][j].getX()*FORCE_FIELD_DRAW_SCALE, (j+0.5)*CELL_SIZE + force_field[i][j].getY()*FORCE_FIELD_DRAW_SCALE,255,120,255,255);
			}
			
		}
	}

	void print_force_field()
	{
		for (int i = 0; i < COLS; i++)
		{
			for (int j = 0; j < ROWS; j++)
			{
				printf("%d, %d\n", force_field[i][j].getX(), force_field[i][j].getY());
			}
			
		}
		
	}
};

struct _particle_system {
    std::vector<Vector2D> pos;
    std::vector<Vector2D> vel;
    std::vector<Vector2D> acc;

	_particle_system(std::vector<Vector2D> arg_pos, std::vector<Vector2D> arg_vel, std::vector<Vector2D> arg_acc)
	{
		pos = arg_pos;
		vel = arg_vel;
		acc = arg_acc;
	}

    void upply_force_field(_world arg_world)
    {
        for (int i = 0; i < PART_NBR; i++)
		{
			Vector2D force = arg_world.get_force(pos[i]);

			acc[i] += force;
		}
		
    }

    void update()
	{
		for (int i = 0; i < PART_NBR; i++)
		{
        	vel[i] += acc[i];
			vel[i] = Vector2Clamp(vel[i], MAX_SPEED_VEC, MAX_SPEED_VEC*(-1));
        	pos[i] += vel[i]*0.1;
        	acc[i] *= 0;

			if(pos[i].getX() < 0)
			{
				pos[i].setX(SCREEN_WIDTH);
			}
			if(pos[i].getX() > SCREEN_WIDTH)
			{
				pos[i].setX(0);
			}
			if(pos[i].getY() < 0)
			{
				pos[i].setY(SCREEN_HEIGHT);
			}
			if(pos[i].getY() > SCREEN_HEIGHT)
			{
				pos[i].setY(0);
			}
		}
    }

	void draw(SDL_Renderer* arg_Renderer)
	{
		for (int i = 0; i < PART_NBR; i++)
		{
			filledCircleRGBA(arg_Renderer, pos[i].getX(), pos[i].getY(), PARTICLE_SIZE,55,120,255,180);
		}
		
	}
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();


SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

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
	 //world setup
    for(int i = 0; i < PLANET_NBR; i++) {
        Vector2D position (rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
		pl_pos.push_back(position);

		float mass = rand() / (RAND_MAX + 1.) * MAX_MASS;
		pl_mass.push_back(mass);
    }

	 _world world = _world(pl_pos, pl_mass);
	 world.campute();

	//particle system setup
	for (int i = 0; i < PART_NBR; i++)
	{
        Vector2D position (rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
		part_pos.push_back(position);

		part_vel.push_back(Vector2D(0, 0));
		part_acc.push_back(Vector2D(0, 0));
	}

	_particle_system particle_system (part_pos, part_vel, part_acc);

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
		particle_system.upply_force_field(world);
		particle_system.update();
		
		SDL_SetRenderDrawColor( gRenderer, 28, 28, 28, 255);
		SDL_RenderClear( gRenderer );
        //////////// DRAW
		// world.draw_grid(gRenderer);
		world.draw_planets(gRenderer);
		world.draw_force_field(gRenderer);

		particle_system.draw(gRenderer);
		
		//Update screen
		SDL_RenderPresent( gRenderer );
	}


	//Free resources and close SDL
	close();

	return 0;
}