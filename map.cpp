#include "map.hpp"

map::map(SDL_Renderer* ren): ren(ren){
	height = DEFAULT_HEIGHT;
	width = DEFAULT_WIDTH;
	terrain.resize(height);
	for(unsigned int i = 0; i < terrain.size(); ++i) 
		terrain[i].resize(2*width);
	InitTerrain();
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


void map::InitTerrain(){
	for(unsigned int row = 0; row < terrain.size(); ++row){
		for(unsigned int col = row%2; col < terrain[row].size(); col+=2){
			if(col % 2 == 0) terrain[row][col] = 
				std::make_shared<tile>(PLAINS, ren, "plains", 0, 0);
			if(col % 2 == 1) terrain[row][col] = 
				std::make_shared<tile>(MOUNTAIN, ren, "mountain", 0, 0);
		}
	}
}

void map::AddColony(const std::shared_ptr<colony> colony){
	colonies.emplace_back(colony);
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
	return terrain[row][column];
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
std::string map::TerrainName(const unsigned int row, const unsigned int col){
	if(row < terrain.size() && col < terrain[row].size())
		return TerrainName(terrain[row][col]);
	return "OUTSIDE_OF_MAP";
}

std::string map::TerrainName(const terrain_t type){
	switch(type){
		case OCEAN:			return "ocean";
		case COAST:			return "coast";
		case PLAINS:		return "plains";
		case MOUNTAIN:		return "mountain";
		default:			return "TERRAIN TYPE NOT FOUND";
	}
	return "";
}

std::string map::TerrainName(const std::shared_ptr<tile> tl){
	return TerrainName(tl->TileType());
}
