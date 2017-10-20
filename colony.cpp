#include "colony.hpp"

namespace TerraNova {

Colony::Colony(SDL_Renderer* ren, const int faction): 
		ren(ren), name(Faction::GenerateColonyName(faction)), faction(faction)
{
	resources.fill(5*RESCAP/5);
	resourceCap.fill(RESCAP);
}

void Colony::ChangeName(const std::string name){
	this->name = name;
}

int Colony::AddResource(const resource_t resource, int amount){
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

std::array<int, LAST_RESOURCE> Colony::AddResources(
		const std::array<int, LAST_RESOURCE>& income){
	std::array<int, LAST_RESOURCE> leftovers;
	for(unsigned int i = 0; i < income.size(); ++i){
		if(income[i] <= 0){
			leftovers[i] = income[i];
			continue;
		}
		if(resources[i] + income[i] > resourceCap[i]){
			leftovers[i] = income[i] - resourceCap[i] + resources[i];
			resources[i] = resourceCap[i];
		} else {
			resources[i] += income[i];
			leftovers[i] = 0;
		}
	}
	return leftovers;
}

int Colony::TakeResource(const resource_t resource, int amount){
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

std::array<int, LAST_RESOURCE> Colony::TakeResources(
		const std::array<int, LAST_RESOURCE>& amounts) {
	std::array<int, LAST_RESOURCE> ret;
	for (auto i = 0u; i < LAST_RESOURCE; ++i) {
		ret[i] = TakeResource(static_cast<resource_t>(i), amounts[i]);
	}
	return ret;
}

std::string Colony::Name() const{
	return name;
}

int Colony::Row() const{
	return row;
}

int Colony::Column() const{
	return colm;
}

const std::array<int, LAST_RESOURCE>& Colony::Resources() const {
	return resources;
}

const int& Colony::Resource(const resource_t resource) const{
	return resources[resource];
}

const int& Colony::Resource(const int resource) const{
	return Resource(static_cast<resource_t>(resource));
}

const int& Colony::AvailablePower() const {
	return powerGrid.AvailablePower();
}

const int& Colony::MaximumPower() const {
	return powerGrid.MaximumPower();
}

void Colony::AddBuilding(std::shared_ptr<Building> toAdd) {
	powerGrid.AddBuilding(toAdd);
}

void Colony::StartTurn(){
	powerGrid.StartTurn();
}

void Colony::EndTurn(){
	powerGrid.EndTurn();
}

std::string Colony::ResourceName(const resource_t resource){
	switch(resource){
		case FOOD:			return "food";
		case CARBON:		return "carbon";
		case SILICON:		return "silicon";
		case IRON:			return "iron";
		default:			return "RESOURCE NOT FOUND";
	}
}

} // namespace TerraNova
