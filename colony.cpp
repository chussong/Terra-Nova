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

	name = "Moonbase JAFFA";

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
		std::array<int, LAST_RESOURCE> oldIncome(worker->Location()->Income());
		for(unsigned int i = 0; i < oldIncome.size(); ++i) resPerTurn[i] -= oldIncome[i];
	}
	worker->SetLocation(location);
	std::array<int, LAST_RESOURCE> newIncome(location->Income());
	for(unsigned int i = 0; i < newIncome.size(); ++i) resPerTurn[i] += newIncome[i];
}

void colony::ProcessTurn(){
	for(int i = 0; i < static_cast<int>(LAST_RESOURCE); ++i){
		AddResource(static_cast<resource_t>(i), resPerTurn[i]);
	}
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
	std::string spriteDir = GetSpritePath("sprites");
	SDL_Color color;
	color.r = 0;
	color.g = 0;
	color.b = 0;
	color.a = 255;
	std::shared_ptr<uiElement> button = std::make_shared<uiElement>(ren, 
			spriteDir + "endturn.png", SCREEN_WIDTH-200, 200);
	button->EnableButton(END_TURN);
	win->AddClickable(button);
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
