#ifndef TILE_HPP
#define TILE_HPP

#include "entity.hpp"
#include "building.hpp"
#include "person.hpp"

class person;
class tile : public entity {
	terrain_t tileType;
	std::shared_ptr<building> bldg;
	std::vector<std::shared_ptr<person>> occupants;

	public:
		tile() = delete;
		tile(terrain_t tileType, SDL_Renderer* ren, const std::string spriteFile,
				const int x, const int y);
		tile(const entity& other) = delete;
		tile(tile&& other) noexcept : 
			entity(std::move(other)), tileType(other.tileType), bldg(other.bldg) {}
		tile& operator=(const entity& other) = delete;

		void Render() const;

		bool InsideQ(const int x, const int y) const;
		terrain_t TileType() const;
		std::array<int, LAST_RESOURCE> Income() const;

		void AddBuilding(std::shared_ptr<building> newBldg);
		void RemoveBuilding();

		bool AddOccupant(std::shared_ptr<person> newOccupant);
		bool RemoveOccupant(std::shared_ptr<person> removeThis);
};

#endif
