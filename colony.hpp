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
class tileType;
class gameWindow;
class buildingPrototype;
class buildingType;
class uiElement;
class building;
//class gfxObject;

class colony {
	std::shared_ptr<uiElement> colonyBackground;
	// buildingTypes should really be a game-level constant
	std::vector<std::shared_ptr<buildingType>> buildingTypes;

	SDL_Renderer* ren;

	std::string name;
	int faction;
	int row;
	int colm; // position on map

	std::vector<std::vector<std::shared_ptr<tile>>> terrain;

	std::array<int, LAST_RESOURCE> resources;
	std::array<int, LAST_RESOURCE> resourceCap;
	//std::array<int, LAST_RESOURCE> resPerTurn;
	int powerSupply;
	int powerDemand;

	std::vector<std::string> buildings;

	std::vector<std::shared_ptr<building>> buildQueue;

	//std::array<std::shared_ptr<uiElement>, LAST_RESOURCE> resourcePanels;
	std::shared_ptr<uiElement> buildingGrid;
	//std::shared_ptr<uiElement> endTurnButton;

	public:
		colony() = delete;
		colony(SDL_Renderer* ren, 
				std::vector<std::vector<std::shared_ptr<tile>>> terrain,
				const int faction);

		void SetBuildingTypes(
				const std::vector<std::shared_ptr<buildingType>> buildingTypes);
		//const std::vector<std::shared_ptr<buildingType>>& BuildingTypes() const
			//{ return buildingTypes; }
		unsigned int NumberOfBuildingTypes() const { return buildingTypes.size(); }
		const std::shared_ptr<buildingType>& BuildingType(const int i) const
			{ return buildingTypes[i]; }

		void ChangeName(const std::string name);
		int Owner() const { return faction; }
		void Move(const int xdist, const int ydist);
		int  AddResource(const resource_t resource, int amount);
		std::array<int, LAST_RESOURCE> AddResources(
				const std::array<int, LAST_RESOURCE>& income);
		int  TakeResource(const resource_t resource, int amount);
		//void SetResourceIncome(const resource_t resource, int amount);
		//void AddResourceIncome(const resource_t resource, int amount);
		void AddBuilding(const std::string building);

		std::string Name() const;
		int Column() const;
		int Row() const;
		std::shared_ptr<tile> Terrain(const unsigned int row, const unsigned int colm) const;
		unsigned int TerrainRows() const { return terrain.size(); }
		unsigned int RowWidth(unsigned int row) const { return terrain[row].size(); }
		const int& Resource(const resource_t resource) const;
		const int& Resource(const int resource) const;
		unsigned int NumberOfResources() const { return resources.size(); }
		std::string ResAsString(const int res) const;

		void AssignWorker(person* worker, const tile* location);

		void EnqueueBuilding(const buildingType* type, tile* destinationTile);
		void EnqueueBuilding(const unsigned int id, tile* destinationTile);
		void AdvanceQueue();

		void ProcessTurn();

		static std::string ResourceName(const resource_t resource);
};

#endif
