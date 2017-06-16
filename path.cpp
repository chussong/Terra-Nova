#include "path.hpp"

path::path(const person& mover, const map& theMap, const int destRow,
		const int destColm){
	steps = ShortestPath(theMap, mover.Row(), mover.Colm(), destRow, destColm,
			mover.MoveCosts());
}

// this is pure Dijkstra with no heuristics
std::vector<std::array<int, 2>> path::ShortestPath(const map& theMap,
		const int startRow, const int startColm, const int destRow, 
		const int destColm, const moveCostTable& moveCosts){
	std::vector<std::vector<unsigned int>> distMap;
	std::vector<std::vector<bool>> closed;
	distMap.resize(theMap.NumberOfRows());
	closed.resize (theMap.NumberOfRows());
	for(auto& row : distMap) row.resize(theMap.NumberOfColumns(), -1   );
	for(auto& row : closed ) row.resize(theMap.NumberOfColumns(), false);

	std::array<int, 3> lowestOpen = {{ startRow, startColm, 0 }};
	std::vector<std::vector<bool>> checked(closed);
	do{
		UpdateNode(theMap, distMap, lowestOpen[0], lowestOpen[1], lowestOpen[2],
				moveCosts);
		closed[lowestOpen[0]][lowestOpen[1]] = true;

		if(lowestOpen[0] == destRow && lowestOpen[1] == destColm){
			break;
		}
		for(auto& row : checked) row.assign(row.size(), 0);
		lowestOpen = FindNextLowestOpen(distMap, closed, checked, lowestOpen);
	}while(true);

	return BuildPathVector(distMap, destRow, destColm);
}

void path::UpdateNode(const map& theMap, 
		std::vector<std::vector<unsigned int>>& distMap,
		const int nodeRow, const int nodeColm, const unsigned int nodeValue,
		const moveCostTable& moveCosts){
	distMap[nodeRow][nodeColm] = nodeValue;
	std::array<unsigned int, 6> adjacentEdges(ForSurrounding(
				std::function<std::shared_ptr<tile>(int,int)>(
					[theMap](int i, int j){return theMap.Terrain(i,j);}
				),
				nodeRow, nodeColm, 
				std::function<unsigned int(std::shared_ptr<tile>)>(
					[moveCosts](std::shared_ptr<tile> tl) 
					{return MoveCost(tl, moveCosts);}
				)));
	for(auto& edge : adjacentEdges) edge += nodeValue;
	SetSurrounding(distMap, nodeRow, nodeColm, adjacentEdges);
}

unsigned int path::MoveCost(const std::shared_ptr<tile> destination,
	const moveCostTable moveCosts){
	unsigned int cost = moveCosts.base;
	if(destination->Wooded()){
		if(moveCosts.wooded < 0){
			return -1;
		} else {
			cost += moveCosts.wooded;
		}
	}
	if(destination->Aquatic()){
		if(moveCosts.aquatic < 0){
			return -1;
		} else {
			cost += moveCosts.aquatic;
		}
	}
	if(destination->Cold()){
		if(moveCosts.cold < 0){
			return -1;
		} else {
			cost += moveCosts.cold;
		}
	}
	if(destination->Hilly()){
		if(moveCosts.hilly < 0){
			return -1;
		} else {
			cost += moveCosts.hilly;
		}
	}
	return cost;
}

// check all adjacent tiles
// if an adjacent tile is closed, run this on it and return output
// if it's open, return its {row, colm, dist}
// return lowest dist of the 6 (directly) checked tiles
std::array<int, 3> path::FindNextLowestOpen(
		const std::vector<std::vector<unsigned int>>& distMap,
		const std::vector<std::vector<bool>>& closedNodes,
		std::vector<std::vector<bool>>& checkedNodes,
		const std::array<int, 3> startingNode){
	std::array<std::array<int, 3>, 6> out = {{
		{{-1, -1, 10000}},
		{{-1, -1, 10000}},
		{{-1, -1, 10000}},
		{{-1, -1, 10000}},
		{{-1, -1, 10000}},
		{{-1, -1, 10000}}
	}};

	for(auto i = 0; i < 6; ++i){
		int row = startingNode[0] + hexAdj[i][0];
		int colm = startingNode[1] + hexAdj[i][1];
		if(checkedNodes[row][colm]) continue;
		checkedNodes[row][colm] = true;
		if(closedNodes[row][colm]){
			out[i] = FindNextLowestOpen(distMap, closedNodes, checkedNodes, 
					{{row, colm, static_cast<int>(distMap[row][colm])}});
		} else {
			out[i] = {{row, colm, static_cast<int>(distMap[row][colm])}};
		}
	}

	return *std::min_element(out.begin(), out.end(), 
			[](std::array<int, 3> a, std::array<int, 3> b){return a[2] < b[2];});
}

std::vector<std::array<int, 2>> path::BuildPathVector(
		const std::vector<std::vector<unsigned int>>& distMap, const int destRow,
		const int destColm){

	if(distMap[destRow][destColm] == 0) return std::vector<std::array<int, 2>>();

	std::array<int, 3> best = {{-1, -1, 10000}};
	for(auto i = 0; i < 6; ++i){
		int row = destRow + hexAdj[i][0];
		int colm = destRow + hexAdj[i][1];
		if(static_cast<int>(distMap[row][colm]) < best[2]) best = {{row, colm, 
			static_cast<int>(distMap[row][colm])}};
	}
	std::vector<std::array<int, 2>> bestPath(BuildPathVector(distMap, best[0], 
				best[1]));
	bestPath.push_back({{destRow, destColm}});
	return bestPath;
}
