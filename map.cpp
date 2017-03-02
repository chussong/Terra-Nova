#include "map.hpp"

map::map(){
	terrain.resize(DEFAULT_HEIGHT);
	for(unsigned int i = 0; i < terrain.size(); ++i) 
		terrain[i].resize(DEFAULT_WIDTH);
	InitTerrain(terrain);
}
	
void map::Clean(){
	CleanExpired(colonies);
	CleanExpired(roamers);
}


void map::InitTerrain(std::vector<std::vector<terrain_t>>& terrain){
	for(unsigned int row = 0; row < terrain.size(); ++row){
		for(unsigned int col = 0; col < terrain[row].size(); ++col){
			terrain[row][col] = PLAINS;
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

terrain_t map::Terrain(const int row, const int column) const{
	return terrain[row][column];
}

std::string map::TerrainName(const terrain_t type){
	switch(type){
		case OCEAN:			return "ocean";
		case COAST:			return "coast";
		case PLAINS:		return "plains";
		case MOUNTAIN:		return "mountain";
	}
	return "";
}
