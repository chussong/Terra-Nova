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
#include "ui.hpp"
#include "faction.hpp"
#include "random.hpp"

namespace TerraNova {

/*struct Map_Iterator : std::iterator_traits<inner_range::iterator> {
	Map_Iterator(const outer_range::iterator& outer_iterator,
			const outer_range::iterator& outer_end): 
		outer_iterator(outer_iterator), outer_end(outer_end) { StartNewRow(); }

	Map_Iterator& operator++() {
		++inner_iterator;
		if (inner_iterator == inner_end) {
			++outer_iterator;
			StartNewRow();
			return *this;
		}
		++inner_iterator;
		if (inner_iterator == inner_end) {
			++outer_iterator;
			StartNewRow();
		}
		return *this;
	}

	Map_Iterator operator++(int) {
		Map_Iterator ret = *this;
		++(*this);
		return ret;
	}

	reference operator*() const {
		return *inner_iterator;
	}

	bool operator==(const Map_Iterator& other) const {
		const bool thisEnd = outer_iterator == outer_end;
		const bool otherEnd = other.outer_iterator == other.outer_end;
		if (thisEnd && otherEnd) return true;
		if (thisEnd != otherEnd) return false;
		return outer_iterator == other.outer_iterator 
			&& inner_iterator == other.inner_iterator;
	}

	private:
		outer_range::iterator outer_iterator, outer_end;
		inner_range::iterator inner_iterator, inner_end;
		bool oddRow = false;

		void StartNewRow() {
			while (outer_iterator != outer_end) {
				oddRow = !oddRow;
				inner_iterator = (*outer_iterator)->begin();
				inner_end = (*outer_iterator)->end();
				if (inner_iterator == inner_end) {
					++outer_iterator;
				} else {
					if (oddRow) ++inner_iterator;
					break;
				}
			}
		}
};*/

class Map {
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
	int viewCenterRow;
	int viewCenterCol;

	/* Terrain format: each entry of terrain is a vector corresponding to a row
	 * of the hex grid. Each of these rows is a vector of Tiles which is twice
	 * as long as the number of Tiles it actually contains: odd rows have only
	 * odd entries and even rows have only even entries.*/
	std::vector<std::vector<std::shared_ptr<Tile>>> terrain;
	std::vector<std::unique_ptr<Colony>> colonies;
	std::vector<std::weak_ptr<Unit>> weakRoamers;
	std::vector<Unit*> roamers;

	void InitTerrain(std::vector<std::shared_ptr<TileType>> types);

	// Path construction functions
	std::vector<std::array<unsigned int, 2>> ShortestPath(const unsigned int startRow, 
			const unsigned int startColm, const unsigned int destRow, 
			const unsigned int destColm, const MoveCostTable& moveCosts);

	void UpdateNode(std::vector<std::vector<unsigned int>>& distMap,
			const unsigned int nodeRow, const unsigned int nodeColm, 
			const unsigned int value, const MoveCostTable& moveCosts);

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

	static std::vector<MoveData> FindMoverData(const std::vector<Unit*>& roamers);

	template<typename T> inline void ForAllTiles(T (Tile::*MemberFunction)()){
		for(auto row = 0u; row < NumberOfRows(); ++row){
			for(auto colm = row % 2; colm < NumberOfColumns(); colm+=2){
				/*std::cout << "Applying function to Tile at (" << row << ","
					<< colm << ")." << std::endl;*/
				(Terrain(row, colm).get()->*MemberFunction)();
			}
		}
	}

	public:
		Map() = delete;
		Map(SDL_Renderer* ren, std::vector<std::shared_ptr<TileType>> types);
		Map(SDL_Renderer* ren, std::vector<std::vector<std::shared_ptr<Tile>>> Tiles):
			ren(ren), terrain(Tiles) {}

		Map(const Map& other) = delete;
		Map& operator=(const Map other) = delete;

		void StartTurn();
		void EndTurn();

		Colony* AddColony(const int row, const int colm, const int faction);
		void BuildColony(Unit& builder);
		Button MakeBuildColonyButton(Unit& builder);
		/*std::shared_ptr<Colony> Colony(const int num);
		const std::shared_ptr<Colony> Colony(const int num) const;*/
		void AddRoamer(std::shared_ptr<Unit> newRoamer, const int row, const int colm);

		std::shared_ptr<Tile> Terrain(const int row, const int column) const;
		std::shared_ptr<Tile> Terrain(const std::array<unsigned int,2> coords) const;
		std::vector<std::vector<std::shared_ptr<Tile>>> SurroundingTerrain(
				const int row, const int colm);
		unsigned int NumberOfRows() const;
		unsigned int NumberOfColumns() const;
		bool OutOfBounds(const unsigned int row, const unsigned int colm) const;
		bool OutOfBounds(const int row, const int colm) const;

		void SetViewCenter(const int row, const int col);
		void MoveView(direction_t dir);
		void CenterViewOn(const int row, const int col);
		void ShiftAllTiles(const int xShift, const int yShift);
		bool MoveUnitTo(Unit* mover, const int row, const int colm);
		bool MoveUnitTo(Unit* mover, const std::array<unsigned int,2>& coords);

		std::string TerrainName(const unsigned int x, const unsigned int y);

		std::unique_ptr<Path> PathTo(const int startRow, const int startColm, 
				const int destRow, const int destColm, 
				const MoveCostTable& moveCosts);
		std::unique_ptr<Path> PathTo(const std::array<unsigned int, 2>& startLoc,
				const std::array<unsigned int, 2>& destLoc,
				const MoveCostTable& moveCosts);

		unsigned int DistanceBetween(const unsigned int rowA, 
				const unsigned int colA, const unsigned int rowB,
				const unsigned int colB, const MoveCostTable& moveCosts);
		unsigned int DistanceBetween(const std::array<unsigned int, 2>& locA,
				const std::array<unsigned int, 2>& locB, 
				const MoveCostTable& moveCosts);
};

} // namespace TerraNova
#endif
