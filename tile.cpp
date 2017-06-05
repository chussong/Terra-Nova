#include "tile.hpp"

tile::tile(terrain_t tileType, SDL_Renderer* ren, const std::string spriteFile,
		const int row, const int colm): entity(ren, spriteFile, 0, 0),
		tileType(tileType), row(row), colm(colm) {}

void tile::Render() const{
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

void tile::MoveTo(int x, int y){
	//std::cout << "A tile has been moved to (" << x << "," << y << ")." << std::endl;
	entity::MoveTo(x,y);
	if(bldg) bldg->MoveTo(MAPDISP_ORIGIN_X + x + (TILE_WIDTH - BUILDING_WIDTH)/2,
			MAPDISP_ORIGIN_Y + y + (4*TILE_HEIGHT/3 - BUILDING_HEIGHT)/2);
	for(std::shared_ptr<person> occ : occupants){
		occ->MoveTo(MAPDISP_ORIGIN_X + x + (TILE_WIDTH - PERSON_WIDTH)/2,
				MAPDISP_ORIGIN_Y + y + (4*TILE_HEIGHT/3 - PERSON_HEIGHT)/2);
	}
}

// this takes an entire SDL_Rect but only uses the positions, not the sizes
void tile::MoveTo(SDL_Rect newLayout){
	MoveTo(newLayout.x, newLayout.y);
}

void tile::Resize(int w, int h){
	entity::Resize(w,h);
	if(bldg) bldg->Resize(w, h);
	for(std::shared_ptr<person> occ : occupants) occ->Resize(w, h);
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
	if(tileType == COLONY) return SCREEN_CHANGE;
	return NOTHING;
}

terrain_t tile::TileType() const{
	return tileType;
}

void tile::SetTileType(const terrain_t newType){
	if(newType == COLONY){
		RemoveBuilding();
		//selectable = true;
	}
	std::string spriteFile;
	switch(newType){
		case MOUNTAIN:
			spriteFile = "mountain";
			break;
		case PLAINS:
			spriteFile = "plains";
			break;
		case OCEAN:
			spriteFile = "ocean";
			break;
		case COAST:
			spriteFile = "coast";
			break;
		case COLONY:
			spriteFile = "colony";
			break;
		default:
			spriteFile = "spriteError";
			break;
	}
	//if(tileType == COLONY && newType != COLONY) selectable = false;
	sprite = gfxManager::RequestSprite(spriteFile);
	sprite->DefaultLayout(layout);
	if(selectable){
		selectedSprite = gfxManager::RequestSprite(spriteFile + "_selected");
	}
	tileType = newType;
}

std::array<int, LAST_RESOURCE> tile::Income() const{
	std::array<int, LAST_RESOURCE> inc = {{0}};
	if(occupants.size() > 0 || (bldg && bldg->Finished() && bldg->Automatic())){
		if(bldg && !bldg->CanHarvest()) return inc;
		switch(tileType){
			case PLAINS:	inc[FOOD] = 4;
							inc[SILICON] = 4;	// for testing only
							break;
			case MOUNTAIN:	inc[IRON] = 4;
							inc[SILICON] = 4;	// for testing only
							break;
			default:		break;
		}
		if(bldg && bldg->CanHarvest()){
			for(unsigned int i = 0; i < inc.size(); ++i) 
				inc[i] += bldg->BonusResources()[i];
		}
	}
	return inc;
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
		if(oldOccupant == newOccupant){
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
		if(occupants[i] == removeThis){
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

std::vector<std::shared_ptr<person>> tile::Occupants() const{
	return occupants;
}

std::shared_ptr<person> tile::Defender() const{
	if(Occupants().size() == 0) return nullptr;
	return Occupants()[0];
}

char tile::Owner() const{
	if(occupants.size() == 0) return 0;
	return Occupants()[0]->Faction();
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
				if(!occupants[0]->CanRespec()){
					std::cerr << "Error: training was completed at a building "
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
