#ifndef GAMEVARS_HPP
#define GAMEVARS_HPP

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

// game state and communication

enum signal_t { ERROR = 0, SELECTED = 100, NEXT_TURN = 200, TRY_BUILD = 300,
	SCREEN_CHANGE = 400, QUIT = 500};

enum screentype_t { QUIT_SCREEN = 0, COLONY_SCREEN = 100, MAP_SCREEN = 200 };

// graphics

constexpr int SCREEN_WIDTH = 1024;
constexpr int SCREEN_HEIGHT = 768;
/*constexpr int MAPDISP_ORIGIN_X = 300;
constexpr int MAPDISP_ORIGIN_Y = 300;
constexpr int TILE_WIDTH = 87;
constexpr int TILE_HEIGHT = 75;
constexpr int RESOURCE_X = SCREEN_WIDTH-400;
constexpr int RESOURCE_Y = SCREEN_HEIGHT-100;
constexpr int RESOURCE_WIDTH = 90;

const std::string COLONY_BACKGROUND = "Space-Colony.png";*/
constexpr auto DEFAULT_FONT = "/usr/share/fonts/truetype/ubuntu-font-family/Ubuntu-R.ttf";
constexpr int DEFAULT_FONT_SIZE = 20;

// tiles and terrain

constexpr int TILE_WIDTH = 87;
constexpr int TILE_HEIGHT = 75;

enum terrain_t { OCEAN = 0, COAST = 1, PLAINS = 2, MOUNTAIN = 3,
	LAST_TERRAIN = 4 };

// from the map

constexpr int DEFAULT_WIDTH = 100;
constexpr int DEFAULT_HEIGHT = 100;

// colony-related

enum resource_t { FOOD = 0, CARBON = 1, SILICON = 2, IRON = 3,
	LAST_RESOURCE = 4 };

constexpr int RESCAP = 100;

constexpr int MAPDISP_ORIGIN_X = 300;
constexpr int MAPDISP_ORIGIN_Y = 300;
constexpr int TILE_X[18] = {
	MAPDISP_ORIGIN_X + TILE_WIDTH,		// inner ring
	MAPDISP_ORIGIN_X + TILE_WIDTH/2,
	MAPDISP_ORIGIN_X - TILE_WIDTH/2,
	MAPDISP_ORIGIN_X - TILE_WIDTH,
	MAPDISP_ORIGIN_X - TILE_WIDTH/2,
	MAPDISP_ORIGIN_X + TILE_WIDTH/2,
	MAPDISP_ORIGIN_X + 2*TILE_WIDTH,	// outer ring
	MAPDISP_ORIGIN_X + 3*TILE_WIDTH/2,
	MAPDISP_ORIGIN_X + TILE_WIDTH,
	MAPDISP_ORIGIN_X,
	MAPDISP_ORIGIN_X - TILE_WIDTH,
	MAPDISP_ORIGIN_X - 3*TILE_WIDTH/2,
	MAPDISP_ORIGIN_X - 2*TILE_WIDTH,
	MAPDISP_ORIGIN_X - 3*TILE_WIDTH/2,
	MAPDISP_ORIGIN_X - TILE_WIDTH,
	MAPDISP_ORIGIN_X,
	MAPDISP_ORIGIN_X + TILE_WIDTH,
	MAPDISP_ORIGIN_X + 3*TILE_WIDTH/2,
};
constexpr int TILE_Y[18] = {
	MAPDISP_ORIGIN_Y,					// inner ring
	MAPDISP_ORIGIN_Y + TILE_HEIGHT,
	MAPDISP_ORIGIN_Y + TILE_HEIGHT,
	MAPDISP_ORIGIN_Y,
	MAPDISP_ORIGIN_Y - TILE_HEIGHT,
	MAPDISP_ORIGIN_Y - TILE_HEIGHT,
	MAPDISP_ORIGIN_Y,					// outer ring
	MAPDISP_ORIGIN_Y + TILE_HEIGHT,
	MAPDISP_ORIGIN_Y + 2*TILE_HEIGHT,
	MAPDISP_ORIGIN_Y + 2*TILE_HEIGHT,
	MAPDISP_ORIGIN_Y + 2*TILE_HEIGHT,
	MAPDISP_ORIGIN_Y + TILE_HEIGHT,
	MAPDISP_ORIGIN_Y,
	MAPDISP_ORIGIN_Y - TILE_HEIGHT,
	MAPDISP_ORIGIN_Y - 2*TILE_HEIGHT,
	MAPDISP_ORIGIN_Y - 2*TILE_HEIGHT,
	MAPDISP_ORIGIN_Y - 2*TILE_HEIGHT,
	MAPDISP_ORIGIN_Y - TILE_HEIGHT,
};

constexpr int RES_PANEL_WIDTH = 90;
constexpr int RES_PANEL_HEIGHT = 90;
constexpr int RES_PANEL_X = SCREEN_WIDTH - LAST_RESOURCE*RES_PANEL_WIDTH;
constexpr int RES_PANEL_Y = SCREEN_HEIGHT - RES_PANEL_HEIGHT;

constexpr int BUILDING_WIDTH = 74;
constexpr int BUILDING_HEIGHT = 74;
constexpr int BUILDING_GRID_PADDING = 3;
constexpr int BUILDING_GRID_ROWS = 3;
constexpr int BUILDING_GRID_COLUMNS = 3;

constexpr int PERSON_WIDTH = 42;
constexpr int PERSON_HEIGHT = 42;

const std::string COLONY_BACKGROUND = "Space-Colony";

// misc UI

enum button_t { END_TURN = 0, BUILDING = 1, LEAVE_COLONY = 2 };

extern TTF_Font* defaultFont;
enum textcolor_t { BLACK = 0, RED = 1, BLUE = 2, GREEN = 3,
	LAST_COLOR = 4 };

#endif
