#ifndef COLONY_HPP
#define COLONY_HPP

#include <string>
#include <vector>
#include <array>
#include <memory>

#include "person.hpp"
#include "templates.hpp"
#include "map.hpp"
#include "gamewindow.hpp"

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

	std::array<terrain_t, 6> innerRing;
	std::array<terrain_t, 12> outerRing;

	std::array<int, LAST_RESOURCE> resources;
	std::array<int, LAST_RESOURCE> resourceCap;
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
		void AddInhabitant(std::shared_ptr<person> inhabitant);
		void AddBuilding(const std::string building);

		std::string Name() const;
		int Column() const;
		int Row() const;
		int Resource(const resource_t resource) const;
		std::shared_ptr<person> Inhabitant(const int number);
		const std::shared_ptr<person> Inhabitant(const int number) const;

		std::vector<gfxObject> InnerRing(const int colm, const int row,
				SDL_Renderer* ren) const;
		std::vector<gfxObject> OuterRing(const int colm, const int row,
				SDL_Renderer* ren) const;
};

#endif
