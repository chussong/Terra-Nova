#include "colony.hpp"

Colony::Colony(SDL_Renderer* ren, 
		std::vector<std::vector<std::shared_ptr<Tile>>> terrain,
		const int faction): 
		ren(ren), faction(faction), terrain(terrain)
{
	name = "Aurora";

	resources.fill(0);
	resourceCap.fill(RESCAP);
	//resPerTurn.fill(0);
	powerSupply = 0;
	powerDemand = 0;
}

void Colony::SetBuildingTypes(
		const std::vector<std::shared_ptr<BuildingType>> buildingTypes){
	this->buildingTypes = buildingTypes;
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
	//if(resourcePanels[resource])
		//resourcePanels[resource]->SetText(std::to_string(resources[resource]));
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
		//resourcePanels[i]->SetText(std::to_string(resources[i]));
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
	//resourcePanels[resource]->SetText(std::to_string(resources[resource]));
	return amount;
}

/*void Colony::SetResourceIncome(const resource_t resource, int amount){
	if(amount < 0) return;
	if(resource < 0 || resource >= LAST_RESOURCE) return;
	resPerTurn[resource] = amount;
}

void Colony::AddResourceIncome(const resource_t resource, int amount){
	if(resource < 0 || resource >= LAST_RESOURCE) return;
	resPerTurn[resource] += amount;
}*/

std::string Colony::Name() const{
	return name;
}

int Colony::Row() const{
	return terrain[2][2]->Row();
}

int Colony::Column() const{
	return terrain[2][2]->Colm();
}

std::shared_ptr<Tile> Colony::Terrain(const unsigned int row, const unsigned int colm) const{
	if(row < terrain.size() || colm < terrain[row].size())
		return terrain[row][colm];
	return nullptr;
}

const int& Colony::Resource(const resource_t resource) const{
	return resources[resource];
}

const int& Colony::Resource(const int resource) const{
	return Resource(static_cast<resource_t>(resource));
}

/*void Colony::AssignWorker(Unit* worker, const Tile* location){
	worker->SetLocation(location->Row(), location->Colm(), false);
}*/

void Colony::EnqueueBuilding(const BuildingType* type, Tile* clickedTile){
	if(!type){
		std::cerr << "Error: attempted to add a nullptr to Building queue." << std::endl;
		return;
	}
	if(!clickedTile){
		std::cerr << "Error: attempted to add a Building to a nullptr Tile." << std::endl;
		return;
	}

	if(clickedTile->Owner() != 0 && Owner() != clickedTile->Owner()){
		std::cout << "Can not build on occupied territory." << std::endl;
		return;
	}

	/*std::vector<std::shared_ptr<TileType>> allowedTerrain = type->AllowedTerrain();
	bool allowedToBuild = allowedTerrain.empty();
	for(auto& terrainType : allowedTerrain){
		if(terrainType == clickedTile->Type()){
			allowedToBuild = true;
			break;
		}
	}
	if(!allowedToBuild){
		std::cout << "Unable to build a(n) " << type->Name() << " on the "
			<< "selected terrain's Tile type." << std::endl;
		return;
	}*/

	std::array<bool, LAST_RESOURCE> haveEnough;
	std::array<int, LAST_RESOURCE> cost = type->Cost();
	bool canAfford = true;
	for(unsigned int i = 0; i < LAST_RESOURCE; ++i){
		if(resources[i] >= cost[i]){
			haveEnough[i] = true;
		} else {
			haveEnough[i] = false;
			canAfford = false;
		}
	}
	if(canAfford){
		for(unsigned int i = 0; i < LAST_RESOURCE; ++i){
			TakeResource(static_cast<resource_t>(i), cost[i]);
		}
		std::shared_ptr<Building> newBuilding = std::make_shared<Building>
			(ren, "Building", 0, 0, type);
		newBuilding->StartConstruction();
		buildQueue.push_back(newBuilding);
		clickedTile->AddBuilding(newBuilding);
	} else {
		std::cout << "Unable to build a(n) " << type->Name() << " because you lack"
			<< " the following resource(s): ";
		for(unsigned int i = 0; i < LAST_RESOURCE; ++i){
			if(haveEnough[i] == false){
				std::cout << ResourceName(static_cast<resource_t>(i)) << ": " 
					<< resources[i] << "/" << cost[i] << ", ";
			}
		}
		std::cout << "\b\b." << std::endl;
	}
}

void Colony::EnqueueBuilding(const unsigned int id, Tile* dTile){
	if(id >= buildingTypes.size()){
		std::cerr << "Error: asked to enqueue a Building type with an id higher \
			than what we know (" << id << ">" << buildingTypes.size()-1 << ")."
			<< std::endl;
		return;
	}
	EnqueueBuilding(buildingTypes[id].get(), dTile);
}

void Colony::AdvanceQueue(){
	if(buildQueue.size() == 0){
		return;
	}
	buildQueue[0]->BuildTurn();
	if(buildQueue[0]->TurnsLeft() < 1){
		std::cout << "Construction of " << buildQueue[0]->Name() << " complete!"
			<< std::endl;
		buildQueue.erase(buildQueue.begin());
	}
}

void Colony::ProcessTurn(){
	//std::cout << "Colony processing turn..." << std::endl;
	for(auto& row : terrain){
		for(auto& space : row){
			if(space->Owner() != Owner()) continue;
			AddResources(space->Income());
			space->Training();
		}
	}
	AdvanceQueue();
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
