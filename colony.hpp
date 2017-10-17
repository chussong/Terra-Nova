#ifndef COLONY_HPP
#define COLONY_HPP

#include <string>
#include <vector>
#include <array>
#include <memory>
#include <SDL.h>

#include "templates.hpp"
#include "gamevars.hpp"
#include "faction.hpp"

namespace TerraNova {

class Colony {
	SDL_Renderer* ren;

	std::string name;
	int faction;
	int row;
	int colm; // position on Map

	std::array<int, LAST_RESOURCE> resources;
	std::array<int, LAST_RESOURCE> resourceCap;
	int powerSupply;
	int powerDemand;

	public:
		Colony() = delete;
		Colony(SDL_Renderer* ren, const int faction);

		void ChangeName(const std::string name);
		int Faction() const { return faction; }
		void Move(const int xdist, const int ydist);
		int  AddResource(const resource_t resource, int amount);
		std::array<int, LAST_RESOURCE> AddResources(
				const std::array<int, LAST_RESOURCE>& amounts);
		int  TakeResource(const resource_t resource, int amount);
		std::array<int, LAST_RESOURCE> TakeResources(
				const std::array<int, LAST_RESOURCE>& amounts);

		std::string Name() const;
		int Column() const;
		void SetColumn(const int newColm) { colm = newColm; }
		int Row() const;
		void SetRow(const int newRow) { row = newRow; }
		const std::array<int, LAST_RESOURCE>& Resources() const;
		const int& Resource(const resource_t resource) const;
		const int& Resource(const int resource) const;
		unsigned int NumberOfResources() const { return resources.size(); }
		std::string ResAsString(const int res) const;

		void AdvanceQueue();

		void ProcessTurn();

		static std::string ResourceName(const resource_t resource);
};

} // namespace TerraNova
#endif
