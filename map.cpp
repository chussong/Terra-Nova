#include "map.hpp"

map::map(SDL_Renderer* ren, std::vector<std::shared_ptr<tileType>> types):
	ren(ren){
	height = DEFAULT_HEIGHT;
	width = DEFAULT_WIDTH;
	terrain.resize(height);
	for(unsigned int i = 0; i < terrain.size(); ++i) 
		terrain[i].resize(2*width);
	InitTerrain(types);
}
	
/*
void map::ViewCenteredOn(const int centerColm, const int centerRow, 
		std::shared_ptr<gameWindow> win){
	if(!win){
		std::cerr << "Error: attempted to draw a map to a nonexistent window."
			<< std::endl;
		return;
	}
	win->ResetObjects();
	DrawTiles(centerColm, centerRow, win);
}

void map::DrawTiles(const int centerColm, const int centerRow, 
		std::shared_ptr<gameWindow> win){
	for(unsigned int i = 0; i < terrain.size(); ++i){
		for(unsigned int j = 0; j < terrain[i].size(); ++j){
			terrain[i][j]->MoveTo(SCREEN_WIDTH/2 + (j-centerColm)*TILE_WIDTH/2,
					SCREEN_HEIGHT/2 + (i-centerRow)*TILE_HEIGHT);
			win->AddObject(terrain[i][j]);
		}
	}
}
*/

void map::Clean(){
	CleanExpired(colonies);
	CleanExpired(roamers);
}


void map::InitTerrain(std::vector<std::shared_ptr<tileType>> types){
	for(unsigned int row = 0; row < terrain.size(); ++row){
		for(unsigned int col = row%2; col < terrain[row].size(); col+=2){
			if(col % 2 == 0) terrain[row][col] = 
				std::make_shared<tile>(FindByName(types, "plains"), ren, row, col);
			if(col % 2 == 1) terrain[row][col] = 
				std::make_shared<tile>(FindByName(types, "mountain"), ren, row, col);
		}
	}
}

// path construction; this is pure Dijkstra with no heuristics
std::vector<std::array<unsigned int, 2>> map::ShortestPath(const unsigned int startRow, 
		const unsigned int startColm, const unsigned int destRow,
		const unsigned int destColm, const moveCostTable& moveCosts){
	if(tile::MoveCost(*Terrain(destRow, destColm), moveCosts) == -1u)
		return std::vector<std::array<unsigned int, 2>>();
	std::vector<std::vector<unsigned int>> distMap;
	std::vector<std::vector<bool>> closed;
	distMap.resize(NumberOfRows());
	closed.resize (NumberOfRows());
	for(auto& row : distMap){
		row.resize(NumberOfColumns(), -1u  );
	}
	for(auto& row : closed ) row.resize(NumberOfColumns(), false);

	std::array<unsigned int, 3> lowestOpen = {{ startRow, startColm, 0 }};
	distMap[startRow][startColm] = 0;
	std::vector<std::vector<bool>> checked(closed);
	bool done = false;
	do{
		UpdateNode(distMap, lowestOpen[0], lowestOpen[1], lowestOpen[2],
				moveCosts);
		closed[lowestOpen[0]][lowestOpen[1]] = true;

		for(auto i = 0; i < 6; ++i){
			if(lowestOpen[0] + hexAdj[i][0] == destRow
					&& lowestOpen[1] + hexAdj[i][1] == destColm){
				done = true;
				break;
			}
		}
		if(done) break;

		for(auto& row : checked) row.assign(row.size(), false);
		checked[lowestOpen[0]][lowestOpen[1]] = true;
		lowestOpen = FindNextLowestOpen(distMap, closed, checked, lowestOpen);
	}while(true);

	auto ret = BuildPathVector(distMap, destRow, destColm);
	ret.insert(ret.begin(), std::array<unsigned int, 2>({{startRow, startColm}}));
	return ret;
}

void map::UpdateNode(std::vector<std::vector<unsigned int>>& distMap,
		const unsigned int nodeRow, const unsigned int nodeColm, 
		const unsigned int nodeValue, const moveCostTable& moveCosts){
	//distMap[nodeRow][nodeColm] = nodeValue;
	std::array<unsigned int, 6> adjacentEdges(ForSurrounding(
				std::function<std::shared_ptr<tile>(int,int)>(
					[this](int i, int j){return this->Terrain(i,j);}
				),
				nodeRow, nodeColm, 
				std::function<unsigned int(std::shared_ptr<tile>)>(
					[moveCosts](std::shared_ptr<tile> tl) 
					{if(!tl) return -1u; return tile::MoveCost(*tl, moveCosts);}
				)));
	for(auto i = 0; i < 6; ++i){
		if(OutOfBounds(nodeRow + hexAdj[i][0], nodeColm + hexAdj[i][1])
				|| adjacentEdges[i] == -1u) continue;
		// (map dist from here) -> dist. from start or skip if we've already been
		adjacentEdges[i] = std::min(adjacentEdges[i] + nodeValue,
				distMap[nodeRow + hexAdj[i][0]][nodeColm + hexAdj[i][1]]);
	}
	SetSurrounding(distMap, nodeRow, nodeColm, adjacentEdges);
}

// check all adjacent tiles
// if an adjacent tile is closed, run this on it and return output
// if it's open, return its {row, colm, dist}
// return lowest dist of the 6 (directly) checked tiles
std::array<unsigned int, 3> map::FindNextLowestOpen(
		const std::vector<std::vector<unsigned int>>& distMap,
		const std::vector<std::vector<bool>>& closedNodes,
		std::vector<std::vector<bool>>& checkedNodes,
		const std::array<unsigned int, 3> startingNode){
	std::array<std::array<unsigned int, 3>, 6> out = {{
		{{-1u, -1u, -1u}},
		{{-1u, -1u, -1u}},
		{{-1u, -1u, -1u}},
		{{-1u, -1u, -1u}},
		{{-1u, -1u, -1u}},
		{{-1u, -1u, -1u}}
	}};

	for(auto i = 0; i < 6; ++i){
		unsigned row = startingNode[0] + hexAdj[i][0];
		unsigned colm = startingNode[1] + hexAdj[i][1];
		if(OutOfBounds(row, colm) || checkedNodes[row][colm]) continue;
		checkedNodes[row][colm] = true;
		if(closedNodes[row][colm]){
			out[i] = FindNextLowestOpen(distMap, closedNodes, checkedNodes, 
					{{row, colm, distMap[row][colm]}});
		} else {
			out[i] = {{row, colm, distMap[row][colm]}};
		}
	}

	return *std::min_element(out.begin(), out.end(), 
			[](std::array<unsigned int, 3> a, std::array<unsigned int, 3> b)
			{return a[2] < b[2];});
}

std::vector<std::array<unsigned int, 2>> map::BuildPathVector(
		const std::vector<std::vector<unsigned int>>& distMap, 
		const unsigned int destRow, const unsigned int destColm){

	/*std::cout << "BEST PATH HOME AROUND (" << destRow << "," << destColm << "):"
		<< std::endl;*/
	if(distMap[destRow][destColm] == 0 || distMap[destRow][destColm] == -1u) 
		return std::vector<std::array<unsigned int, 2>>();

	std::array<unsigned int, 3> best({{-1u, -1u, -1u}});
	for(auto i = 0; i < 6; ++i){
		unsigned int row = destRow + hexAdj[i][0];
		unsigned int colm = destColm + hexAdj[i][1];
		if(row >= distMap.size() || colm >= distMap[row].size()) continue;
		/*std::cout << "(" << row << "," << colm << "): " << distMap[row][colm]
			<< std::endl;*/
		if(distMap[row][colm] < best[2]) best = {{row, colm, distMap[row][colm]}};
	}
	/*std::cout << "BEST: (" << best[0] << "," << best[1] << "," << best[2] << ")\n"
		<< "-------------------" << std::endl;*/
	std::vector<std::array<unsigned int, 2>> bestPath(BuildPathVector(distMap, best[0], 
				best[1]));
	bestPath.push_back(std::array<unsigned int,2>(
				{{destRow, destColm}}));
	return bestPath;
}

// end of path construction

void map::AddColony(const std::shared_ptr<colony> col, int row, int colm){
	colonies.emplace_back(col);
	Terrain(row, colm)->SetHasColony(true);
}

std::shared_ptr<colony> map::Colony(const int num){
	if(num < 0 || static_cast<unsigned int>(num) >= colonies.size())
		return nullptr;
	return colonies[num].lock();
}

const std::shared_ptr<colony> map::Colony(const int num) const{
	if(num < 0 || static_cast<unsigned int>(num) >= colonies.size())
		return nullptr;
	return colonies[num].lock();
}

std::shared_ptr<tile> map::Terrain(const int row, const int column) const{
	if((row + column)%2 != 0){
		std::cerr << "Error: someone is trying to access the non-existent "
			<< "tile at row " << row << ", column " << column << "." << std::endl;
		return nullptr;
	}
	if(OutOfBounds(row, column)) return nullptr;
	if(!terrain[row][column]){
		std::cerr << "Error: someone is trying to access the valid tile at row "
			<< row << ", column " << column << ", but there's nothing there."
			<< std::endl;
	}
	return terrain[row][column];
}

std::vector<std::vector<std::shared_ptr<tile>>> map::SurroundingTerrain(
		const int row, const int colm){
	std::vector<std::vector<std::shared_ptr<tile>>> ret;
	ret.resize(5);
	for(unsigned int i = 0; i < ret.size(); ++i){
		ret[i].resize(ret.size() -
				std::abs((static_cast<int>(ret.size())-1)/2 - static_cast<int>(i)));
		for(unsigned int j = 0; j < ret[i].size(); ++j){
			ret[i][j] = Terrain(row - 2 + i, 
					colm - (ret.size()-1) + std::abs((int)i - ((int)ret.size()-1)/2) + 2*j);
		}
	}
	return ret;
}

unsigned int map::NumberOfRows() const{
	if(terrain.size() < 1){
		std::cerr << "Error: asked for number of rows on a map, but the map "
			<< "has not been initialized." << std::endl;
		return 0;
	}
	return terrain.size();
}

unsigned int map::NumberOfColumns() const{
	if(terrain.size() < 1){
		std::cerr << "Error: asked for number of columns on a map, but the map "
			<< "has not been initialized." << std::endl;
		return 0;
	}
	return terrain[0].size();
}

bool map::OutOfBounds(const unsigned int row, const unsigned int colm) const{
	return row >= NumberOfRows() || colm >= NumberOfColumns();
}

bool map::OutOfBounds(const int row, const int colm) const{
	return OutOfBounds(static_cast<unsigned int>(row), static_cast<unsigned int>(colm));
}

/*
std::vector<std::vector<std::shared_ptr<tile>>> map::SurroundingTerrain(
		int centerColm, int centerRow, int widthToDisplay, int heightToDisplay){

	leftEdge = std::max(centerColm - widthToDisplay, 0);
	leftEdge = std::min(leftEdge, width - 2*widthToDisplay);
	bottomEdge = std::max(centerRow - heightToDisplay/2, 0);
	bottomEdge = std::min(bottomEdge, height - heightToDisplay);

	std::vector<std::vector<std::shared_ptr<tile>>> sur;
	sur.resize(heightToDisplay);
	for(unsigned int i = 0; i < sur.size(); ++i){
		sur[i].resize(2*widthToDisplay);
		for(unsigned int j = i%2; j < sur[i].size(); j+=2){
			sur[i][j] = terrain	[bottomEdge + i][leftEdge - centerRow%2 + j];
		}
	}
	return sur;
}
*/

void map::MoveView(direction_t dir){
	int xShift=0, yShift=0;
	switch(dir){
		case VIEW_DOWN:
			yShift = -TILE_HEIGHT;
			break;
		case VIEW_UP:
			yShift = TILE_HEIGHT;
			break;
		case VIEW_LEFT:
			xShift = TILE_WIDTH;
			break;
		case VIEW_RIGHT:
			xShift = -TILE_WIDTH;
			break;
	}
	for(unsigned int i = 0; i < terrain.size(); ++i){
		for(unsigned int j = i%2; j < terrain[i].size(); j+=2){
			Terrain(i,j)->MoveTo(Terrain(i,j)->X() + xShift,
						Terrain(i,j)->Y() + yShift);
		}
	}
}

std::string map::TerrainName(const unsigned int row, const unsigned int col){
	if(row < terrain.size() && col < terrain[row].size())
		return Terrain(row, col)->Name();
	return "OUTSIDE_OF_MAP";
}

std::unique_ptr<path> map::PathTo(const int startRow, const int startColm, 
		const int destRow, const int destColm, const moveCostTable& moveCosts){
	std::vector<std::array<unsigned int, 2>> steps(ShortestPath(startRow, startColm,
				destRow, destColm, moveCosts));
	return steps.size() == 0 ? nullptr : std::make_unique<path>(steps);
}
