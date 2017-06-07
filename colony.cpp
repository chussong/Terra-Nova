#include "colony.hpp"

colony::colony(SDL_Renderer* ren, 
		std::vector<std::vector<std::shared_ptr<tile>>> terrain,
		const int faction): 
		ren(ren), faction(faction), terrain(terrain)
{
	name = "Aurora";

	resources.fill(0);
	resourceCap.fill(RESCAP);
	resPerTurn.fill(0);
	powerSupply = 0;
	powerDemand = 0;
}

void colony::SetBuildingTypes(
		const std::vector<std::shared_ptr<buildingType>> buildingTypes){
	this->buildingTypes = buildingTypes;
}

void colony::ChangeName(const std::string name){
	this->name = name;
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
	if(resourcePanels[resource])
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

std::string colony::Name() const{
	return name;
}

int colony::Row() const{
	return terrain[2][2]->Row();
}

int colony::Column() const{
	return terrain[2][2]->Colm();
}

std::shared_ptr<tileType> colony::Terrain(const unsigned int row, const unsigned int colm) const{
	if(row < terrain.size() || colm < terrain[row].size())
		return terrain[row][colm]->TileType();
	return nullptr;
}

int colony::Resource(const resource_t resource) const{
	return resources[resource];
}

void colony::AssignWorker(std::shared_ptr<person> worker, 
		const std::shared_ptr<tile> location){
	worker->MoveToTile(location);
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

	if(clickedTile->Owner() != 0 && Owner() != clickedTile->Owner()){
		std::cout << "Can not build on occupied territory." << std::endl;
		return;
	}

	std::vector<std::shared_ptr<tileType>> allowedTerrain = type->AllowedTerrain();
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
			(ren, "building", 0, 0, type);
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
	buildQueue[0]->BuildTurn();
	if(buildQueue[0]->TurnsLeft() < 1){
		std::cout << "Construction of " << buildQueue[0]->Name() << " complete!"
			<< std::endl;
		buildQueue.erase(buildQueue.begin());
	}
}

void colony::ProcessTurn(){
	for(auto& row : terrain){
		for(auto& space : row){
			if(space->Owner() != Owner()) continue;
			AddResources(space->Income());
			space->Training();
		}
	}
	AdvanceQueue();
}

void colony::MakeColonyScreen(std::shared_ptr<gameWindow> win) {
	if(!win){
		std::cerr << "Error: attempted to draw a colony to a nonexistent window."
			<< std::endl;
		return;
	}
	if(!colonyBackground) colonyBackground = 
		std::make_shared<uiElement>(ren, COLONY_BACKGROUND, 0, 0);

	win->ResetBackground(colonyBackground);
	win->ResetObjects();
	DrawTiles(win);
	DrawResources(win);
	//DrawColonists(win);
	DrawColonyMisc(win);
}

void colony::DrawTiles(std::shared_ptr<gameWindow> win){
	for(unsigned int i = 0; i < terrain.size(); ++i){
		for(unsigned int j = 0; j < terrain[i].size(); ++j){
			terrain[i][j]->MoveTo(TileX(i,j), TileY(i));
			if(terrain[i][j]->HasColony()){
				win->AddClickable(terrain[i][j]);
			} else {
				win->AddObject(terrain[i][j]);
			}
			for(auto& occ : terrain[i][j]->Occupants()) win->AddClickable(occ);
			/*std::cout << "Tile " << i << ", a " << terrain[i]->TileType() << ", "
				<< "moved to (" << terrain[i]->X() << "," << terrain[i]->Y() << ")."
				<< std::endl;*/
		}
	}
}

int colony::TileX(const unsigned int row, const unsigned int colm){
	int ret = 0;
	ret -= (terrain.size()-1)/2 * TILE_WIDTH;
	ret += std::abs((int)row - ((int)terrain.size()-1)/2) * TILE_WIDTH/2;
	ret += colm * TILE_WIDTH;
	return ret;
}

int colony::TileY(const unsigned int row){
	int ret = 0;
	ret -= (static_cast<int>(terrain.size()) - 1)/2 * TILE_HEIGHT;
	ret += row * TILE_HEIGHT;
	return ret;
}

void colony::DrawResources(std::shared_ptr<gameWindow> win){
	for(unsigned int i = 0; i < resourcePanels.size(); ++i){
		resourcePanels[i] = std::make_shared<uiElement>(ren,
				ResourceName(static_cast<resource_t>(i)) + "_panel", 
				RES_PANEL_X + i*RES_PANEL_WIDTH, RES_PANEL_Y);
		resourcePanels[i]->AddText(std::to_string(resources[i]),
				RES_PANEL_WIDTH/2, 2*RES_PANEL_HEIGHT/3);
		win->AddObject(resourcePanels[i]);
	}
}

/*void colony::DrawColonists(std::shared_ptr<gameWindow> win){
	for(unsigned int i = 0; i < inhabitants.size(); ++i){
		win->AddClickable(inhabitants[i].lock());
	}
}*/

void colony::DrawColonyMisc(std::shared_ptr<gameWindow> win){
	int gridLeftEdge = SCREEN_WIDTH - 50 - 
		BUILDING_GRID_COLUMNS*(BUILDING_WIDTH + 2*BUILDING_GRID_PADDING);
	int gridTopEdge = 350;
	buildingGrid = std::make_shared<uiElement>(ren, "buildingGrid", 
			gridLeftEdge, gridTopEdge);
	win->AddObject(buildingGrid);

	for(unsigned int i = 0; i < buildingButtons.size(); ++i){
		if(i >= buildingTypes.size()) break;
		buildingButtons[i] = std::make_shared<buildingPrototype>(ren, "building",
				gridLeftEdge + (2*(i%3) + 1)*BUILDING_GRID_PADDING
					+ (i%3)*BUILDING_WIDTH,
				gridTopEdge + (2*(i/3) + 1)*BUILDING_GRID_PADDING
					+ (i/3)*BUILDING_HEIGHT,
				buildingTypes[i]);
		win->AddClickable(buildingButtons[i]);
	}

	SDL_Color color;
	color.r = 0;
	color.g = 0;
	color.b = 0;
	color.a = 255;
	leaveColonyButton = std::make_shared<uiElement>(ren, "leavecolony",
			SCREEN_WIDTH-200, 100);
	leaveColonyButton->EnableButton(LEAVE_COLONY);
	win->AddClickable(leaveColonyButton);

	endTurnButton = std::make_shared<uiElement>(ren, "endturn", 
			SCREEN_WIDTH-200, 200);
	endTurnButton->EnableButton(END_TURN);
	win->AddClickable(endTurnButton);
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
