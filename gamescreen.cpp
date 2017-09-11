#include "gamescreen.hpp"

GameScreen::GameScreen(SDL_Renderer* ren): playerNumber(1), selected(nullptr) {
	this->ren = ren;
}

void GameScreen::AddEndTurnButton(std::unique_ptr<Button> newButton){
	endTurnButton = std::move(newButton);
	endTurnButton->SetVisible(false);
}

void GameScreen::Render(){
	// change view if necessary
	theMap->CenterViewOn(viewCenterRow, viewCenterCol);
	if(currentFocusColony && !newFocusColony){
		BreakFocusOnColony();
	}
	if(newFocusColony && newFocusColony != currentFocusColony) FocusOnColony();
	ClearOutOfBounds();
	if(selected && selected->IsUnit()){
		if(dynamic_cast<Unit*>(selected)->Dead()){
			ClearSelected();
		}
	}
	//FillHoverText(e.button.x, e.button.y);

	// render everything
	for(auto& thing : background) thing->Render();
	for(auto& thing : objects	) thing->Render();
	for(auto& thing : clickables) thing->Render();
	for(auto& thing : UI        ) thing->Render();
	for(auto& thing : topLevelUI) thing->Render();
	if(endTurnButton) endTurnButton->Render();
	if(dialogueBox) dialogueBox->Render();
	if(hoverTextBox) hoverTextBox->Render();
}

SDL_Renderer* GameScreen::Renderer() const{
	return ren;
}

GFXObject* GameScreen::Object(const int num){
	if(num > static_cast<long>(objects.size()) || num < 0){
		std::cout << "Error: tried to access UIElement " << num <<
			", which does not exist." << std::endl;
		return nullptr;
	}
	return objects[num];
}

GFXObject* GameScreen::ClickedObject(const int x, const int y){
	for(unsigned int i = UI.size()-1; i < UI.size(); --i){
		if(UI[i]->InsideQ(x, y)) return UI[i].get();
	}
	for(unsigned int i = clickables.size()-1; i < clickables.size(); --i){
		if(clickables[i]->InsideQ(x, y)) return clickables[i];
	}
	for(unsigned int i = objects.size()-1; i < objects.size(); --i){
		if(objects[i]->InsideQ(x, y)) return objects[i];
	}
	/*std::cout << "No clickable object detected at clicked location." << std::endl;
	std::cout << "I am aware of " << clickables.size() << " selectable objects "
		<< "and " << objects.size() << " non-selectable objects." << std::endl;*/
	return nullptr;
}

void GameScreen::ClickObject(GFXObject* toClick){
	if(!toClick){
		std::cerr << "Error: somehow asked to ClickObject a nullptr." << std::endl;
		return;
	}
	//std::cout << "Clicked an object." << std::endl;
	if(dynamic_cast<Tile*>(toClick)) ClickTile(dynamic_cast<Tile*>(toClick));
	if(dynamic_cast<Unit*>(toClick)) ClickUnit(dynamic_cast<Unit*>(toClick));
}

void GameScreen::ClickTile(Tile* clickedTile){
	//std::cout << "Clicked a tile at (" << clickedTile->Row() << ","
		//<< clickedTile->Colm() << ")." << std::endl;
	viewCenterRow = clickedTile->Row();
	viewCenterCol = clickedTile->Colm();
	//std::cout << "viewCenter now (" << viewCenterRow << "," << viewCenterCol
		//<< ")." << std::endl;
	//theMap->CenterViewOn(viewCenterRow, viewCenterCol);
	if(clickedTile->HasColony()){
		//std::cout << "Clicked on a colony, setting newFocusColony to "
			//<< clickedTile->LinkedColony() << "." << std::endl;
		if(currentFocusColony != clickedTile->LinkedColony()){
			newFocusColony = clickedTile->LinkedColony();
		} else {
			newFocusColony = nullptr;
		}
	}
}

void GameScreen::ClickUnit(Unit*){
}

// this iterates through stuff backwards so that the most recent thing is on top
GFXObject* GameScreen::SelectedObject(const int x, const int y){
	if(endTurnButton->InsideQ(x, y)){
		if(endTurnButton->Click()) return selected;
		return endTurnButton.get();
	}
	for(unsigned int i = UI.size()-1; i < UI.size(); --i){
		if(UI[i]->InsideQ(x, y)){
			if(UI[i]->Click()) return selected;
			return UI[i].get();
		}
	}
	for(unsigned int i = clickables.size()-1; i < clickables.size(); --i){
		if(clickables[i]->InsideQ(x, y)){
			if(clickables[i]->Click()){
				ClickObject(clickables[i]);
				return selected;
			}
			return clickables[i];
		}
	}
	// object isn't selectable so it can never be returned
	for(unsigned int i = objects.size()-1; i < objects.size(); --i){
		if(objects[i]->InsideQ(x, y)){
			if(objects[i]->Click()){
				ClickObject(objects[i]);
				return selected;
			}
		}
	}
	return nullptr;
}

/*std::array<int, 4> GameScreen::Layout() const{
	std::array<int, 4> layout;
	SDL_GetWindowPosition(win, &layout[0], &layout[1]);
	SDL_GetWindowSize(win, &layout[2], &layout[3]);
	return layout;
}*/

/*bool GameScreen::Ready() const{
	if(win == nullptr || ren == nullptr) return false;
	return true;
}*/

void GameScreen::ClearSelected(){
	if(selected) {
		selected->Deselect();
		selected = nullptr;
	}
	RemoveInfoPanel();
	RemoveOrderPanel();
}

void GameScreen::ResetBackground(std::unique_ptr<UIElement> newThing){
	background.clear();
	background.push_back(std::move(newThing));
}

void GameScreen::AddToBackground(std::unique_ptr<UIElement> newThing){
	background.push_back(std::move(newThing));
}

void GameScreen::ResetObjects(){
	weakObjects.clear();
	objects.clear();
	weakClickables.clear();
	clickables.clear();
	UI.clear();
	topLevelUI.clear();
}

void GameScreen::AddTopLevelUI(std::shared_ptr<UIAggregate> newThing){
	topLevelUI.push_back(newThing);
}

void GameScreen::AddUI(std::unique_ptr<GFXObject> newThing){
	UI.push_back(std::move(newThing));
}

void GameScreen::AddClickable(std::shared_ptr<GFXObject> newThing){
	weakClickables.push_back(newThing);
}

void GameScreen::AddObject(std::shared_ptr<GFXObject> newThing){
	weakObjects.push_back(newThing);
}

signal_t GameScreen::HandleKeyPress(SDL_Keycode key){
	if(dialogueBox){
		switch(key){
			case SDLK_1:
				dialogueBox->MakeDecision(1);
				break;
			case SDLK_2:
				dialogueBox->MakeDecision(2);
				break;
			case SDLK_3:
				dialogueBox->MakeDecision(3);
				break;
			case SDLK_4:
				dialogueBox->MakeDecision(4);
				break;
			case SDLK_5:
				dialogueBox->MakeDecision(5);
				break;
			case SDLK_RIGHT:
			case SDLK_RETURN:
				if(dialogueBox->Advance()) dialogueBox.reset();
				return NOTHING;
			default:
				break;
		}
	} else {
		switch(key){
			case SDLK_ESCAPE:
				return QUIT;
			case SDLK_RETURN:
				return NEXT_TURN;
			case SDLK_LEFT:
				viewCenterCol -= 2;
				break;
			case SDLK_RIGHT:
				viewCenterCol += 2;
				break;
			case SDLK_UP:
				viewCenterRow -= 1;
				break;
			case SDLK_DOWN:
				viewCenterRow += 1;
				break;
			case SDLK_h:
				return SIG_ORDER_HARVEST;
			case SDLK_p:
				return SIG_ORDER_PATROL;
			case SDLK_b:
				if(selected && selected->IsUnit() 
						&& !dynamic_cast<Unit*>(selected)->Unique()){
					theMap->BuildColony(*dynamic_cast<Unit*>(selected));
				}
				break;
			default:
				break;
		}
	}
	return NOTHING;
}

void GameScreen::StartTurn(){
	if(selected) UpdateInfoPanel(selected);
	clickables = CheckAndLock(weakClickables);
	objects = CheckAndLock(weakObjects);
}

void GameScreen::EndTurn(){
	clickables.clear();
	objects.clear();
	if(selected && dynamic_cast<Unit*>(selected) && dynamic_cast<Unit*>(selected)->Dead()){
		ClearSelected();
	}
}

/*void GameScreen::ChangeSelected(GFXObject* newSelected){
	if(newSelected != selected){
		if(selected) selected->Deselect();
	}
	selected = newSelected;
}*/

void GameScreen::FocusOnColony(){
	if(!newFocusColony){
		std::cerr << "Error: told to focus on a colony, but newFocusColony was "
			<< "set to nullptr." << std::endl;
		return;
	}
	ResetObjects();
	const int row = newFocusColony->Row();
	const int colm = newFocusColony->Column();
	ForTwoSurrounding(std::function<std::shared_ptr<Tile>(int,int)>(
				[&] (int i, int j) { return theMap->Terrain(i,j); }),
			row, colm, [this] (std::shared_ptr<Tile> t) 
			{ if(t && t->LinkedColony() == newFocusColony) AddObject(t); });
	objects = CheckAndLock(weakObjects);
	AddClickable(theMap->Terrain(row, colm));
	for(auto& obj : objects){
		Tile* tile = dynamic_cast<Tile*>(obj);
		for(auto i = 0u; i < tile->NumberOfOccupants(); ++i){
			AddClickable(tile->SharedOccupant(i));
		}
	}
	clickables = CheckAndLock(weakClickables);
	DrawResources(newFocusColony);
	DrawColonyMisc(newFocusColony);
	currentFocusColony = newFocusColony;
}

void GameScreen::BreakFocusOnColony(){
	if(!currentFocusColony){
		std::cerr << "Error: asked to break focus on a colony, but was not "
			<< "focusing on one to begin with." << std::endl;
		return;
	}
	ResetObjects();
	AddMapTiles();
	clickables = CheckAndLock(weakClickables);
	objects = CheckAndLock(weakObjects);
	currentFocusColony = nullptr;
}

void GameScreen::DrawResources(const Colony* col){
	for(unsigned int i = 0; i < col->NumberOfResources(); ++i){
		auto newPanel = std::make_unique<UIElement>(ren, 
					Colony::ResourceName(static_cast<resource_t>(i)) + "_panel",
					RES_PANEL_X + i*RES_PANEL_WIDTH, RES_PANEL_Y);
		newPanel->AddDynamicText(col->Resource(i),
				RES_PANEL_WIDTH/2, 2*RES_PANEL_HEIGHT/3);
		AddUI(std::move(newPanel));
	}
}

void GameScreen::DrawColonyMisc(const Colony* col){
	int gridLeftEdge = SCREEN_WIDTH - 50 - 
		BUILDING_GRID_COLUMNS*(BUILDING_WIDTH + 2*BUILDING_GRID_PADDING);
	int gridTopEdge = 350;
	auto buildingGrid = std::make_unique<UIElement>(ren, "building_grid", 
			gridLeftEdge, gridTopEdge);
	AddUI(std::move(buildingGrid));

	const std::vector<BuildingType*>& buildingTypes(Faction::BuildingTypes(col->Faction()));

	for(unsigned int i = 0; i < 9; ++i){
		if(i >= buildingTypes.size()) break;
		
		AddUI(std::make_unique<BuildingPrototype>(ren, "building",
					gridLeftEdge + (2*(i%3) + 1)*BUILDING_GRID_PADDING
						+ (i%3)*BUILDING_WIDTH,
					gridTopEdge + (2*(i/3) + 1)*BUILDING_GRID_PADDING
						+ (i/3)*BUILDING_HEIGHT,
					buildingTypes[i]));
	}

	std::unique_ptr<Button> leaveColonyButton = std::make_unique<Button>(ren,
			"leavecolony", SCREEN_WIDTH-200, 100, 
			std::function<void()>(std::bind(&GameScreen::LeaveColony, this)));
	AddUI(std::move(leaveColonyButton));
}

void GameScreen::ClearOutOfBounds(){
	if(currentFocusColony){
		int centerRow = currentFocusColony->Row();
		int centerCol = currentFocusColony->Column();

		auto it = clickables.begin();
		while(it != clickables.end()){
			if(!(*it)->IsUnit()){
				++it;
				continue;
			}
			Unit* u = dynamic_cast<Unit*>(*it);
			int rowDiff = centerRow - u->Row();
			int colDiff = centerCol - u->Colm();
			if(std::abs(rowDiff) > 2 || std::abs(colDiff) + std::abs(rowDiff) > 4){
				it = clickables.erase(it);
			} else {
				++it;
			}
		}
	}
}

void GameScreen::SelectNew(const int clickX, const int clickY){
	GFXObject* newSelected = SelectedObject(clickX, clickY);
	if(!newSelected && selected){
		selected->Deselect();
		RemoveInfoPanel();
		RemoveOrderPanel();
	} else if(selected != newSelected) {
		if(selected) selected->Deselect();
		newSelected->Select();
		if(newSelected->IsUnit()){
			// below condition should be "if panels are up"
			if(selected && selected->IsUnit() && !dynamic_cast<Unit*>(selected)->Dead()){
				SwapInfoPanel(newSelected);
				SwapOrderPanel(newSelected);
			} else {
				MakeInfoPanel(newSelected);
				MakeOrderPanel(newSelected);
			}
		}
	}
	selected = newSelected;
}

void GameScreen::FillHoverText(const int hoverX, const int hoverY){
	if(SearchForHoverText(UI, hoverX, hoverY)) return;
	if(SearchForHoverText(clickables, hoverX, hoverY)) return;
	//if(SearchForHoverText(objects, hoverX, hoverY)) return;
	hoverTextBox = nullptr;
}

// this should Resize(int,int) the box to be slightly larger than the text
void GameScreen::SetHoverText(const GFXObject& textSource){
	if(!hoverTextBox){
		std::cerr << "Error: asked to set the text in the hover text box, but "
			<< "the box was a nullptr." << std::endl;
		return;
	}
	SDL_Rect textBound = hoverTextBox->Layout();
	textBound.x = textBound.w/2;
	textBound.y = textBound.h/2;
	hoverTextBox->AddText(textSource.HoverText(), textBound, uiFont);
}

/*void GameScreen::PlayDialogue(const std::string& dialoguePath) {
	Dialogue newDialogue(dialoguePath);
	dialogueBox = std::make_unique<DialogueBox>(ren, &testDialogue);
}*/

void GameScreen::PlayDialogue(Dialogue* dialogue) {
	if(!dialogue) {
		std::cerr << "Error: GameScreen was asked to play a null dialogue."
			<< std::endl;
		return;
	}
	dialogueBox = std::make_unique<DialogueBox>(ren, dialogue);
}

void GameScreen::KeyPress(const SDL_Keycode key) {
	signal_t keySig = HandleKeyPress(key);
	switch(keySig/100){
		//case SCREEN_CHANGE/100:
			//return SCREEN_CHANGE;
		case NEXT_TURN/100:
			endTurnButton->Click();
			break;
			//return NEXT_TURN;
		case QUIT/100:
			quit = true;
			break;
		case SIG_GIVE_ORDER/100:
			if(selected && selected->IsUnit()){
				switch(keySig%100){
					case ORDER_PATROL:
						dynamic_cast<Unit*>(selected)->OrderPatrol();
						break;
					case ORDER_HARVEST:
						dynamic_cast<Unit*>(selected)->OrderHarvest();
						break;
				}
			}
			break;
		default:
			break;
	}
}

void GameScreen::LeftClick(const int x, const int y) {
	SelectNew(x, y);
}

void GameScreen::RightClick(const int x, const int y) {
	GFXObject* obj = ClickedObject(x, y);
	Tile* clickedTile = dynamic_cast<Tile*>(obj);
	if (selected->IsUnit() && clickedTile) {
		/*std::cout << "This character will now attempt to "
			"construct a Path to that Tile." << std::endl;*/
		Unit* selectedUnit = dynamic_cast<Unit*>(selected);
		selectedUnit->OrderMove(theMap->PathTo(
				selectedUnit->Row(), selectedUnit->Colm(),
				clickedTile->Row(), clickedTile->Colm(),
				selectedUnit->MoveCosts()));
	}
	Unit* clickedUnit = dynamic_cast<Unit*>(obj);
	if (clickedUnit && selected->IsUnit()) {
		Unit* selectedUnit = dynamic_cast<Unit*>(selected);
		selectedUnit->OrderMove(theMap->PathTo(
				selectedUnit->Row(), selectedUnit->Colm(),
				clickedUnit->Row(), clickedUnit->Colm(),
				selectedUnit->MoveCosts()));
	}
}

void GameScreen::MapScreen(Map* baseMap, int centerRow,
		int centerColm){
	theMap = baseMap;
	MapScreenCenteredOn(centerRow, centerColm);
}

void GameScreen::MapScreenCenteredOn(const int centerRow, const int centerColm){
	std::unique_ptr<UIElement> colonyBackground = 
		std::make_unique<UIElement>(ren, COLONY_BACKGROUND, 0, 0);
	ResetBackground(std::move(colonyBackground));

	ResetObjects();
	CenterMapTiles(centerRow, centerColm);
	AddMapTiles();
	viewCenterRow = centerRow;
	viewCenterCol = centerColm;
	theMap->SetViewCenter(centerRow, centerColm);

	if(!endTurnButton){
		std::cerr << "Warning: told to make a Map screen but the end turn "
			<< "button was a null pointer." << std::endl;
	} else {
		endTurnButton->MoveTo(SCREEN_WIDTH-200, 200);
		endTurnButton->SetVisible(true);
	}
	clickables = CheckAndLock(weakClickables);
	objects = CheckAndLock(weakObjects);
}

// warning: this function moves all of the tiles to be centered on centerRow
// and centerColm, regardless of where the view was before!
void GameScreen::CenterMapTiles(const int centerRow, const int centerColm){
	/*std::cout << "Constructing a Map centered on [" << centerRow << "," 
		<< centerColm << "]." << std::endl;*/
	for(unsigned int i = 0; i < theMap->NumberOfRows(); ++i){
		for(unsigned int j = i%2; j < theMap->NumberOfColumns(); j+=2){
			theMap->Terrain(i,j)->MoveTo(
					0 + (static_cast<int>(j)-centerColm)*TILE_WIDTH/2,
					0 + (static_cast<int>(i)-centerRow)*TILE_HEIGHT);
			if(theMap->Terrain(i,j)->X() > 0
					&& theMap->Terrain(i,j)->X() < SCREEN_WIDTH
					&& theMap->Terrain(i,j)->Y() > 0
					&& theMap->Terrain(i,j)->Y() < SCREEN_HEIGHT){
				/*std::cout << "The Tile at [" << i << "," << j << "] should "
					<< "appear on the screen at (" << theMap->Terrain(i,j)->X()
					<< "," << theMap->Terrain(i,j)->Y() << ")." << std::endl;*/
			}
		}
	}
}

void GameScreen::AddMapTiles(){
	for(unsigned int i = 0; i < theMap->NumberOfRows(); ++i){
		for(unsigned int j = i%2; j < theMap->NumberOfColumns(); j+=2){
			if(theMap->Terrain(i,j)->HasColony()){
				AddClickable(theMap->Terrain(i,j));
			} else {
				AddObject(theMap->Terrain(i,j));
				//AddClickable(theMap->Terrain(i,j));
			}
			// this seems silly; should be able to just get roamers from the map
			for(auto k = 0u; k < theMap->Terrain(i,j)->NumberOfOccupants(); ++k){
				AddClickable(theMap->Terrain(i,j)->SharedOccupant(k));
			}
		}
	}
}

void GameScreen::MakeInfoPanel(const GFXObject* source){
	if(!(source && source->IsUnit())) return;
	AddTopLevelUI(std::make_shared<UnitInfoPanel>(ren, 
				dynamic_cast<const Unit*>(source)));
}

void GameScreen::SwapInfoPanel(const GFXObject* source){
	if(!(source && source->IsUnit())) return;
	for(unsigned int i = 0; i < topLevelUI.size(); ++i){
		if(std::dynamic_pointer_cast<UnitInfoPanel>(topLevelUI[i])){
			std::dynamic_pointer_cast<UnitInfoPanel>(topLevelUI[i])->Update(
					dynamic_cast<const Unit*>(source));
		}
	}
}

void GameScreen::UpdateInfoPanel(const GFXObject* source){
	if(!(source && source->IsUnit())) return;
	for(unsigned int i = 0; i < topLevelUI.size(); ++i){
		if(std::dynamic_pointer_cast<UnitInfoPanel>(topLevelUI[i])){
			std::dynamic_pointer_cast<UnitInfoPanel>(topLevelUI[i])->UpdateHealth(
					dynamic_cast<const Unit*>(source));
		}
	}
}

void GameScreen::RemoveInfoPanel(){
	for(unsigned int i = 0; i < topLevelUI.size(); ++i){
		if(std::dynamic_pointer_cast<UnitInfoPanel>(topLevelUI[i])){
			topLevelUI.erase(topLevelUI.begin() + i);
		}
	}
}

void GameScreen::MakeOrderPanel(GFXObject* source){
	if(!(source && source->IsUnit())) return;
	auto panel = std::make_unique<UnitOrderPanel>(ren, 
			dynamic_cast<Unit*>(source));
	if(!dynamic_cast<Unit*>(source)->Unique()){
		panel->AddButton(theMap->MakeBuildColonyButton(*dynamic_cast<Unit*>(source)));
	}
	AddUI(std::move(panel));
}

// need Build Colony button handed over from Map
void GameScreen::SwapOrderPanel(GFXObject* source){
	if(!(source && source->IsUnit())) return;
	for(unsigned int i = 0; i < UI.size(); ++i){
		if(dynamic_cast<UnitOrderPanel*>(UI[i].get())){
			dynamic_cast<UnitOrderPanel*>(UI[i].get())->Update(
					dynamic_cast<Unit*>(source));
			if(!dynamic_cast<Unit*>(source)->Unique()){
				dynamic_cast<UnitOrderPanel*>(UI[i].get())->AddButton(
					theMap->MakeBuildColonyButton(*dynamic_cast<Unit*>(source)));
			}
		}
	}
}

void GameScreen::RemoveOrderPanel(){
	for(unsigned int i = 0; i < UI.size(); ++i){
		if(dynamic_cast<UnitOrderPanel*>(UI[i].get())){
			UI.erase(UI.begin() + i);
		}
	}
}
