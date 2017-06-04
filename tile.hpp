#ifndef TILE_HPP
#define TILE_HPP

#include "entity.hpp"
#include "building.hpp"
#include "person.hpp"

class person;
class building;
class tile : public entity {
	terrain_t tileType;
	std::shared_ptr<building> bldg;
	std::vector<std::shared_ptr<person>> occupants;

	int row;
	int colm;

	public:
		tile() = delete;
		tile(terrain_t tileType, SDL_Renderer* ren, const std::string spriteFile,
				const int row, const int colm);
		tile(const entity& other) = delete;
		tile(tile&& other) noexcept : 
			entity(std::move(other)), tileType(other.tileType), bldg(other.bldg) {}
		tile& operator=(const entity& other) = delete;

		void Render() const;
		void MoveTo(int x, int y);
		void MoveTo(SDL_Rect newLayout);
		void Resize(int w, int h);
		void Resize(SDL_Rect newLayout);

		bool InsideQ(const int x, const int y) const;
		int Select();
		terrain_t TileType() const;
		void SetTileType(const terrain_t newType);
		std::array<int, LAST_RESOURCE> Income() const;

		void SetLocation(const int row, const int colm);
		int Row() const;
		int Colm() const;

		void AddBuilding(std::shared_ptr<building> newBldg);
		void RemoveBuilding();

		bool AddOccupant(std::shared_ptr<person> newOccupant);
		bool RemoveOccupant(std::shared_ptr<person> removeThis);
		std::vector<std::shared_ptr<person>> Occupants() const;
		std::shared_ptr<person> Defender() const;
		char Owner() const;

		void Training();
};

#endif
