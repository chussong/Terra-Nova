#ifndef GAMEVARS_HPP
#define GAMEVARS_HPP

#include <string>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

// Game state and communication

enum order_t { ORDER_PATROL = 1, ORDER_ADVANCE = 2, ORDER_HARVEST = 3 };

enum signal_t { ERROR = 0, SELECTED = 100, NEXT_TURN = 200, TRY_BUILD = 300,
	SCREEN_CHANGE = 400, QUIT = 500, NOTHING = 600, SIG_GIVE_ORDER = 700,
	SIG_ORDER_PATROL = 701, SIG_ORDER_ADVANCE = 702, SIG_ORDER_HARVEST = 703};

// graphics

constexpr int SCREEN_WIDTH = 1024;
constexpr int SCREEN_HEIGHT = 768;

constexpr auto DEFAULT_FONT = "./Ubuntu-R.ttf";
constexpr int DEFAULT_FONT_SIZE = 20;
constexpr auto UI_FONT = DEFAULT_FONT;
constexpr int UI_FONT_SIZE = 20;
//constexpr auto UI_FONT = "/usr/share/fonts/truetype/ubuntu-font-family/Ubuntu-R.ttf";
constexpr auto DIALOGUE_FONT_NORMAL = DEFAULT_FONT;
constexpr int DIALOGUE_FONT_SIZE = 30;

extern TTF_Font* defaultFont;
extern TTF_Font* uiFont;
extern TTF_Font* dialogueFont;

inline TTF_Font* DefaultFont() { return defaultFont; }
inline TTF_Font* UIFont() { return uiFont ? uiFont : DefaultFont(); }
inline TTF_Font* DialogueFont() { return dialogueFont ? dialogueFont : DefaultFont(); }

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
constexpr int RES_PANEL_X = 200;
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

constexpr int INFO_PANEL_WIDTH = 410;
constexpr int INFO_PANEL_HEIGHT = 160;
constexpr int INFO_PANEL_X = SCREEN_WIDTH - INFO_PANEL_WIDTH;
constexpr int INFO_PANEL_Y = SCREEN_HEIGHT - INFO_PANEL_HEIGHT;
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
constexpr int UNIT_NAME_W = 199;
constexpr int UNIT_NAME_H = 48;
constexpr int UNIT_HEALTH_X = 140;
constexpr int UNIT_HEALTH_Y = 25;
constexpr int UNIT_HEALTH_W = 199;
constexpr int UNIT_HEALTH_H = 48;
constexpr int UNIT_ATTACK_X = 145;
constexpr int UNIT_ATTACK_Y = 25;
constexpr int UNIT_ATTACK_W = 199;
constexpr int UNIT_ATTACK_H = 48;

constexpr int BUILDING_INFO_PORTRAIT_WIDTH = 150;
constexpr int BUILDING_INFO_ICON_WIDTH = 50;
constexpr int BUILDING_INFO_ROW_HEIGHT = 50;

constexpr int ORDER_PANEL_WIDTH = 155;
constexpr int ORDER_PANEL_HEIGHT = 155;
constexpr int ORDER_BUTTON_WIDTH = 45;
constexpr int ORDER_BUTTON_HEIGHT = 45;

constexpr int DIALOGUE_BOX_WIDTH = 800;
constexpr int DIALOGUE_BOX_HEIGHT = 200;
constexpr int DIALOGUE_BOX_X = (SCREEN_WIDTH - DIALOGUE_BOX_WIDTH)/2;
constexpr int DIALOGUE_BOX_Y = SCREEN_HEIGHT - DIALOGUE_BOX_HEIGHT;

constexpr int SUBWINDOW_BUTTON_SPACING = 25;

// misc UI

enum button_t { END_TURN = 0, BUILDING = 1, LEAVE_COLONY = 2 };

enum textcolor_t { BLACK = 0, RED = 1, BLUE = 2, GREEN = 3,
	LAST_COLOR = 4 };

constexpr char NO_FACTION = 0;

#endif
