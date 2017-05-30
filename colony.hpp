#ifndef COLONY_HPP
#define COLONY_HPP

#include <string>
#include <vector>
#include <array>
#include <memory>
#include <SDL.h>

#include "person.hpp"
#include "building.hpp"
#include "templates.hpp"
#include "map.hpp"
#include "tile.hpp"
#include "gfxobject.hpp"
#include "ui.hpp"
#include "gamewindow.hpp"
#include "gamevars.hpp"

class map;
class tile;
class gameWindow;
//class gfxObject;

class colony {
	std::shared_ptr<uiElement> colonyBackground;
	std::vector<std::shared_ptr<buildingType>> buildingTypes;

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

	std::vector<std::shared_ptr<building>> buildQueue;

	std::array<std::shared_ptr<uiElement>, LAST_RESOURCE> resourcePanels;
	std::shared_ptr<uiElement> buildingGrid;
	std::array<std::shared_ptr<buildingPrototype>, 
		BUILDING_GRID_ROWS*BUILDING_GRID_COLUMNS> buildingButtons;
	std::shared_ptr<uiElement> endTurnButton;

	void Clean();

	public:
		colony(SDL_Renderer* ren, std::shared_ptr<map> myMap,
				const int row, const int colm);

		void SetBuildingTypes(
				const std::vector<std::shared_ptr<buildingType>> buildingTypes);

		void ChangeName(const std::string name);
		void Move(const int xdist, const int ydist);
		int  AddResource(const resource_t resource, int amount);
		std::array<int, LAST_RESOURCE> AddResources(
				const std::array<int, LAST_RESOURCE>& income);
		int  TakeResource(const resource_t resource, int amount);
		void SetResourceIncome(const resource_t resource, int amount);
		void AddResourceIncome(const resource_t resource, int amount);
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
		void AssignWorker(std::shared_ptr<person> worker,
				const std::shared_ptr<tile> location);

		void EnqueueBuilding(const std::shared_ptr<buildingType> type,
				std::shared_ptr<tile> destinationTile);
		void EnqueueBuilding(const unsigned int id,
				std::shared_ptr<tile> destinationTile);
		void AdvanceQueue();

		void ProcessTurn();

		void MakeColonyScreen(std::shared_ptr<gameWindow> win);
		void DrawTiles(std::shared_ptr<gameWindow> win);
		void DrawResources(std::shared_ptr<gameWindow> win);
		void DrawColonists(std::shared_ptr<gameWindow> win);
		void DrawColonyMisc(std::shared_ptr<gameWindow> win);
		
		static std::string ResourceName(const resource_t resource);
};

#endif
