#ifndef TILE_HPP
#define TILE_HPP

#include "entity.hpp"

constexpr int TILE_WIDTH = 87;
constexpr int TILE_HEIGHT = 75;

enum terrain_t { OCEAN = 0, COAST = 1, PLAINS = 10, MOUNTAIN = 100 };

enum resource_t { FOOD = 0, CARBON = 1, SILICON = 2, IRON = 3,
	LAST_RESOURCE = 4 };

class tile : public entity {
	terrain_t tileType;

	public:
		tile() = delete;
		tile(terrain_t tileType, SDL_Renderer* ren, const std::string spriteFile,
				const int x, const int y);
		tile(const entity& other) = delete;
		tile(tile&& other) noexcept : 
			entity(std::move(other)), tileType(other.tileType) {}
		tile& operator=(const entity& other) = delete;

		bool InsideQ(const int x, const int y) const;
		terrain_t TileType() const;
		std::array<int, LAST_RESOURCE> Income() const;
};

#endif
