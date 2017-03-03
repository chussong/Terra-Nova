#ifndef COLONY_HPP
#define COLONY_HPP

#include <string>
#include <vector>
#include <array>
#include <memory>

#include "person.hpp"
#include "templates.hpp"
#include "map.hpp"

class map;
class gfxObject;

constexpr int RESCAP = 100;
enum resource_t { FOOD = 0, CARBON = 1, SILICON = 2, IRON = 3,
	LAST_RESOURCE = 4 };

class colony {
	std::string name;
	std::shared_ptr<map> myMap;
	int row;
	int colm; // position on map

	std::array<terrain_t, 18> terrain;

	std::array<int, LAST_RESOURCE> resources;
	std::array<int, LAST_RESOURCE> resourceCap;
	std::array<int, LAST_RESOURCE> resPerTurn;
	int powerSupply;
	int powerDemand;

	std::vector<std::weak_ptr<person>> inhabitants;
	std::vector<std::string> buildings;
	std::vector<std::weak_ptr<person>> stagingArea;

	void Clean();

	public:
		colony(std::shared_ptr<map> myMap, const int row, const int colm);

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
};

#endif
