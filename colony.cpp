#include "colony.hpp"

colony::colony(std::shared_ptr<map> myMap, const int row, const int colm):
	myMap(myMap), row(row), colm(colm){
	name = "Moonbase JAFFA";

	resources.fill(0);
	resourceCap.fill(RESCAP);
	powerSupply = 0;
	powerDemand = 0;
}

void colony::Clean(){
	CleanExpired(inhabitants);
	CleanExpired(stagingArea);
}

void colony::ChangeName(const std::string name){
	this->name = name;
}

void colony::Move(const int xdist, const int ydist){
	if(colm + xdist > 100 || colm + xdist < -100) return; // export these 100s
	if(row + ydist > 100 || row + ydist < -100) return; // to constexprs
	colm += xdist;
	row += ydist;
}

int colony::AddResource(const resource_t resource, int amount){
	if(resource < 0 || resource >= LAST_RESOURCE) return -1;
	if(amount < 0) return 0;
	if(resources[resource] + amount > resourceCap[resource]){
		amount -= resourceCap[resource]-resources[resource];
		resources[resource] = resourceCap[resource];
	} else {
		resources[resource] += amount;
		amount = 0;
	}
	return amount;
}

int colony::TakeResource(const resource_t resource, int amount){
	if(resource < 0 || resource >= LAST_RESOURCE) return -1;
	if(amount < 0) return 0;
	if(amount > resources[resource]){
		amount = resources[resource];
		resources[resource] = 0;
	} else {
		resources[resource] -= amount;
	}
	return amount;
}

void colony::AddInhabitant(std::shared_ptr<person> inhabitant){
	inhabitants.emplace_back(inhabitant);
}

void colony::AddBuilding(const std::string building){
	buildings.push_back(building);
}

std::string colony::Name() const{
	return name;
}

int colony::Column() const{
	return colm;
}

int colony::Row() const{
	return row;
}

int colony::Resource(const resource_t resource) const{
	return resources[resource];
}

std::shared_ptr<person> colony::Inhabitant(const int number) {
	if(number < 0 || (unsigned int)number >= inhabitants.size()) return nullptr;
	return inhabitants[number].lock();
}

const std::shared_ptr<person> colony::Inhabitant(const int number) const {
	if(number < 0 || (unsigned int)number >= inhabitants.size()) return nullptr;
	return inhabitants[number].lock();
}

std::vector<gfxObject> colony::InnerRing(const int colm, const int row,
		SDL_Renderer* ren) const{
	std::vector<gfxObject> ring; // starting 1 hex right, proceed CCW
	std::string spriteDir = GetSpritePath("sprites");
	ring.emplace_back(ren, spriteDir + map::TerrainName(myMap->Terrain(row, colm+2))
			+ ".png", MAPDISP_ORIGIN_X + TILE_WIDTH, MAPDISP_ORIGIN_Y);
	ring.emplace_back(ren, spriteDir + map::TerrainName(myMap->Terrain(row+1, colm+1))
			+ ".png", MAPDISP_ORIGIN_X + TILE_WIDTH/2, MAPDISP_ORIGIN_Y + TILE_HEIGHT);
	ring.emplace_back(ren, spriteDir + map::TerrainName(myMap->Terrain(row+1, colm-1))
			+ ".png", MAPDISP_ORIGIN_X - TILE_WIDTH/2, MAPDISP_ORIGIN_Y + TILE_HEIGHT);
	ring.emplace_back(ren, spriteDir + map::TerrainName(myMap->Terrain(row, colm-2))
			+ ".png", MAPDISP_ORIGIN_X - TILE_WIDTH, MAPDISP_ORIGIN_Y);
	ring.emplace_back(ren, spriteDir + map::TerrainName(myMap->Terrain(row-1, colm-1))
			+ ".png", MAPDISP_ORIGIN_X - TILE_WIDTH/2, MAPDISP_ORIGIN_Y - TILE_HEIGHT);
	ring.emplace_back(ren, spriteDir + map::TerrainName(myMap->Terrain(row-1, colm+1))
			+ ".png", MAPDISP_ORIGIN_X + TILE_WIDTH/2, MAPDISP_ORIGIN_Y - TILE_HEIGHT);
	return ring;
}

std::vector<gfxObject> colony::OuterRing(const int colm, const int row,
		SDL_Renderer* ren) const{
	std::vector<gfxObject> ring; // starting 2 hexes right, proceed CCW
	std::string spriteDir = GetSpritePath("sprites");
	ring.emplace_back(ren, spriteDir + map::TerrainName(myMap->Terrain(row, colm+2))
			+ ".png", MAPDISP_ORIGIN_X + 2*TILE_WIDTH, MAPDISP_ORIGIN_Y);
	ring.emplace_back(ren, spriteDir + map::TerrainName(myMap->Terrain(row+1, colm+1))
			+ ".png", MAPDISP_ORIGIN_X + 3*TILE_WIDTH/2, MAPDISP_ORIGIN_Y + TILE_HEIGHT);
	ring.emplace_back(ren, spriteDir + map::TerrainName(myMap->Terrain(row+1, colm-1))
			+ ".png", MAPDISP_ORIGIN_X + TILE_WIDTH, MAPDISP_ORIGIN_Y + 2*TILE_HEIGHT);
	ring.emplace_back(ren, spriteDir + map::TerrainName(myMap->Terrain(row, colm-2))
			+ ".png", MAPDISP_ORIGIN_X, MAPDISP_ORIGIN_Y + 2*TILE_HEIGHT);
	ring.emplace_back(ren, spriteDir + map::TerrainName(myMap->Terrain(row-1, colm-1))
			+ ".png", MAPDISP_ORIGIN_X - TILE_WIDTH, MAPDISP_ORIGIN_Y + 2*TILE_HEIGHT);
	ring.emplace_back(ren, spriteDir + map::TerrainName(myMap->Terrain(row-1, colm+1))
			+ ".png", MAPDISP_ORIGIN_X - 3*TILE_WIDTH/2, MAPDISP_ORIGIN_Y + TILE_HEIGHT);
	ring.emplace_back(ren, spriteDir + map::TerrainName(myMap->Terrain(row, colm+2))
			+ ".png", MAPDISP_ORIGIN_X - 2*TILE_WIDTH, MAPDISP_ORIGIN_Y);
	ring.emplace_back(ren, spriteDir + map::TerrainName(myMap->Terrain(row+1, colm+1))
			+ ".png", MAPDISP_ORIGIN_X - 3*TILE_WIDTH/2, MAPDISP_ORIGIN_Y - TILE_HEIGHT);
	ring.emplace_back(ren, spriteDir + map::TerrainName(myMap->Terrain(row+1, colm-1))
			+ ".png", MAPDISP_ORIGIN_X - TILE_WIDTH, MAPDISP_ORIGIN_Y - 2*TILE_HEIGHT);
	ring.emplace_back(ren, spriteDir + map::TerrainName(myMap->Terrain(row, colm-2))
			+ ".png", MAPDISP_ORIGIN_X, MAPDISP_ORIGIN_Y - 2*TILE_HEIGHT);
	ring.emplace_back(ren, spriteDir + map::TerrainName(myMap->Terrain(row-1, colm-1))
			+ ".png", MAPDISP_ORIGIN_X + TILE_WIDTH, MAPDISP_ORIGIN_Y - 2*TILE_HEIGHT);
	ring.emplace_back(ren, spriteDir + map::TerrainName(myMap->Terrain(row-1, colm+1))
			+ ".png", MAPDISP_ORIGIN_X + 3*TILE_WIDTH/2, MAPDISP_ORIGIN_Y - TILE_HEIGHT);
	return ring;
}
