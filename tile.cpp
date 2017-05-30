#include "tile.hpp"

tile::tile(terrain_t tileType, SDL_Renderer* ren, const std::string spriteFile,
		const int x, const int y): entity(ren, spriteFile, x, y), tileType(tileType)
{
/*	std::cout << "Using the renderer at " << ren <<
		", I have rendered the sprite at " << spriteFile <<
		", resulting in a sprite at " << sprite.get() << std::endl;*/
}

void tile::Render() const{
	sprite->RenderTo(ren, layout);
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

bool tile::InsideQ(const int x, const int y) const {
	int relX = x - layout.x;
	int relY = y - layout.y;
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

terrain_t tile::TileType() const{
	return tileType;
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

void tile::AddBuilding(std::shared_ptr<building> newBldg){
	bldg = newBldg;
	SDL_Rect bldgLayout = layout;
	bldgLayout.x += (TILE_WIDTH - BUILDING_WIDTH)/2;
	bldgLayout.y += (4*TILE_HEIGHT/3 - BUILDING_HEIGHT)/2;
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
	newOccupant->MoveTo(layout.x + (TILE_WIDTH - PERSON_WIDTH)/2,
			layout.y + (4*TILE_HEIGHT/3 - PERSON_HEIGHT)/2);
	return true;
}

bool tile::RemoveOccupant(std::shared_ptr<person> removeThis){
	if(!removeThis){
		std::cerr << "Error: attempted to remove a blank occupant from a tile."
			<< std::endl;
		return false;
	}
	for(unsigned int i = occupants.size(); i <= occupants.size(); --i){
		if(occupants[i] == removeThis){
			occupants.erase(occupants.begin()+i);
			if(bldg && occupants.empty()) bldg->FinishTraining();
			return true;
		}
	}
	std::cerr << "Error: attempted to remove an occupant from a tile but the "
		<< "occupant was not found." << std::endl;
	return false;
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
