#ifndef COLONY_HPP
#define COLONY_HPP

#include <string>
#include <vector>
#include <array>
#include <memory>
#include <SDL.h>

#include "person.hpp"
#include "templates.hpp"
#include "map.hpp"
#include "tile.hpp"
#include "gfxobject.hpp"
#include "ui.hpp"

class map;
class tile;
//class gfxObject;

constexpr int RESCAP = 100;
enum resource_t { FOOD = 0, CARBON = 1, SILICON = 2, IRON = 3,
	LAST_RESOURCE = 4 };

constexpr int MAPDISP_ORIGIN_X = 300;
constexpr int MAPDISP_ORIGIN_Y = 300;
constexpr int TILE_WIDTH = 87;
constexpr int TILE_HEIGHT = 75;
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
const std::string COLONY_BACKGROUND = "Space-Colony.png";

class colony {
	std::shared_ptr<uiElement> colonyBackground;

	SDL_Renderer* ren;

	std::string name;
	std::shared_ptr<map> myMap;
	int row;
	int colm; // position on map

	std::array<std::shared_ptr<tile>, 18> terrain;

	std::array<int, LAST_RESOURCE> resources;
	std::array<int, LAST_RESOURCE> resourceCap;
	std::array<int, LAST_RESOURCE> resPerTurn;
	int powerSupply;
	int powerDemand;

	std::vector<std::weak_ptr<person>> inhabitants;
	std::vector<std::string> buildings;
	std::vector<std::weak_ptr<person>> stagingArea;

	std::array<std::shared_ptr<uiElement>, LAST_RESOURCE> resourcePanels;

	void Clean();

	public:
		colony(SDL_Renderer* ren, std::shared_ptr<map> myMap,
				const int row, const int colm);

		void ChangeName(const std::string name);
		void Move(const int xdist, const int ydist);
		int  AddResource(const resource_t resource, int amount);
		int  TakeResource(const resource_t resource, int amount);
		void SetResourceIncome(const resource_t resource, int amount);
		void AddInhabitant(std::shared_ptr<person> inhabitant);
		void AddBuilding(const std::string building);

		std::string Name() const;
		int Column() const;
		int Row() const;
		terrain_t Terrain(const unsigned int num) const;
		int Resource(const resource_t resource) const;
		std::string ResAsString(const int res) const;
		std::shared_ptr<person> Inhabitant(const int number);
		const std::shared_ptr<person> Inhabitant(const int number) const;

		void ProcessTurn();

		void MakeColonyScreen(std::shared_ptr<gameWindow> win);
		void DrawTiles(std::shared_ptr<gameWindow> win);
		void DrawResources(std::shared_ptr<gameWindow> win);
		void DrawColonists(std::shared_ptr<gameWindow> win);
		void DrawColonyMisc(std::shared_ptr<gameWindow> win);
		
		static std::string ResourceName(const resource_t resource);
};

#endif
