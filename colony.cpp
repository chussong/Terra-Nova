#include "colony.hpp"

colony::colony(SDL_Renderer* ren, std::shared_ptr<map> myMap,
		const int row, const int colm): ren(ren), myMap(myMap), row(row), 
		colm(colm)
{
	std::string spriteDir = GetSpritePath("sprites");

	if(!colonyBackground) colonyBackground = 
		std::make_shared<uiElement>(ren, spriteDir + COLONY_BACKGROUND, 0, 0);

	// inner ring, starting from right and going CCW
	terrain[0] = std::make_shared<tile>(myMap->Terrain(row, colm+2), ren,
			spriteDir + myMap->TerrainName(row, colm+2) + ".png",
			TILE_X[0], TILE_Y[0]);
	terrain[1] = std::make_shared<tile>(myMap->Terrain(row+1, colm+1), ren,
			spriteDir + myMap->TerrainName(row+1, colm+1) + ".png",
			TILE_X[1], TILE_Y[1]);
	terrain[2] = std::make_shared<tile>(myMap->Terrain(row+1, colm-1), ren,
			spriteDir + myMap->TerrainName(row+1, colm-1) + ".png",
			TILE_X[2], TILE_Y[2]);
	terrain[3] = std::make_shared<tile>(myMap->Terrain(row, colm-2), ren,
			spriteDir + myMap->TerrainName(row, colm-2) + ".png",
			TILE_X[3], TILE_Y[3]);
	terrain[4] = std::make_shared<tile>(myMap->Terrain(row-1, colm-1), ren,
			spriteDir + myMap->TerrainName(row-1, colm-1) + ".png",
			TILE_X[4], TILE_Y[4]);
	terrain[5] = std::make_shared<tile>(myMap->Terrain(row-1, colm+1), ren,
			spriteDir + myMap->TerrainName(row-1, colm+1) + ".png",
			TILE_X[5], TILE_Y[5]);

	// outer ring, starting from right and going CCW
	terrain[6] = std::make_shared<tile>(myMap->Terrain(row, colm+4), ren,
			spriteDir + myMap->TerrainName(row, colm+4) + ".png",
			TILE_X[6], TILE_Y[6]);
	terrain[7] = std::make_shared<tile>(myMap->Terrain(row+1, colm+3), ren,
			spriteDir + myMap->TerrainName(row+1, colm+3) + ".png",
			TILE_X[7], TILE_Y[7]);
	terrain[8] = std::make_shared<tile>(myMap->Terrain(row+2, colm+2), ren,
			spriteDir + myMap->TerrainName(row+2, colm+2) + ".png",
			TILE_X[8], TILE_Y[8]);
	terrain[9] = std::make_shared<tile>(myMap->Terrain(row+2, colm), ren,
			spriteDir + myMap->TerrainName(row+2, colm) + ".png",
			TILE_X[9], TILE_Y[9]);
	terrain[10] = std::make_shared<tile>(myMap->Terrain(row+2, colm-2), ren,
			spriteDir + myMap->TerrainName(row+2, colm-2) + ".png",
			TILE_X[10], TILE_Y[10]);
	terrain[11] = std::make_shared<tile>(myMap->Terrain(row+1, colm-3), ren,
			spriteDir + myMap->TerrainName(row+1, colm-3) + ".png",
			TILE_X[11], TILE_Y[11]);
	terrain[12] = std::make_shared<tile>(myMap->Terrain(row, colm-4), ren,
			spriteDir + myMap->TerrainName(row, colm+4) + ".png",
			TILE_X[12], TILE_Y[12]);
	terrain[13] = std::make_shared<tile>(myMap->Terrain(row-1, colm-3), ren,
			spriteDir + myMap->TerrainName(row-1, colm-3) + ".png",
			TILE_X[13], TILE_Y[13]);
	terrain[14] = std::make_shared<tile>(myMap->Terrain(row-2, colm-2), ren,
			spriteDir + myMap->TerrainName(row-2, colm-2) + ".png",
			TILE_X[14], TILE_Y[14]);
	terrain[15] = std::make_shared<tile>(myMap->Terrain(row-2, colm), ren,
			spriteDir + myMap->TerrainName(row-2, colm) + ".png",
			TILE_X[15], TILE_Y[15]);
	terrain[16] = std::make_shared<tile>(myMap->Terrain(row-2, colm+2), ren,
			spriteDir + myMap->TerrainName(row-2, colm+2) + ".png",
			TILE_X[16], TILE_Y[16]);
	terrain[17] = std::make_shared<tile>(myMap->Terrain(row-1, colm+1), ren,
			spriteDir + myMap->TerrainName(row-1, colm+3) + ".png",
			TILE_X[17], TILE_Y[17]);

	name = "Aurora";

	resources.fill(0);
	resourceCap.fill(RESCAP);
	resPerTurn.fill(0);
	powerSupply = 0;
	powerDemand = 0;

	for(unsigned int i = 0; i < resourcePanels.size(); ++i){
		resourcePanels[i] = std::make_shared<uiElement>(ren,
				spriteDir + ResourceName(static_cast<resource_t>(i)) + "_panel.png", 
				RES_PANEL_X + i*RES_PANEL_WIDTH, RES_PANEL_Y);
		resourcePanels[i]->AddText(std::to_string(resources[i]),
				RES_PANEL_WIDTH/2, 2*RES_PANEL_HEIGHT/3);
	}

	int gridLeftEdge = SCREEN_WIDTH - 50 - 
		BUILDING_GRID_COLUMNS*(BUILDING_WIDTH + 2*BUILDING_GRID_PADDING);
	int gridTopEdge = 350;
	buildingGrid = std::make_shared<uiElement>(ren,
			spriteDir + "buildingGrid.png", gridLeftEdge, gridTopEdge);

	SDL_Color color;
	color.r = 0;
	color.g = 0;
	color.b = 0;
	color.a = 255;
	endTurnButton = std::make_shared<uiElement>(ren, 
			spriteDir + "endturn.png", SCREEN_WIDTH-200, 200);
	endTurnButton->EnableButton(END_TURN);
}

void colony::SetBuildingTypes(
		const std::vector<std::shared_ptr<buildingType>> buildingTypes){
	std::string spriteDir = GetSpritePath("sprites");
	int gridLeftEdge = SCREEN_WIDTH - 50 - 
		BUILDING_GRID_COLUMNS*(BUILDING_WIDTH + 2*BUILDING_GRID_PADDING);
	int gridTopEdge = 350;

	this->buildingTypes = buildingTypes;

	for(unsigned int i = 0; i < buildingButtons.size(); ++i){
		if(i >= buildingTypes.size()) break;
		buildingButtons[i] = std::make_shared<buildingPrototype>(ren, 
				spriteDir + "building.png",
				gridLeftEdge + (2*(i%3) + 1)*BUILDING_GRID_PADDING
					+ (i%3)*BUILDING_WIDTH,
				gridTopEdge + (2*(i/3) + 1)*BUILDING_GRID_PADDING
					+ (i/3)*BUILDING_HEIGHT,
				buildingTypes[i]);
	}

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
	resourcePanels[resource]->SetText(std::to_string(resources[resource]));
	return amount;
}

std::array<int, LAST_RESOURCE> colony::AddResources(
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
		resourcePanels[i]->SetText(std::to_string(resources[i]));
	}
	return leftovers;
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
	resourcePanels[resource]->SetText(std::to_string(resources[resource]));
	return amount;
}

void colony::SetResourceIncome(const resource_t resource, int amount){
	if(amount < 0) return;
	if(resource < 0 || resource >= LAST_RESOURCE) return;
	resPerTurn[resource] = amount;
}

void colony::AddResourceIncome(const resource_t resource, int amount){
	if(resource < 0 || resource >= LAST_RESOURCE) return;
	resPerTurn[resource] += amount;
}

void colony::AddInhabitant(std::shared_ptr<person> inhabitant){
	inhabitants.emplace_back(inhabitant);
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
	if(num < terrain.size()) return terrain[num]->TileType();
	return OCEAN;
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

void colony::AssignWorker(std::shared_ptr<person> worker, 
		const std::shared_ptr<tile> location){
	if(worker->Location()){
		worker->Location()->RemoveOccupant(worker);
	}

	if(location->AddOccupant(worker)) worker->SetLocation(location);
}

void colony::EnqueueBuilding(const std::shared_ptr<buildingType> type,
		const std::shared_ptr<tile> clickedTile){
	if(!type){
		std::cerr << "Error: attempted to add a nullptr to building queue." << std::endl;
		return;
	}
	if(!clickedTile){
		std::cerr << "Error: attempted to add a building to a nullptr tile." << std::endl;
		return;
	}

	std::vector<terrain_t> allowedTerrain = type->AllowedTerrain();
	bool allowedToBuild = allowedTerrain.empty();
	for(auto& terrainType : allowedTerrain){
		if(terrainType == clickedTile->TileType()){
			allowedToBuild = true;
			break;
		}
	}
	if(!allowedToBuild){
		std::cout << "Unable to build a(n) " << type->Name() << " on the "
			<< "selected terrain's tile type." << std::endl;
		return;
	}

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
		std::shared_ptr<building> newBuilding = std::make_shared<building>
			(ren, GetSpritePath("sprites") + "building.png", 0, 0, type);
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

void colony::EnqueueBuilding(const unsigned int id, std::shared_ptr<tile> dTile){
	if(id >= buildingTypes.size()){
		std::cerr << "Error: asked to enqueue a building type with an id higher \
			than what we know (" << id << ">" << buildingTypes.size()-1 << ")."
			<< std::endl;
		return;
	}
	EnqueueBuilding(buildingTypes[id], dTile);
}

void colony::AdvanceQueue(){
	if(buildQueue.size() == 0){
		return;
	}
	buildQueue[0]->NextTurn();
	if(buildQueue[0]->TurnsLeft() < 1){
		std::cout << "Construction of " << buildQueue[0]->Name() << " complete!"
			<< std::endl;
		buildQueue.erase(buildQueue.begin());
	}
}

void colony::ProcessTurn(){
	for(auto& space : terrain){
		AddResources(space->Income());
	}
	AdvanceQueue();
}

void colony::MakeColonyScreen(std::shared_ptr<gameWindow> win) {
	win->ResetBackground(colonyBackground);
	win->ResetObjects();
	DrawTiles(win);
	DrawResources(win);
	DrawColonists(win);
	DrawColonyMisc(win);
}

void colony::DrawTiles(std::shared_ptr<gameWindow> win){
	for(unsigned int i = 0; i < terrain.size(); ++i) win->AddObject(terrain[i]);
}

void colony::DrawResources(std::shared_ptr<gameWindow> win){
	for(unsigned int i = 0; i < resourcePanels.size(); ++i){
//		resourcePanels[i]->SetText(std::to_string(resources[i]));
		win->AddObject(resourcePanels[i]);
	}
}

void colony::DrawColonists(std::shared_ptr<gameWindow> win){
/*	std::string spriteDir = GetSpritePath("sprites");
	std::shared_ptr<entity> placeholderColonist = std::make_shared<entity>(ren,
			spriteDir + "colonist.png", 110, SCREEN_HEIGHT-60);*/
	for(unsigned int i = 0; i < inhabitants.size(); ++i){
		win->AddClickable(inhabitants[i].lock());
	}
}

void colony::DrawColonyMisc(std::shared_ptr<gameWindow> win){
	win->AddObject(buildingGrid);
	win->AddClickable(endTurnButton);
	for(unsigned int i = 0; i < buildingButtons.size(); ++i){
		if(buildingButtons[i] == nullptr) break;
		win->AddClickable(buildingButtons[i]);
	}
}

std::string colony::ResourceName(const resource_t resource){
	switch(resource){
		case FOOD:			return "food";
		case CARBON:		return "carbon";
		case SILICON:		return "silicon";
		case IRON:			return "iron";
		default:			return "RESOURCE NOT FOUND";
	}
}
