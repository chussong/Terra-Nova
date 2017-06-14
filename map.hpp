#ifndef MAP_HPP
#define MAP_HPP

#include <vector>
#include <memory>
#include <string>
#include "templates.hpp"
#include "gamevars.hpp"
#include "tile.hpp"
#include "colony.hpp"

class colony;
class person;

class map {
	SDL_Renderer* ren;

	int width;
	int height;
	/* Terrain format: each entry of terrain is a vector corresponding to a row
	 * of the hex grid. Each of these rows is a vector of tiles which is twice
	 * as long as the number of tiles it actually contains: odd rows have only
	 * odd entries and even rows have only even entries.*/
	std::vector<std::vector<std::shared_ptr<tile>>> terrain;
	std::vector<std::weak_ptr<colony>> colonies;
	std::vector<std::weak_ptr<person>> roamers;

	void Clean();

	void InitTerrain(std::vector<std::shared_ptr<tileType>> types);

	public:
		map() = delete;
		map(SDL_Renderer* ren, std::vector<std::shared_ptr<tileType>> types);
		map(SDL_Renderer* ren, std::vector<std::vector<std::shared_ptr<tile>>> tiles):
			ren(ren), terrain(tiles) {}

		void AddColony(const std::shared_ptr<colony> colony, int row, int colm);
		std::shared_ptr<colony> Colony(const int num);
		const std::shared_ptr<colony> Colony(const int num) const;

		std::shared_ptr<tile> Terrain(const int row, const int column) const;
		std::vector<std::vector<std::shared_ptr<tile>>> SurroundingTerrain(
				const int row, const int colm);
		unsigned int NumberOfRows() const;
		unsigned int NumberOfColumns() const;

		void MoveView(direction_t dir);

		std::string TerrainName(const unsigned int x, const unsigned int y);
};
#endif
