#include "colony.hpp"

colony::colony(){
	name = "Moonbase JAFFA";
	xpos = 0;
	ypos = 0;

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
	if(xpos + xdist > 100 || xpos + xdist < -100) return; // export these 100s
	if(ypos + ydist > 100 || ypos + ydist < -100) return; // to constexprs
	xpos += xdist;
	ypos += ydist;
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

int colony::Xpos() const{
	return xpos;
}

int colony::Ypos() const{
	return ypos;
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
