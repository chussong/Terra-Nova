#ifndef TILE_HPP
#define TILE_HPP

#include "entity.hpp"
#include "gamewindow.hpp"
#include "map.hpp" // should actually just have terrain_t enum in this file

class tile : public entity {
	terrain_t tileType;

	public:
		tile() = delete;
		tile(terrain_t tileType, SDL_Renderer* ren, const std::string spriteFile,
				const int x, const int y) : 
			entity(ren, spriteFile, x, y), tileType(tileType) {}
		tile(const entity& other) = delete;
		tile(tile&& other) noexcept : 
			entity(std::move(other)), tileType(other.tileType) {}
		tile& operator=(const entity& other) = delete;

		bool InsideQ(const int x, const int y) const;
};

#endif
