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
	enum MoveStatus { MS_UNCHECKED, MS_CLEAR, MS_BOUNCE, MS_ASSAULT, 
		MS_ASSAULTED, MS_FRIENDCLASH, MS_FOECLASH, MS_TARGETMOVING, 
		MS_FINISHED };

	struct MoveData {
		unsigned int id;
		std::array<unsigned int,2> origin;
		std::array<unsigned int,2> destination;
		MoveStatus status;
	};

	SDL_Renderer* ren;

	int width;
	int height;
	/* Terrain format: each entry of terrain is a vector corresponding to a row
	 * of the hex grid. Each of these rows is a vector of tiles which is twice
	 * as long as the number of tiles it actually contains: odd rows have only
	 * odd entries and even rows have only even entries.*/
	std::vector<std::vector<std::shared_ptr<tile>>> terrain;
	std::vector<std::weak_ptr<colony>> colonies;
	std::vector<std::weak_ptr<person>> weakRoamers;
	std::vector<person*> roamers;

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

	// movement processing functions
	void ProcessMovement();
	bool UncontestedMoves(std::vector<MoveData>& moverData);
	bool AssaultMoves(std::vector<MoveData>& moverData);
	bool ClashMoves(std::vector<MoveData>& moverData);

	static std::vector<MoveData> FindMoverData(const std::vector<person*>& roamers);

	template<typename T> inline void ForAllTiles(T (tile::*MemberFunction)()){
		for(auto row = 0u; row < NumberOfRows(); ++row){
			for(auto colm = row % 2; colm < NumberOfColumns(); colm+=2){
				/*std::cout << "Applying function to tile at (" << row << ","
					<< colm << ")." << std::endl;*/
				(Terrain(row, colm).get()->*MemberFunction)();
			}
		}
	}

	public:
		map() = delete;
		map(SDL_Renderer* ren, std::vector<std::shared_ptr<tileType>> types);
		map(SDL_Renderer* ren, std::vector<std::vector<std::shared_ptr<tile>>> tiles):
			ren(ren), terrain(tiles) {}

		void StartTurn();
		void EndTurn();

		void AddColony(const std::shared_ptr<colony> colony, int row, int colm);
		std::shared_ptr<colony> Colony(const int num);
		const std::shared_ptr<colony> Colony(const int num) const;
		void AddRoamer(std::shared_ptr<person> newRoamer, const int row, const int colm);

		std::shared_ptr<tile> Terrain(const int row, const int column) const;
		std::shared_ptr<tile> Terrain(const std::array<unsigned int,2> coords) const;
		std::vector<std::vector<std::shared_ptr<tile>>> SurroundingTerrain(
				const int row, const int colm);
		unsigned int NumberOfRows() const;
		unsigned int NumberOfColumns() const;
		bool OutOfBounds(const unsigned int row, const unsigned int colm) const;
		bool OutOfBounds(const int row, const int colm) const;

		void MoveView(direction_t dir);
		bool MoveUnitTo(person* mover, const int row, const int colm);
		bool MoveUnitTo(person* mover, const std::array<unsigned int,2>& coords);

		std::string TerrainName(const unsigned int x, const unsigned int y);

		std::unique_ptr<path> PathTo(const int startRow, const int startColm, 
				const int destRow, const int destColm, 
				const moveCostTable& moveCosts);
};
#endif
