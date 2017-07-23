#ifndef COLONY_HPP
#define COLONY_HPP

#include <string>
#include <vector>
#include <array>
#include <memory>
#include <SDL.h>

#include "building.hpp"
#include "templates.hpp"
#include "tile.hpp"
#include "gamevars.hpp"

class Colony {
	//std::shared_ptr<UIElement> colonyBackground;
	// BuildingTypes should really be a Game-level constant
	std::vector<std::shared_ptr<BuildingType>> buildingTypes;

	SDL_Renderer* ren;

	std::string name;
	int faction;
	int row;
	int colm; // position on Map

	std::vector<std::vector<std::shared_ptr<Tile>>> terrain;

	std::array<int, LAST_RESOURCE> resources;
	std::array<int, LAST_RESOURCE> resourceCap;
	//std::array<int, LAST_RESOURCE> resPerTurn;
	int powerSupply;
	int powerDemand;

	std::vector<std::string> buildings;

	std::vector<std::shared_ptr<Building>> buildQueue;

	//std::array<std::shared_ptr<UIElement>, LAST_RESOURCE> resourcePanels;
	//std::shared_ptr<UIElement> buildingGrid;
	//std::shared_ptr<UIElement> endTurnButton;

	public:
		Colony() = delete;
		Colony(SDL_Renderer* ren, 
				std::vector<std::vector<std::shared_ptr<Tile>>> terrain,
				const int faction);

		void SetBuildingTypes(
				const std::vector<std::shared_ptr<BuildingType>> BuildingTypes);
		//const std::vector<std::shared_ptr<BuildingType>>& BuildingTypes() const
			//{ return BuildingTypes; }
		unsigned int NumberOfBuildingTypes() const { return buildingTypes.size(); }
		const std::shared_ptr<BuildingType>& KnownBuildingType(const int i) const
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
		void AddBuilding(const std::string Building);

		std::string Name() const;
		int Column() const;
		int Row() const;
		std::shared_ptr<Tile> Terrain(const unsigned int row, const unsigned int colm) const;
		unsigned int TerrainRows() const { return terrain.size(); }
		unsigned int RowWidth(unsigned int row) const { return terrain[row].size(); }
		const int& Resource(const resource_t resource) const;
		const int& Resource(const int resource) const;
		unsigned int NumberOfResources() const { return resources.size(); }
		std::string ResAsString(const int res) const;

		//void AssignWorker(Unit* worker, const Tile* location);

		void EnqueueBuilding(const BuildingType* type, Tile* destinationTile);
		void EnqueueBuilding(const unsigned int id, Tile* destinationTile);
		void AdvanceQueue();

		void ProcessTurn();

		static std::string ResourceName(const resource_t resource);
};

#endif
