#ifndef PATH_HPP
#define PATH_HPP

#include <vector>
#include <array>
#include <algorithm>
#include <memory>

#include "templates.hpp"
#include "map.hpp"
#include "tile.hpp"
#include "person.hpp"

struct moveCostTable;
class map;
class tile;
class person;
class path {
	std::vector<std::array<int, 2>> steps;

	static std::vector<std::array<int, 2>> ShortestPath(const map& theMap,
			const int startRow, const int startColm, const int destRow, 
			const int destColm, const moveCostTable& moveCosts);

	static void UpdateNode(const map& theMap,
			std::vector<std::vector<unsigned int>>& distMap,
			const int nodeRow, const int nodeColm, const unsigned int value,
			const moveCostTable& moveCosts);

	static unsigned int MoveCost(const std::shared_ptr<tile> destination,
		const moveCostTable moveCosts);

	static std::array<int, 3> FindNextLowestOpen(
			const std::vector<std::vector<unsigned int>>& distMap,
			const std::vector<std::vector<bool>>& closedNodes,
			std::vector<std::vector<bool>>& checkedNodes,
			const std::array<int, 3> startingNode);

	static std::vector<std::array<int, 2>> BuildPathVector(
			const std::vector<std::vector<unsigned int>>& distMap, const int destRow,
			const int destColm);
	public:
		path() = delete;
		path(const person& mover, const map& theMap, const int destRow,
				const int destColm);

};

#endif
