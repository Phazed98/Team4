/******************************************************************************
Author:Rich Davison
Description: Random bits and pieces - too lazy to see if Sony have equivelents

-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*//////////////////////////////////////////////////////////////////////////////

#pragma once

//It's pi(ish)...
static const float		PI = 3.14159265358979323846f;

//It's pi...divided by 360.0f!
static const float		PI_OVER_360 = PI / 360.0f;

//Radians to degrees
static inline double RadToDeg(const double rad)	
{
	return rad * 180.0 / PI;
};

//Degrees to radians
static inline double DegToRad(const double deg)	
{
	return deg * PI / 180.0;
};

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

#define SHADERDIR	"/Shaders/"
#define MESHDIR		"/Meshes/"
#define TEXTUREDIR  "/Textures/"
#define SOUNDSDIR	"/Sounds/"

//Added by Sam (Matt's Idea)
//.h file is being used to store and provide easy access to the information of the planes/tiles
//Kind of a hacky method buts its the easiest to work with, and allows quick changes that should
//filter down to all relevant places in the program

//the distance from the centre of the game world X/Y axes to the centre of each tile
#define TILE_CENTRE_OFFSET 400
//sizes of tiles
#define TILE_HEIGHT 30
#define TILE_WIDTH 400
#define TILE_HALF_WIDTH 50 //needed to save computation
#define TILE_DEPTH 800
#define INITIAL_Z_POSITION -7550 // This is the Z value where tiles/planes/blocks/tracks start spawning
#define END_POSITION 2000 // This is the Z value where tiles/planes/blocks/tracks stop moving and reset


//distance from the centre of the game world X/Y axes to the surface of a tile....( TILE_CENTRE_OFFSET - TILE_HEIGHT - 40 for correction )
#define SURFACE_CENTRE_OFFSET 340