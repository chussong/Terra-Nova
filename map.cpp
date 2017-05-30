#include "map.hpp"

map::map(SDL_Renderer* ren): ren(ren){
	terrain.resize(DEFAULT_HEIGHT);
	for(unsigned int i = 0; i < terrain.size(); ++i) 
		terrain[i].resize(DEFAULT_WIDTH);
	InitTerrain();
}
	
void map::Clean(){
	CleanExpired(colonies);
	CleanExpired(roamers);
}


void map::InitTerrain(){
	for(unsigned int row = 0; row < terrain.size(); ++row){
		for(unsigned int col = 0; col < terrain[row].size(); ++col){
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
	return terrain[row][column];
}

std::vector<std::vector<std::shared_ptr<tile>>> map::SurroundingTerrain(
		int centerColm, int centerRow, int widthToDisplay, int heightToDisplay){
	//obviously placeholder
	return terrain;
}

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
