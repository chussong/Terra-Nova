#include "tile.hpp"

tile::tile(std::shared_ptr<tileType> type, SDL_Renderer* ren,
		const int row, const int colm): entity(ren, type->Path(), 0, 0),
		type(type), row(row), colm(colm) {}

void tile::Render() const{
	SDL_Rect renderLayout = layout;
	renderLayout.x += MAPDISP_ORIGIN_X;
	renderLayout.y += MAPDISP_ORIGIN_Y;
	sprite->RenderTo(ren, renderLayout);
	if(bldg) bldg->Render();
	for(auto& occ : occupants){
		if(!occ.lock()){
			std::cerr << "Error: attempted to render a non-existent occupant."
				<< std::endl;
			continue;
		}
		occ.lock()->Render();
	}
}

void tile::MoveTo(int x, int y){
	//std::cout << "A tile has been moved to (" << x << "," << y << ")." << std::endl;
	entity::MoveTo(x,y);
	if(bldg) bldg->MoveTo(MAPDISP_ORIGIN_X + x + (TILE_WIDTH - BUILDING_WIDTH)/2,
			MAPDISP_ORIGIN_Y + y + (4*TILE_HEIGHT/3 - BUILDING_HEIGHT)/2);
	for(auto& occ : occupants){
		occ.lock()->MoveTo(MAPDISP_ORIGIN_X + x + (TILE_WIDTH - PERSON_WIDTH)/2,
				MAPDISP_ORIGIN_Y + y + (4*TILE_HEIGHT/3 - PERSON_HEIGHT)/2);
	}
}

// this takes an entire SDL_Rect but only uses the positions, not the sizes
void tile::MoveTo(SDL_Rect newLayout){
	MoveTo(newLayout.x, newLayout.y);
}

void tile::Resize(int w, int h){
	if(bldg) bldg->Resize(bldg->W()*w/this->W(), bldg->H()*h/this->H());
	for(auto& occ : occupants){
		std::shared_ptr<person> lockOcc = occ.lock();
		lockOcc->Resize(lockOcc->W()*w/this->W(), lockOcc->H()*h/this->H());
	}
	entity::Resize(w,h);
}

// this takes an entire SDL_Rect but only uses the sizes, not the positions
void tile::Resize(SDL_Rect newLayout){
	Resize(newLayout.w, newLayout.h);
}

bool tile::InsideQ(const int x, const int y) const {
	/*std::cout << "Testing to see if a click at (" << x << "," << y << ") is "
		<< "inside of a tile at (" << layout.x << "," << layout.y << ")." << std::endl;*/
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

int tile::Select() {
	if(hasColony) return SCREEN_CHANGE;
	return NOTHING;
}

std::shared_ptr<tileType> tile::TileType() const{
	return type.lock();
}

void tile::SetTileType(const std::shared_ptr<tileType> newType){
	ChangeSprite(newType->Path());
	type = newType;
}

std::array<int, LAST_RESOURCE> tile::Income() const{
	std::array<int, LAST_RESOURCE> inc = {{0}};
	if(occupants.size() > 0 || (bldg && bldg->Finished() && bldg->Automatic())){
		if(bldg && !bldg->CanHarvest()) return inc;
		inc = type.lock()->Yield();
		if(bldg && bldg->CanHarvest()){
			for(unsigned int i = 0; i < inc.size(); ++i) 
				inc[i] += bldg->BonusResources()[i];
		}
	}
	return inc;
}

bool tile::HasColony() const{
	return hasColony;
}

void tile::SetHasColony(const bool val){
	if(val == true){
		RemoveBuilding();
		//selectable = true;
		ChangeSprite("terrain/colony");
	}
	if(val == false){
		//selectable = false;
		ChangeSprite(type.lock()->Path());
	}
	hasColony = val;
}

void tile::SetLocation(const int row, const int colm){
	this->row = row;
	this->colm = colm;
}

int tile::Row() const{
	return row;
}

int tile::Colm() const{
	return colm;
}

void tile::AddBuilding(std::shared_ptr<building> newBldg){
	bldg = newBldg;
	SDL_Rect bldgLayout = layout;
	bldgLayout.x += MAPDISP_ORIGIN_X + (TILE_WIDTH - BUILDING_WIDTH)/2;
	bldgLayout.y += MAPDISP_ORIGIN_Y + (4*TILE_HEIGHT/3 - BUILDING_HEIGHT)/2;
	bldg->MoveTo(bldgLayout);
}

void tile::RemoveBuilding(){
	bldg.reset();
}

bool tile::AddOccupant(std::shared_ptr<person> newOccupant){
	if(!newOccupant){
		std::cerr << "Error: attempted to add a blank occupant to a tile."
			<< std::endl;
		return false;
	}
	if((bldg && bldg->MaxOccupants() <= occupants.size()) ||
			(!bldg && occupants.size() > 0)) return false;

	for(auto& oldOccupant : occupants){
		if(oldOccupant.lock() == newOccupant){
			std::cerr << "Error: attempted to add an occupant to a tile who was"
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
	occupants.push_back(newOccupant);
	/*newOccupant->MoveTo(MAPDISP_ORIGIN_X + layout.x + (TILE_WIDTH - PERSON_WIDTH)/2,
			MAPDISP_ORIGIN_Y + layout.y + (4*TILE_HEIGHT/3 - PERSON_HEIGHT)/2);*/
	return true;
}

bool tile::RemoveOccupant(std::shared_ptr<person> removeThis){
	if(!removeThis){
		std::cerr << "Error: attempted to remove a blank occupant from a tile."
			<< std::endl;
		return false;
	}
	for(unsigned int i = occupants.size()-1; i <= occupants.size(); --i){
		if(occupants[i].lock() == removeThis){
			occupants.erase(occupants.begin()+i);
			if(bldg && occupants.empty()) bldg->FinishTraining();
			return true;
		}
	}
	std::cerr << "Error: attempted to remove an occupant from a tile but they "
		<< "were not found among its " << occupants.size() << " occupant(s)."
		<< std::endl;
	return false;
}

std::vector<std::weak_ptr<person>> tile::Occupants() const{
	return occupants;
}

std::shared_ptr<person> tile::Defender() const{
	if(Occupants().size() == 0) return nullptr;
	return Occupants()[0].lock();
}

char tile::Owner() const{
	if(occupants.size() == 0) return 0;
	return Occupants()[0].lock()->Faction();
}

void tile::Training(){
	if(bldg && bldg->NowTraining()){
		bldg->TrainingTurn();
		if(bldg->TurnsToTrain() == 0){
			if(occupants.empty()){
				std::cerr << "Error: training was completed at a building but "
					<< "there were no occupants to receive it." << std::endl;
				return;
			} else {
				if(!occupants[0].lock()->CanRespec()){
					std::cerr << "Error: training was completed at a building "
						<< "but the occupant was unique and could not be "
						<< "retrained. This should not happen." << std::endl;
				} else {
					occupants[0].lock()->ChangeSpec(bldg->NowTraining());
				}
				bldg->FinishTraining();
			}
		}
	}
}

unsigned int tile::MoveCost(const tile& destination,
		const moveCostTable moveCosts){
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

