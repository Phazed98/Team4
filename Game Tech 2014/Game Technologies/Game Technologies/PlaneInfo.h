//Added by Sam (Matt's Idea)
//.h file is being used to store and provide easy access to the information of the planes/tiles
//Kind of a hacky method buts its the easiest to work with, and allows quick changes that should
//filter down to all relevant places in the program

//the distance from the centre of the game world X/Y axes to the centre of each tile
#define TILE_CENTRE_OFFSET 500
//sizes of tiles
#define TILE_HEIGHT 100
#define TILE_WIDTH 200
#define TILE_HALF_WIDTH 100 //needed to save computation
#define TILE_DEPTH 400


//distance from the centre of the game world X/Y axes to the surface of a tile....TILE_CENTRE_OFFSET - TILE_HEIGHT
#define SURFACE_CENTRE_OFFSET 400