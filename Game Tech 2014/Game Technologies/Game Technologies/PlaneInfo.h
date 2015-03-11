//Added by Sam (Matt's Idea)
//.h file is being used to store and provide easy access to the information of the planes/tiles
//Kind of a hacky method buts its the easiest to work with, and allows quick changes that should
//filter down to all relevant places in the program

//the distance from the centre of the game world X/Y axes to the centre of each tile
#define TILE_CENTRE_OFFSET 400
//sizes of tiles
#define TILE_HEIGHT 20
#define TILE_WIDTH 200
#define TILE_HALF_WIDTH 100 //needed to save computation
#define TILE_DEPTH 400



#define INITIAL_Z_POSITION -8000 // This is the Z value where tiles/planes/blocks/tracks start spawning
#define END_POSITION 2000 // This is the Z value where tiles/planes/blocks/tracks stop moving and reset


//distance from the centre of the game world X/Y axes to the surface of a tile....( TILE_CENTRE_OFFSET - TILE_HEIGHT - 40 for correction )
#define SURFACE_CENTRE_OFFSET 340

//Maximum rendered tiles per plane at a time (including behind player to the point of generation in front of player)
#define MAX_NUM_TILES_PER_PLANE 10