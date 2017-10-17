#include "tile.hpp"

namespace TerraNova {

Tile::Tile(TileType* type, SDL_Renderer* ren,
		const int row, const int colm): GFXObject(ren, type->Path(), 0, 0),
		type(type), row(row), colm(colm) {}

void Tile::Render() const{
	SDL_Rect renderLayout = layout;
	renderLayout.x += MAPDISP_ORIGIN_X;
	renderLayout.y += MAPDISP_ORIGIN_Y;
	sprite->RenderTo(ren, renderLayout);
	if(bldg) bldg->Render();
	for(auto& occ : occupants){
		if(!occ){
			std::cerr << "Error: attempted to render a non-existent occupant."
				<< std::endl;
			continue;
		}
		occ->Render();
	}
}

void Tile::StartTurn(){
	occupants = CheckAndLock(weakOccupants);
	/*if(occupants.size() > 0){
		std::cout << "The Tile at (" << row << "," << colm << ") has "
			<< occupants.size() << " occupants." << std::endl;
	}*/
	if (linkedColony /*&& (Faction() == linkedColony->Faction())*/) {
		linkedColony->AddResources(this->Income());
	}
	if (bldg && bldg->TurnsLeft() > 0) bldg->BuildTurn();
}

void Tile::EndTurn(){
	occupants.clear();
}

void Tile::MoveTo(int x, int y){
	//std::cout << "A Tile has been moved to (" << x << "," << y << ")." << std::endl;
	GFXObject::MoveTo(x,y);
	if(bldg) bldg->MoveTo(MAPDISP_ORIGIN_X + x + (TILE_WIDTH - BUILDING_WIDTH)/2,
			MAPDISP_ORIGIN_Y + y + (4*TILE_HEIGHT/3 - BUILDING_HEIGHT)/2);
	for(auto& occ : occupants){
		occ->MoveTo(MAPDISP_ORIGIN_X + x + (TILE_WIDTH - PERSON_WIDTH)/2,
				MAPDISP_ORIGIN_Y + y + (4*TILE_HEIGHT/3 - PERSON_HEIGHT)/2);
	}
}

// this takes an entire SDL_Rect but only uses the positions, not the sizes
void Tile::MoveTo(SDL_Rect newLayout){
	MoveTo(newLayout.x, newLayout.y);
}

void Tile::Resize(int w, int h){
	if(bldg) bldg->Resize(bldg->W()*w/this->W(), bldg->H()*h/this->H());
	for(auto& occ : occupants){
		occ->Resize(occ->W()*w/this->W(), occ->H()*h/this->H());
	}
	GFXObject::Resize(w,h);
}

// this takes an entire SDL_Rect but only uses the sizes, not the positions
void Tile::Resize(SDL_Rect newLayout){
	Resize(newLayout.w, newLayout.h);
}

bool Tile::InsideQ(const int x, const int y){
	/*std::cout << "Testing to see if a click at (" << x << "," << y << ") is "
		<< "inside of a Tile at (" << layout.x << "," << layout.y << ")." << std::endl;*/
	int relX = x - layout.x - MAPDISP_ORIGIN_X;
	int relY = y - layout.y - MAPDISP_ORIGIN_Y;
	if(relX < 0 || relY < 0 || relX > layout.w || relY > layout.h ||
			(4*relY < layout.h &&
			 ((2*relX < layout.w - 3.464*relY) || 
			 (2*relX > layout.w + 3.464*relY))) ||
			(4*relY > 3*layout.h &&
			 ((4*relX < 1.732*(4*relY - 3*layout.h)) || 
			 (4*relX > 4*layout.w - 1.732*(4*relY - 3*layout.h)))))
		return false;
	return true;
}

GFXObject* Tile::SelectAt(const int x, const int y) {
	if (bldg && bldg->InsideQ(x, y)) return bldg.get();
	if (InsideQ(x, y)) return this;
	return nullptr;
}

bool Tile::Click() {
	/*if(HasColony()){
		//EnterColony(linkedColony);
		return true;
	}

	return false;*/
	return HasColony();
}

TileType* Tile::Type() const{
	return type;
}

void Tile::SetTileType(TileType* newType){
	ChangeSprite(newType->Path());
	type = newType;
}

std::array<int, LAST_RESOURCE> Tile::Income() const{
	std::array<int, LAST_RESOURCE> inc = {{0}};

	if(bldg && !bldg->CanHarvest()) return inc;

	bool hasHarvester = false;
	for(auto& occ : occupants){
		if(occ->Orders() == ORDER_HARVEST) hasHarvester = true;
	}
	if(bldg && bldg->Finished() && bldg->Automatic()) hasHarvester = true;
	if(!hasHarvester) return inc;

	inc = type->Yield();
	//std::cout << "Yield[0]: " << type->Yield()[0] << std::endl;
	if(bldg){
		for(unsigned int i = 0; i < inc.size(); ++i) 
			inc[i] += bldg->BonusResources()[i];
	}
	return inc;
}

bool Tile::HasColony() const{
	return hasColony;
}

Colony* Tile::LinkedColony() const{
	return linkedColony;
}

void Tile::SetColony(Colony* newColony){
	if(newColony){
		RemoveBuilding();
		//selectable = true;
		ChangeSprite("terrain/colony_p" + std::to_string(newColony->Faction()));
		newColony->SetRow(this->Row());
		newColony->SetColumn(this->Colm());
		LinkColony(newColony);
		hasColony = true;
	} else {
		ChangeSprite(type->Path());
		LinkColony(nullptr);
		hasColony = false;
	}
}

void Tile::LinkColony(Colony* colonyToLink){
	linkedColony = colonyToLink;
}

void Tile::SetLocation(const int row, const int colm){
	this->row = row;
	this->colm = colm;
}

int Tile::Row() const{
	return row;
}

int Tile::Colm() const{
	return colm;
}

void Tile::AddBuilding(std::shared_ptr<Building> newBldg){
	bldg = newBldg;
	SDL_Rect bldgLayout = layout;
	bldgLayout.x += MAPDISP_ORIGIN_X + (TILE_WIDTH - BUILDING_WIDTH)/2;
	bldgLayout.y += MAPDISP_ORIGIN_Y + (4*TILE_HEIGHT/3 - BUILDING_HEIGHT)/2;
	bldg->MoveTo(bldgLayout);
}

void Tile::AddBuilding(const BuildingType* type) {
	AddBuilding(std::make_shared<Building>(ren, 0, 0, type));
}

void Tile::RemoveBuilding(){
	bldg.reset();
}

bool Tile::AddOccupant(std::shared_ptr<Unit> newOccupant){
	if(!newOccupant){
		std::cerr << "Error: attempted to add a blank occupant to a Tile."
			<< std::endl;
		return false;
	}
	if((bldg && bldg->MaxOccupants() <= NumberOfOccupants()) ||
			(!bldg && NumberOfOccupants() > 0)) return false;

	for(auto& oldOccupant : occupants){
		if(oldOccupant == newOccupant.get()){
			std::cerr << "Error: attempted to add an occupant to a Tile who was"
				<< " already occupying it." << std::endl;
			return false;
		}
	}
	if(bldg && !bldg->CanTrain().empty()){
		if(!newOccupant->CanRespec()){
			std::cout << "This character is unique and can not be retrained."
				<< std::endl;
			return false;
		} else {
			bldg->StartTraining(bldg->CanTrain()[0]);
		}
	}
	weakOccupants.push_back(newOccupant);
	occupants.push_back(newOccupant.get());
	/*newOccupant->MoveTo(MAPDISP_ORIGIN_X + layout.x + (TILE_WIDTH - PERSON_WIDTH)/2,
			MAPDISP_ORIGIN_Y + layout.y + (4*TILE_HEIGHT/3 - PERSON_HEIGHT)/2);*/
	return true;
}

bool Tile::RemoveOccupant(Unit* removeThis){
	/*std::cout << "Removing an occupant from the Tile at (" << row << "," << colm
		<< "), which has " << occupants.size() << " occupants." << std::endl;*/
	if(!removeThis){
		std::cerr << "Error: attempted to remove a blank occupant from a Tile."
			<< std::endl;
		return false;
	}
	// doing this instead of remove_if so that we can error if they're not found
	for(unsigned int i = occupants.size()-1; i <= occupants.size(); --i){
		if(occupants[i] == removeThis){
			occupants.erase(occupants.begin()+i);
			weakOccupants.erase(weakOccupants.begin()+i);
			if(bldg && occupants.empty()) bldg->FinishTraining();
			return true;
		}
	}
	std::cerr << "Error: attempted to remove an occupant from the Tile at ("
		<< row << "," << colm << "), but they were not found among its " 
		<< occupants.size() << " occupant(s)." << std::endl;
	return false;
}

std::vector<Unit*> Tile::Occupants() const{
	return occupants;
}

std::shared_ptr<Unit> Tile::SharedOccupant(const unsigned int i) const {
	if(i >= NumberOfOccupants()){
		std::cerr << "Error: someone asked to take ownership of a tile's "
			<< "occupant #" << i << ", but it only has " << NumberOfOccupants()
			<< "." << std::endl;
		return nullptr;
	}
	return weakOccupants[i].lock();
}

unsigned int Tile::NumberOfLivingOccupants() const{
	unsigned int ret = 0u;
	for(auto& occ : occupants) if(!occ->Dead()) ++ret;
	return ret;
}

// eventually we want this to return the strongest occupant, not the first one
Unit* Tile::Defender() const{
	if(NumberOfOccupants() == 0) return nullptr;
	return Occupants()[0];
}

int Tile::Faction() const{
	if(NumberOfOccupants() == 0) return 0;
	return Occupants()[0]->Faction();
}

void Tile::Training(){
	if(bldg && bldg->NowTraining()){
		bldg->TrainingTurn();
		if(bldg->TurnsToTrain() == 0){
			if(occupants.empty()){
				std::cerr << "Error: training was completed at a Building but "
					<< "there were no occupants to receive it." << std::endl;
				return;
			} else {
				if(!occupants[0]->CanRespec()){
					std::cerr << "Error: training was completed at a Building "
						<< "but the occupant was unique and could not be "
						<< "retrained. This should not happen." << std::endl;
				} else {
					occupants[0]->ChangeSpec(bldg->NowTraining());
				}
				bldg->FinishTraining();
			}
		}
	}
}

unsigned int Tile::MoveCost(const Tile& destination,
		const MoveCostTable moveCosts){
	//std::cout << destination.Name() << " move cost:";
	unsigned int cost = moveCosts.base;
	if(destination.Wooded()){
		if(moveCosts.wooded < 0){
			//std::cout << -1u << std::endl;
			return -1u;
		} else {
			cost += moveCosts.wooded;
		}
	}
	if(destination.Aquatic()){
		if(moveCosts.aquatic < 0){
			//std::cout << -1u << std::endl;
			return -1u;
		} else {
			cost += moveCosts.aquatic;
		}
	}
	if(destination.Cold()){
		if(moveCosts.cold < 0){
			//std::cout << -1u << std::endl;
			return -1u;
		} else {
			cost += moveCosts.cold;
		}
	}
	if(destination.Hilly()){
		if(moveCosts.hilly < 0){
			//std::cout << -1u << std::endl;
			return -1u;
		} else {
			cost += moveCosts.hilly;
		}
	}
	//std::cout << cost << std::endl;
	return cost;
}

} // namespace TerraNova
