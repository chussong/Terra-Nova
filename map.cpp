#include "map.hpp"

namespace TerraNova {

Map::Map(SDL_Renderer* ren, std::vector<std::shared_ptr<TileType>> types):
	ren(ren){
	height = DEFAULT_HEIGHT;
	width = DEFAULT_WIDTH;
	terrain.resize(height);
	for(unsigned int i = 0; i < terrain.size(); ++i) 
		terrain[i].resize(2*width);
	InitTerrain(types);
}
	
/*
void Map::ViewCenteredOn(const int centerColm, const int centerRow, 
		std::shared_ptr<GameWindow> win){
	if(!win){
		std::cerr << "Error: attempted to draw a Map to a nonexistent window."
			<< std::endl;
		return;
	}
	win->ResetObjects();
	DrawTiles(centerColm, centerRow, win);
}

void Map::DrawTiles(const int centerColm, const int centerRow, 
		std::shared_ptr<GameWindow> win){
	for(unsigned int i = 0; i < terrain.size(); ++i){
		for(unsigned int j = 0; j < terrain[i].size(); ++j){
			terrain[i][j]->MoveTo(SCREEN_WIDTH/2 + (j-centerColm)*TILE_WIDTH/2,
					SCREEN_HEIGHT/2 + (i-centerRow)*TILE_HEIGHT);
			win->AddObject(terrain[i][j]);
		}
	}
}
*/

void Map::StartTurn(){
	//CleanExpired(colonies);
	roamers = CheckAndLock(weakRoamers);
	ForAllTiles(&Tile::StartTurn);
	for(auto& col : colonies) col->StartTurn();

	ProcessMovement();
}

void Map::EndTurn(){
	roamers.clear();
	ForAllTiles(&Tile::EndTurn);
	for(auto& col : colonies) col->EndTurn();
}

void Map::InitTerrain(std::vector<std::shared_ptr<TileType>> types){
	for(unsigned int row = 0; row < terrain.size(); ++row){
		for(unsigned int col = row%2; col < terrain[row].size(); col+=2){
			if(col % 2 == 0) terrain[row][col] = 
				std::make_shared<Tile>(FindByName(types, "plains").get(), ren, row, col);
			if(col % 2 == 1) terrain[row][col] = 
				std::make_shared<Tile>(FindByName(types, "mountain").get(), ren, row, col);
		}
	}
}

// Path construction; this is pure Dijkstra with no heuristics
std::vector<std::array<unsigned int, 2>> Map::ShortestPath(const unsigned int startRow, 
		const unsigned int startColm, const unsigned int destRow,
		const unsigned int destColm, const MoveCostTable& moveCosts){
	if(startRow == destRow && startColm == destColm)
		return std::vector<std::array<unsigned int, 2>>();
	if(Tile::MoveCost(*Terrain(destRow, destColm), moveCosts) == -1u)
		return std::vector<std::array<unsigned int, 2>>();
	std::vector<std::vector<unsigned int>> distMap;
	std::vector<std::vector<bool>> closed;
	distMap.resize(NumberOfRows());
	closed.resize (NumberOfRows());
	for(auto& row : distMap){
		row.resize(NumberOfColumns(), -1u  );
	}
	for(auto& row : closed ) row.resize(NumberOfColumns(), false);

	std::array<unsigned int, 3> lowestOpen = {{ startRow, startColm, 0 }};
	distMap[startRow][startColm] = 0;
	std::vector<std::vector<bool>> checked(closed);
	bool done = false;
	do{
		UpdateNode(distMap, lowestOpen[0], lowestOpen[1], lowestOpen[2],
				moveCosts);
		closed[lowestOpen[0]][lowestOpen[1]] = true;

		for(auto i = 0; i < 6; ++i){
			if(lowestOpen[0] + hexAdj[i][0] == destRow
					&& lowestOpen[1] + hexAdj[i][1] == destColm){
				done = true;
				break;
			}
		}
		if(done) break;

		for(auto& row : checked) row.assign(row.size(), false);
		checked[lowestOpen[0]][lowestOpen[1]] = true;
		lowestOpen = FindNextLowestOpen(distMap, closed, checked, lowestOpen);
	}while(true);

	auto ret = BuildPathVector(distMap, destRow, destColm);
	ret.insert(ret.begin(), std::array<unsigned int, 2>({{startRow, startColm}}));
	return ret;
}

void Map::UpdateNode(std::vector<std::vector<unsigned int>>& distMap,
		const unsigned int nodeRow, const unsigned int nodeColm, 
		const unsigned int nodeValue, const MoveCostTable& moveCosts){
	//distMap[nodeRow][nodeColm] = nodeValue;
	std::array<unsigned int, 6> adjacentEdges(ForSurrounding(
				std::function<std::shared_ptr<Tile>(int,int)>(
					[this](int i, int j){return this->Terrain(i,j);}
				),
				nodeRow, nodeColm, 
				std::function<unsigned int(std::shared_ptr<Tile>)>(
					[moveCosts](std::shared_ptr<Tile> tl) 
					{if(!tl) return -1u; return Tile::MoveCost(*tl, moveCosts);}
				)));
	for(auto i = 0; i < 6; ++i){
		if(OutOfBounds(nodeRow + hexAdj[i][0], nodeColm + hexAdj[i][1])
				|| adjacentEdges[i] == -1u) continue;
		// (Map dist from here) -> dist. from start or skip if we've already been
		adjacentEdges[i] = std::min(adjacentEdges[i] + nodeValue,
				distMap[nodeRow + hexAdj[i][0]][nodeColm + hexAdj[i][1]]);
	}
	SetSurrounding(distMap, nodeRow, nodeColm, adjacentEdges);
}

// check all adjacent Tiles
// if an adjacent Tile is closed, run this on it and return output
// if it's open, return its {row, colm, dist}
// return lowest dist of the 6 (directly) checked Tiles
std::array<unsigned int, 3> Map::FindNextLowestOpen(
		const std::vector<std::vector<unsigned int>>& distMap,
		const std::vector<std::vector<bool>>& closedNodes,
		std::vector<std::vector<bool>>& checkedNodes,
		const std::array<unsigned int, 3> startingNode){
	std::array<std::array<unsigned int, 3>, 6> out = {{
		{{-1u, -1u, -1u}},
		{{-1u, -1u, -1u}},
		{{-1u, -1u, -1u}},
		{{-1u, -1u, -1u}},
		{{-1u, -1u, -1u}},
		{{-1u, -1u, -1u}}
	}};

	for(auto i = 0; i < 6; ++i){
		unsigned row = startingNode[0] + hexAdj[i][0];
		unsigned colm = startingNode[1] + hexAdj[i][1];
		if(OutOfBounds(row, colm) || checkedNodes[row][colm]) continue;
		checkedNodes[row][colm] = true;
		if(closedNodes[row][colm]){
			out[i] = FindNextLowestOpen(distMap, closedNodes, checkedNodes, 
					{{row, colm, distMap[row][colm]}});
		} else {
			out[i] = {{row, colm, distMap[row][colm]}};
		}
	}

	return *std::min_element(out.begin(), out.end(), 
			[](std::array<unsigned int, 3> a, std::array<unsigned int, 3> b)
			{return a[2] < b[2];});
}

std::vector<std::array<unsigned int, 2>> Map::BuildPathVector(
		const std::vector<std::vector<unsigned int>>& distMap, 
		const unsigned int destRow, const unsigned int destColm){

	/*std::cout << "BEST PATH HOME AROUND (" << destRow << "," << destColm << "):"
		<< std::endl;*/
	if(distMap[destRow][destColm] == 0 || distMap[destRow][destColm] == -1u) 
		return std::vector<std::array<unsigned int, 2>>();

	std::array<unsigned int, 3> best({{-1u, -1u, -1u}});
	for(auto i = 0; i < 6; ++i){
		unsigned int row = destRow + hexAdj[i][0];
		unsigned int colm = destColm + hexAdj[i][1];
		if(row >= distMap.size() || colm >= distMap[row].size()) continue;
		/*std::cout << "(" << row << "," << colm << "): " << distMap[row][colm]
			<< std::endl;*/
		if(distMap[row][colm] < best[2]) best = {{row, colm, distMap[row][colm]}};
	}
	/*std::cout << "BEST: (" << best[0] << "," << best[1] << "," << best[2] << ")\n"
		<< "-------------------" << std::endl;*/
	std::vector<std::array<unsigned int, 2>> bestPath(BuildPathVector(distMap, best[0], 
				best[1]));
	bestPath.push_back(std::array<unsigned int,2>(
				{{destRow, destColm}}));
	return bestPath;
}

// end of Path construction

// movement execution

// method 2:
void Map::ProcessMovement(){
	std::vector<MoveData> moverData = FindMoverData(roamers);

	// 1: Identify anyone who wants to move into a space where no one else
	//    (a) already is, or (b) also wants to move. Move them and mark 'done'.
	// 2: Identify anyone who wants to move into a space with a stationary Unit.
	//    If ally, bounce, if enemy, fight.
	// 3: Identify anyone who wants to move into a space that someone else wants
	//    to move into. If ally, determine winner randomly, bounce other; if 
	//    enemy, fight.
	// 4: Repeat 1-3 in order until nothing changes.
	// Note that due to short circuiting, the below code always does #1 until
	// things stop changing, then #2 until things stop changing, etc.
	bool somethingChanged;
	do{
		somethingChanged = false;
		somethingChanged = somethingChanged || UncontestedMoves(moverData);
		somethingChanged = somethingChanged || AssaultMoves(moverData);
		somethingChanged = somethingChanged || ClashMoves(moverData);
	}while(somethingChanged);

	// 5: Identify anyone who wants to move into a space occupied by someone who
	//    also wants to move. At this point, (I think) IF AND ONLY IF THERE IS A
	//    CYCLE, they succeed.

	// I'm leaving this unimplemented for now which should make it move obvious
	// that the system isn't done yet

	for(auto& md : moverData){
		if(md.status != MS_FINISHED){
			std::cerr << "Error: movement of " << roamers[md.id]->Name() << " ("
				<< md.id << "), located at (" << roamers[md.id]->Row() << ","
				<< roamers[md.id]->Colm() << "), could not be resolved." 
				<< std::endl;
			md.status = MS_FINISHED;
		}
	}
}

bool Map::UncontestedMoves(std::vector<MoveData>& moverData){
	bool somethingChanged = false;
	for(auto& md1 : moverData){
		if(md1.status != MS_UNCHECKED) continue;
		Unit* defender = Terrain(md1.destination)->Defender();
		if(!defender){
			// no defender, can move unless someone else also wants to
			for(auto& md2 : moverData){
				if(md2.destination == md1.destination && md2.id != md1.id){
					if(roamers[md1.id]->Faction() == roamers[md2.id]->Faction()){
						md1.status = MS_FRIENDCLASH;
						if(md2.status == MS_UNCHECKED) md2.status = MS_FRIENDCLASH;
					} else {
						md1.status = MS_FOECLASH;
						md2.status = MS_FOECLASH;
					}
					break;
				}
			}
			if(md1.status == MS_UNCHECKED){
				MoveUnitTo(roamers[md1.id], md1.destination);
				roamers[md1.id]->AdvancePath();
				md1.status = MS_FINISHED;
				somethingChanged = true;
			}
		} else {
			// if defender isn't moving, bounce if friendly, assault if enemy.
			// This could be ill-advised: if ally is being attacked, we want to
			// interpret this bounce as a support in the impending combat.
			if(!defender->CurrentPath()){
				if(roamers[md1.id]->Faction() == defender->Faction()){
					md1.status = MS_BOUNCE;
				} else {
					md1.status = MS_ASSAULT;
				}
			} else {
				md1.status = MS_TARGETMOVING;
			}
		}
	}
	return somethingChanged;
}

bool Map::AssaultMoves(std::vector<MoveData>& moverData){
	bool somethingChanged = false;
	for(auto& md : moverData){
		if(md.status == MS_ASSAULT){
			// this should actually allow multiple participants in the fight
			Unit* defender = Terrain(md.destination)->Defender();
			if(defender) Unit::Fight(roamers[md.id], defender);
			if(defender->Dead() && !roamers[md.id]->Dead()){
				MoveUnitTo(roamers[md.id], md.destination);
				roamers[md.id]->AdvancePath();
			}
			md.status = MS_FINISHED;
			somethingChanged = true;
		}
	}
	return somethingChanged;
}

bool Map::ClashMoves(std::vector<MoveData>& moverData){
	bool somethingChanged = false;
	for(auto& md : moverData){
		if(md.status == MS_FRIENDCLASH){
			std::vector<MoveData*> clashers;
			for(auto& md2 : moverData){
				if(md2.status == MS_FRIENDCLASH
						&& md2.destination == md.destination){
					clashers.push_back(&md2);
				}
			}
			int win = Random::Int(0, clashers.size()-1);
			MoveUnitTo(roamers[clashers[win]->id], clashers[win]->destination);
			roamers[clashers[win]->id]->AdvancePath();
			for(auto& cl : clashers) cl->status = MS_FINISHED;
			somethingChanged = true;
			continue;
		}
		if(md.status == MS_FOECLASH){
			// this should allow for multiple clashers who want to fight
			MoveData* foe;
			for(auto& md2 : moverData){
				if(md2.status == MS_FOECLASH 
						&& md2.destination == md.destination && md2.id != md.id){
					foe = &md2;
					break;
				}
			}
			if(!foe){
				std::cerr << "Error: roamer with ID " << md.id << " thought it "
					<< "had a foe clash but its opponent could not be found."
					<< std::endl;
				md.status = MS_FINISHED;
				somethingChanged = true;
				continue;
			}
			Unit::Fight(roamers[md.id], roamers[foe->id]);
			if(roamers[foe->id]->Dead() && !roamers[md.id]->Dead()){
				MoveUnitTo(roamers[md.id], md.destination);
				roamers[md.id]->AdvancePath();
			}
			if(roamers[md.id]->Dead() && !roamers[foe->id]->Dead()){
				MoveUnitTo(roamers[foe->id], foe->destination);
				roamers[foe->id]->AdvancePath();
			}
			md.status = MS_FINISHED;
			foe->status = MS_FINISHED;
			somethingChanged = true;
			continue;
		}
	}
	return somethingChanged;
}

/*void Map::ProcessMovement(){
	Clean();
	// We actually need to find everyone who wants to advance and everyone
	// who wants to patrol, check for conflicts, do fights, THEN move them.
	// Outline:
	// 1: find all the Tiles anyone wants to move into
	// 2: find any pairs advancing at each other and give them ORDER_CLASH
	// 3: any Unit in one of the target Tiles gets orders = ORDER_DEFEND
	//    unless it already has ORDER_CLASH or ORDER_HARVEST
	// 4: Do fights: attacker and defender, plus check each Tile adjacent to
	//    each for OPPOSING patrollers, adding them to the fight, even if the
	//    defender did not get ORDER_DEFEND. For a clash, any patrollers must
	//    be adjacent to BOTH attacker and defender; additional Units advancing
	//    into the target space can also join, whichever side they're on. Thus,
	//    a defender can use ORDER_PATROL to weakly defend each adjacent Tile,
	//    or ORDER_ADVANCE into a friendly Tile to defend it strongly.
	// 5: With fights concluded, move everyone with ORDER_ADVANCE and 
	//    ORDER_CLASH as necessary. Ones which did not participate in a fight
	//    can be fired upon by patrollers adjacent to their initial space.
	std::vector<MoveData> moverData = FindMoverData(roamers);
	MoverConflicts conflicts = FindConflicts(moverData);
	for(auto& r : roamers){
		if(r->Orders() == ORDER_ADVANCE){
			if(!r->Path()){
				std::cerr << "Error: a Unit (" << r->Name() << ") attempted to "
					<< "move as ordered but did not have a Path." << std::endl;
				continue;
			}
			std::array<unsigned int, 2> nextStep = r->NextStep();
			std::cout << "Move " << r->Name() << " to (" << nextStep[0] << "," 
				<< nextStep[1] << ")." << std::endl;
			MoveUnitTo(r, nextStep[0], nextStep[1]);
			if(r->Path()->Advance()) r->OrderPatrol();
		}
	}
}*/

std::vector<Map::MoveData> Map::FindMoverData(const std::vector<Unit*>& roamers){
	std::vector<MoveData> ret;
	for(auto i = 0u; i < roamers.size(); ++i){
		if(roamers[i]->Orders() == ORDER_ADVANCE){
			MoveData newMoveData;
			newMoveData.id = i;
			newMoveData.origin = roamers[i]->Location();
			newMoveData.destination = roamers[i]->NextStep();
			newMoveData.status = MS_UNCHECKED;
			ret.push_back(newMoveData);
		}
	}
	return ret;
}

/*MoverConflicts Map::FindConflicts(const std::vector<MoveData>& moverData){
	MoverConflicts ret;
	std::vector<std::array<int,2>> preliminaryConflicts;
	// check for everyone who might be bumping into each other
	for(auto& md1 : moverData){
		for(auto& md2 : moverData){
			if(md1.destination == md2.origin){
				preliminaryConflicts.emplace_back(md1.id, md2.id);
			}
		}
	}
}*/

// end of movement execution

Colony* Map::AddColony(const int row, const int colm, const int faction){
	std::unique_ptr<Colony> newCol = std::make_unique<Colony>(ren, faction);
	Colony* colPtr = newCol.get();
	Terrain(row, colm)->SetColony(colPtr);
	ForTwoSurrounding(std::function<Tile*(int,int)>([this] (int i, int j)
				{ return Terrain(i,j).get(); }),
			row, colm, [colPtr] (Tile* t) 
			{ if(t && !t->LinkedColony()) t->LinkColony(colPtr); });
	colonies.push_back(std::move(newCol));
	return colonies.back().get();
}

void Map::BuildColony(Unit& builder){
	AddColony(builder.Row(), builder.Colm(), builder.Faction());
	builder.BeConsumed();
}

Button Map::MakeBuildColonyButton(Unit& builder){
	return { ren, "button_build_colony", 0, 0,
			std::function<void()>(std::bind(&Map::BuildColony, this, std::ref(builder)))};
}

/*std::shared_ptr<Colony> Map::Colony(const int num){
	if(num < 0 || static_cast<unsigned int>(num) >= colonies.size())
		return nullptr;
	return colonies[num].lock();
}

const std::shared_ptr<Colony> Map::Colony(const int num) const{
	if(num < 0 || static_cast<unsigned int>(num) >= colonies.size())
		return nullptr;
	return colonies[num].lock();
}*/

void Map::AddRoamer(std::shared_ptr<Unit> newRoamer, const int row, const int colm){
	if(!newRoamer){
		std::cerr << "Error: told to add a roamer to a Map, but it was a "
			<< "nullptr." << std::endl;
		return;
	}
	if(OutOfBounds(row, colm)){
		std::cerr << "Error: told to add a roamer to a Map, but the given "
			<< "coordinates (" << row << "," << colm << ") are out of bounds."
			<< std::endl;
		return;
	}
	MoveUnitTo(newRoamer.get(), row, colm);
	weakRoamers.push_back(newRoamer);
}

std::shared_ptr<Tile> Map::Terrain(const int row, const int column) const{
	if(OutOfBounds(row, column)){
		/*if(row != -1 || column != -1){
			std::cout << "Warning: someone requested the out-of-bounds Tile at "
				"(" << row << "," << column << ")." << std::endl;
		}*/
		return nullptr;
	}
	if((row + column)%2 != 0){
		std::cerr << "Error: someone is trying to access the non-existent "
			<< "Tile at row " << row << ", column " << column << "." << std::endl;
		return nullptr;
	}
	if(!terrain[row][column]){
		std::cerr << "Error: someone is trying to access the valid Tile at row "
			<< row << ", column " << column << ", but there's nothing there."
			<< std::endl;
	}
	return terrain[row][column];
}

std::shared_ptr<Tile> Map::Terrain(const std::array<unsigned int,2> coords) const{
	return Terrain(coords[0], coords[1]);
}

std::vector<std::vector<std::shared_ptr<Tile>>> Map::SurroundingTerrain(
		const int row, const int colm){
	std::vector<std::vector<std::shared_ptr<Tile>>> ret;
	ret.resize(5);
	for(unsigned int i = 0; i < ret.size(); ++i){
		ret[i].resize(ret.size() -
				std::abs((static_cast<int>(ret.size())-1)/2 - static_cast<int>(i)));
		for(unsigned int j = 0; j < ret[i].size(); ++j){
			ret[i][j] = Terrain(row - 2 + i, 
					colm - (ret.size()-1) + std::abs((int)i - ((int)ret.size()-1)/2) + 2*j);
		}
	}
	return ret;
}

unsigned int Map::NumberOfRows() const{
	if(terrain.size() < 1){
		std::cerr << "Error: asked for number of rows on a Map, but the Map "
			<< "has not been initialized." << std::endl;
		return 0;
	}
	return terrain.size();
}

unsigned int Map::NumberOfColumns() const{
	if(terrain.size() < 1){
		std::cerr << "Error: asked for number of columns on a Map, but the Map "
			<< "has not been initialized." << std::endl;
		return 0;
	}
	return terrain[0].size();
}

bool Map::OutOfBounds(const unsigned int row, const unsigned int colm) const{
	return row >= NumberOfRows() || colm >= NumberOfColumns();
}

bool Map::OutOfBounds(const int row, const int colm) const{
	return OutOfBounds(static_cast<unsigned int>(row), static_cast<unsigned int>(colm));
}

void Map::SetViewCenter(const int row, const int col){
	viewCenterRow = row;
	viewCenterCol = col;
}

void Map::MoveView(direction_t dir){
	int xShift=0, yShift=0;
	switch(dir){
		case VIEW_DOWN:
			yShift = TILE_HEIGHT;
			++viewCenterRow;
			break;
		case VIEW_UP:
			yShift = -TILE_HEIGHT;
			--viewCenterRow;
			break;
		case VIEW_LEFT:
			xShift = TILE_WIDTH;
			viewCenterCol -= 2;
			break;
		case VIEW_RIGHT:
			xShift = -TILE_WIDTH;
			viewCenterCol += 2;
			break;
	}

	ShiftAllTiles(xShift, yShift);
}

void Map::CenterViewOn(const int row, const int col){
	//std::cout << "Centering view on (" << row << "," << col << ")." << std::endl;
	if(row == viewCenterRow && col == viewCenterCol) return;
	//std::cout << "Moving center of map view from (" << viewCenterRow << ","
		//<< viewCenterCol << ") to (" << row << "," << col << ")." << std::endl;
	int yShift = TILE_HEIGHT*(viewCenterRow - row);
	int xShift = TILE_WIDTH*(viewCenterCol - col)/2;

	ShiftAllTiles(xShift, yShift);
	SetViewCenter(row, col);
}

void Map::ShiftAllTiles(const int xShift, const int yShift){
	for(unsigned int i = 0; i < terrain.size(); ++i){
		for(unsigned int j = i%2; j < terrain[i].size(); j+=2){
			Terrain(i,j)->MoveTo(Terrain(i,j)->X() + xShift,
						Terrain(i,j)->Y() + yShift);
		}
	}
}

// null origin = place Unit on Tile, it wasn't anywhere before
// null destination = remove Unit from Tile, it's not going to a place
// return true if the Unit was moved, false if not
bool Map::MoveUnitTo(Unit* mover, const int row, const int colm){
	if(!mover){
		std::cerr << "Error: a Map was told to move a Unit, but was given a "
			<< "nullptr." << std::endl;
		return false;
	}
	if(OutOfBounds(row, colm)){
		std::cerr << "Error: a Map was told to move a Unit, but the given "
			<< "destination was out of bounds." << std::endl;
		return false;
	}
	Tile* origin = Terrain(mover->Row(), mover->Colm()).get();
	Tile* destination = Terrain(row, colm).get();
	if(!destination){
		std::cerr << "Error: a Map was told to move a Unit, but the "
			<< "destination Tile returned a nullptr." << std::endl;
		return false;
	}
	/*if(mover->MovesLeft() < 1){
		std::cout << "This Unit has used all of its moves." << std::endl;
		return false;
	}*/

	// if there's an enemy there, fight it (DEPRECATED)
	/*if(destination->Occupants().size() != 0 
			&& mover->Faction() != destination->Owner()){
		Unit::Fight(mover, destination->Defender());
		if(destination->Occupants().size() > 0) return true; //enemy lived=>stay
	}*/
	
	// do the actual moving
	if(!destination->AddOccupant(mover->shared_from_base<Unit>())){
		std::cout << "That Tile is already fully occupied." << std::endl;
		return false;
	}
	if(origin) origin->RemoveOccupant(mover);
	mover->MoveSpriteToTile(destination->X(), destination->Y(),
			destination->W(), destination->H());
	mover->SetLocation(destination->Row(), destination->Colm(), origin!=nullptr);
	return true;
}

bool Map::MoveUnitTo(Unit* mover, const std::array<unsigned int,2>& coords){
	return MoveUnitTo(mover, coords[0], coords[1]);
}

std::string Map::TerrainName(const unsigned int row, const unsigned int col){
	if(row < terrain.size() && col < terrain[row].size())
		return Terrain(row, col)->Name();
	return "OUTSIDE_OF_MAP";
}

std::unique_ptr<Path> Map::PathTo(const int startRow, const int startColm, 
		const int destRow, const int destColm, const MoveCostTable& moveCosts){
	std::vector<std::array<unsigned int, 2>> steps(ShortestPath(startRow, startColm,
				destRow, destColm, moveCosts));
	return steps.size() == 0 ? nullptr : std::make_unique<Path>(steps);
}

std::unique_ptr<Path> Map::PathTo(const std::array<unsigned int, 2>& startLoc,
		const std::array<unsigned int, 2>& destLoc,
		const MoveCostTable& moveCosts){
	return PathTo(startLoc[0], startLoc[1], destLoc[0], destLoc[1], moveCosts);
}

unsigned int Map::DistanceBetween(const unsigned int rowA, 
		const unsigned int colA, const unsigned int rowB,
		const unsigned int colB, const MoveCostTable& moveCosts){
	return ShortestPath(rowA, colA, rowB, colB, moveCosts).size();
}

unsigned int Map::DistanceBetween(const std::array<unsigned int, 2>& locA,
		const std::array<unsigned int, 2>& locB, const MoveCostTable& moveCosts){
	return DistanceBetween(locA[0], locA[1], locB[0], locB[1], moveCosts);
}

} // namespace TerraNova
