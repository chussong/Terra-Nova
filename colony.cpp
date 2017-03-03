#include "colony.hpp"

colony::colony(std::shared_ptr<map> myMap, const int row, const int colm):
	myMap(myMap), row(row), colm(colm){
	name = "Moonbase JAFFA";

	terrain[0] = myMap->Terrain(row, colm+2);
	terrain[1] = myMap->Terrain(row+1, colm+1);
	terrain[2] = myMap->Terrain(row+1, colm-1);
	terrain[3] = myMap->Terrain(row, colm-2);
	terrain[4] = myMap->Terrain(row-1, colm-1);
	terrain[5] = myMap->Terrain(row-1, colm+1);

	terrain[6] = myMap->Terrain(row, colm+4);
	terrain[7] = myMap->Terrain(row+1, colm+3);
	terrain[8] = myMap->Terrain(row+2, colm+2);
	terrain[9] = myMap->Terrain(row+2, colm);
	terrain[10] = myMap->Terrain(row+2, colm-2);
	terrain[11] = myMap->Terrain(row+1, colm-3);
	terrain[12] = myMap->Terrain(row, colm-4);
	terrain[13] = myMap->Terrain(row-1, colm-3);
	terrain[14] = myMap->Terrain(row-2, colm-2);
	terrain[15] = myMap->Terrain(row-2, colm);
	terrain[16] = myMap->Terrain(row-2, colm+2);
	terrain[17] = myMap->Terrain(row-1, colm+3);

	resources.fill(0);
	resourceCap.fill(RESCAP);
	resPerTurn.fill(0);
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

void colony::SetResourceIncome(const resource_t resource, int amount){
	if(amount < 0) return;
	if(resource < 0 || resource >= LAST_RESOURCE) return;
	resPerTurn[resource] = amount;
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

terrain_t colony::Terrain(const unsigned int num) const{
	if(num < terrain.size()) return terrain[num];
	return OCEAN;
}

int colony::Resource(const resource_t resource) const{
	return resources[resource];
}

std::string colony::ResAsString(const int res) const{
	return std::to_string(resources[res]);
}

std::shared_ptr<person> colony::Inhabitant(const int number) {
	if(number < 0 || (unsigned int)number >= inhabitants.size()) return nullptr;
	return inhabitants[number].lock();
}

const std::shared_ptr<person> colony::Inhabitant(const int number) const {
	if(number < 0 || (unsigned int)number >= inhabitants.size()) return nullptr;
	return inhabitants[number].lock();
}

void colony::ProcessTurn(){
	for(int i = 0; i < static_cast<int>(LAST_RESOURCE); ++i){
		AddResource(static_cast<resource_t>(i), resPerTurn[i]);
	}
}
