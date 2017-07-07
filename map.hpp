#ifndef MAP_HPP
#define MAP_HPP

#include <vector>
#include <memory>
#include <string>
#include "templates.hpp"
#include "gamevars.hpp"
#include "tile.hpp"
#include "colony.hpp"
#include "path.hpp"

class colony;
class person;
class tileType;
class tile;
class path;

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
	std::vector<person*> roamers;

	void Clean();

	void InitTerrain(std::vector<std::shared_ptr<tileType>> types);

	// path construction functions
	std::vector<std::array<unsigned int, 2>> ShortestPath(const unsigned int startRow, 
			const unsigned int startColm, const unsigned int destRow, 
			const unsigned int destColm, const moveCostTable& moveCosts);

	void UpdateNode(std::vector<std::vector<unsigned int>>& distMap,
			const unsigned int nodeRow, const unsigned int nodeColm, 
			const unsigned int value, const moveCostTable& moveCosts);

	std::array<unsigned int, 3> FindNextLowestOpen(
			const std::vector<std::vector<unsigned int>>& distMap,
			const std::vector<std::vector<bool>>& closedNodes,
			std::vector<std::vector<bool>>& checkedNodes,
			const std::array<unsigned int, 3> startingNode);

	std::vector<std::array<unsigned int, 2>> BuildPathVector(
			const std::vector<std::vector<unsigned int>>& distMap, 
			const unsigned int destRow, const unsigned int destColm);

	public:
		map() = delete;
		map(SDL_Renderer* ren, std::vector<std::shared_ptr<tileType>> types);
		map(SDL_Renderer* ren, std::vector<std::vector<std::shared_ptr<tile>>> tiles):
			ren(ren), terrain(tiles) {}

		void ProcessTurn();

		void AddColony(const std::shared_ptr<colony> colony, int row, int colm);
		std::shared_ptr<colony> Colony(const int num);
		const std::shared_ptr<colony> Colony(const int num) const;
		void AddRoamer(person* newRoamer, const int row, const int colm);

		std::shared_ptr<tile> Terrain(const int row, const int column) const;
		std::vector<std::vector<std::shared_ptr<tile>>> SurroundingTerrain(
				const int row, const int colm);
		unsigned int NumberOfRows() const;
		unsigned int NumberOfColumns() const;
		bool OutOfBounds(const unsigned int row, const unsigned int colm) const;
		bool OutOfBounds(const int row, const int colm) const;

		void MoveView(direction_t dir);
		bool MoveUnitTo(person* mover, const int row, const int colm);

		std::string TerrainName(const unsigned int x, const unsigned int y);

		std::unique_ptr<path> PathTo(const int startRow, const int startColm, 
				const int destRow, const int destColm, 
				const moveCostTable& moveCosts);
};
#endif
