#ifndef GAMEVARS_HPP
#define GAMEVARS_HPP

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

// Game state and communication

enum order_t { ORDER_PATROL = 1, ORDER_ADVANCE = 2, ORDER_HARVEST = 3 };

enum signal_t { ERROR = 0, SELECTED = 100, NEXT_TURN = 200, TRY_BUILD = 300,
	SCREEN_CHANGE = 400, QUIT = 500, NOTHING = 600, SIG_GIVE_ORDER = 700,
	SIG_ORDER_PATROL = 701, SIG_ORDER_ADVANCE = 702, SIG_ORDER_HARVEST = 703};

enum screentype_t { QUIT_SCREEN = 0, COLONY_SCREEN = 100, MAP_SCREEN = 200 };

// graphics

constexpr int SCREEN_WIDTH = 1024;
constexpr int SCREEN_HEIGHT = 768;

constexpr auto DEFAULT_FONT = "/usr/share/fonts/truetype/ubuntu-font-family/Ubuntu-R.ttf";
constexpr int DEFAULT_FONT_SIZE = 20;

// Tiles and terrain

struct TileAttributes {
	bool aquatic = false;
	bool cold = false;
	bool flat = false;
	bool hilly = false;
	bool wooded = false;
};

constexpr int TILE_WIDTH = 87;
constexpr int TILE_HEIGHT = 75;\

// from the Map

enum direction_t { VIEW_RIGHT = 0, VIEW_UP = 1, VIEW_LEFT = 2, VIEW_DOWN = 3 };

constexpr int DEFAULT_WIDTH = 100;
constexpr int DEFAULT_HEIGHT = 100;

// Colony screen-related

enum resource_t { FOOD = 0, CARBON = 1, SILICON = 2, IRON = 3,
	LAST_RESOURCE = 4 };

constexpr int RESCAP = 100;

const std::string COLONY_BACKGROUND = "Space-Colony";

// UI sizes and positioning

constexpr int MAPDISP_ORIGIN_X = 300;
constexpr int MAPDISP_ORIGIN_Y = 300;
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
constexpr int HEALTH_BAR_WIDTH = 40;
constexpr int HEALTH_BAR_HEIGHT = 10;

constexpr int UNIT_INFO_PANEL_WIDTH = 410;
constexpr int UNIT_INFO_PANEL_HEIGHT = 160;
constexpr int PORTRAIT_X = 5;
constexpr int PORTRAIT_Y = 5;
constexpr int FACTIONCOLOR_X = 156;
constexpr int FACTIONCOLOR_Y = 6;
constexpr int HEALTHICON_X = 156;
constexpr int HEALTHICON_Y = 56;
constexpr int WEAPONICON_X = 156;
constexpr int WEAPONICON_Y = 106;
constexpr int UNIT_NAME_X = 140;
constexpr int UNIT_NAME_Y = 25;
constexpr int UNIT_HEALTH_X = 140;
constexpr int UNIT_HEALTH_Y = 25;
constexpr int UNIT_ATTACK_X = 145;
constexpr int UNIT_ATTACK_Y = 25;

constexpr int ORDER_PANEL_WIDTH = 155;
constexpr int ORDER_PANEL_HEIGHT = 155;
constexpr int ORDER_BUTTON_WIDTH = 45;
constexpr int ORDER_BUTTON_HEIGHT = 45;

// misc UI

enum button_t { END_TURN = 0, BUILDING = 1, LEAVE_COLONY = 2 };

extern TTF_Font* defaultFont;
enum textcolor_t { BLACK = 0, RED = 1, BLUE = 2, GREEN = 3,
	LAST_COLOR = 4 };

#endif
