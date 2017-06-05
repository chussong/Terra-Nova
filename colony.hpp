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
#include "tile.hpp"
#include "gfxobject.hpp"
#include "ui.hpp"
#include "gamewindow.hpp"
#include "gamevars.hpp"

class tile;
class gameWindow;
//class gfxObject;

class colony {
	std::shared_ptr<uiElement> colonyBackground;
	std::vector<std::shared_ptr<buildingType>> buildingTypes;

	SDL_Renderer* ren;

	std::string name;
	int faction;
	int row;
	int colm; // position on map

	std::vector<std::vector<std::shared_ptr<tile>>> terrain;

	std::array<int, LAST_RESOURCE> resources;
	std::array<int, LAST_RESOURCE> resourceCap;
	std::array<int, LAST_RESOURCE> resPerTurn;
	int powerSupply;
	int powerDemand;

	std::vector<std::string> buildings;

	std::vector<std::shared_ptr<building>> buildQueue;

	std::array<std::shared_ptr<uiElement>, LAST_RESOURCE> resourcePanels;
	std::shared_ptr<uiElement> buildingGrid;
	std::array<std::shared_ptr<buildingPrototype>, 
		BUILDING_GRID_ROWS*BUILDING_GRID_COLUMNS> buildingButtons;
	std::shared_ptr<uiElement> leaveColonyButton;
	std::shared_ptr<uiElement> endTurnButton;

	public:
		colony() = delete;
		colony(SDL_Renderer* ren, 
				std::vector<std::vector<std::shared_ptr<tile>>> terrain,
				const int faction);

		void SetBuildingTypes(
				const std::vector<std::shared_ptr<buildingType>> buildingTypes);

		void ChangeName(const std::string name);
		int Owner() const { return faction; }
		void Move(const int xdist, const int ydist);
		int  AddResource(const resource_t resource, int amount);
		std::array<int, LAST_RESOURCE> AddResources(
				const std::array<int, LAST_RESOURCE>& income);
		int  TakeResource(const resource_t resource, int amount);
		void SetResourceIncome(const resource_t resource, int amount);
		void AddResourceIncome(const resource_t resource, int amount);
		void AddBuilding(const std::string building);

		std::string Name() const;
		int Column() const;
		int Row() const;
		terrain_t Terrain(const unsigned int row, const unsigned int colm) const;
		int Resource(const resource_t resource) const;
		std::string ResAsString(const int res) const;

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
		int  TileX(const unsigned int row, const unsigned int colm);
		int  TileY(const unsigned int row);
		void DrawResources(std::shared_ptr<gameWindow> win);
		//void DrawColonists(std::shared_ptr<gameWindow> win);
		void DrawColonyMisc(std::shared_ptr<gameWindow> win);
		
		static std::string ResourceName(const resource_t resource);
};

#endif
