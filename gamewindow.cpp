#include "gamewindow.hpp"

gameWindow::gameWindow(const std::string& title, const int x, const int y,
		const int w, const int h){
	if((SDL_WasInit(SDL_INIT_EVERYTHING) & SDL_INIT_EVERYTHING) == 0){
		if(!InitSDL()) return;
	}
	win = SDL_CreateWindow(title.c_str(), x, y, w, h, SDL_WINDOW_SHOWN);
	if(win == nullptr){
		LogSDLError(std::cout, "CreateWindow");
	} else {
		ren = SDL_CreateRenderer(win, -1,
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if(ren == nullptr){
			LogSDLError(std::cout, "CreateRenderer");
		}
	}
	gfxManager::Initialize(ren);
}

/*gameWindow::gameWindow(gameWindow&& other): win(nullptr), ren(nullptr),
		background(std::move(other.background)), 
		clickables(std::move(other.clickables)), objects(std::move(other.objects)),
		buttons(std::move(other.buttons)) {
	std::swap(win, other.win);
	std::swap(ren, other.ren);
}*/


gameWindow::~gameWindow(){
	SDL_Cleanup(ren, win);
}

void gameWindow::Render(){
	SDL_RenderClear(ren);
	/*for(unsigned int i = 0; i < background.size(); ++i) background[i]->Render();
	for(unsigned int i = 0; i < objects.size(); ++i) objects[i]->Render();
	for(unsigned int i = 0; i < clickables.size(); ++i) clickables[i]->Render();*/
	for(auto& thing : background) thing->Render();
	for(auto& thing : objects	) thing->Render();
	for(auto& thing : clickables) thing->Render();
	for(auto& thing : topLevelUI) thing->Render();
	SDL_RenderPresent(ren);
}

bool gameWindow::InitSDL(){
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
		LogSDLError(std::cout, "SDL_Init");
		return false;
	}
	if((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG){
		LogSDLError(std::cout, "IMG_Init");
		SDL_Quit();
		return false;
	}
	if(TTF_Init() != 0){
		LogSDLError(std::cout, "TTF_Init");
		IMG_Quit();
		SDL_Quit();
		return false;
	}
	gfxObject::defaultFont = TTF_OpenFont(DEFAULT_FONT, DEFAULT_FONT_SIZE);
	return true;
}

void gameWindow::QuitSDL(){
	TTF_CloseFont(gfxObject::defaultFont);
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void gameWindow::AddObject(std::string filename, const int x, const int y){
	objects.push_back(std::make_shared<entity>(ren, filename, x, y));
}

SDL_Renderer* gameWindow::Renderer() const{
	return ren;
}

std::shared_ptr<entity> gameWindow::Object(const int num){
	if(num > static_cast<long>(objects.size()) || num < 0){
		std::cout << "Error: tried to access uiElement " << num <<
			", which does not exist." << std::endl;
		return nullptr;
	}
	return objects[num];
}

std::shared_ptr<entity> gameWindow::ClickedObject(const int x, const int y){
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

std::shared_ptr<entity> gameWindow::SelectedObject(const int x, const int y){
	for(unsigned int i = clickables.size()-1; i < clickables.size(); --i){
		if(clickables[i]->InsideQ(x, y)) return clickables[i];
	}
	return nullptr;
}

std::array<int, 4> gameWindow::Layout() const{
	std::array<int, 4> layout;
	SDL_GetWindowPosition(win, &layout[0], &layout[1]);
	SDL_GetWindowSize(win, &layout[2], &layout[3]);
	return layout;
}

bool gameWindow::Ready() const{
	if(win == nullptr || ren == nullptr) return false;
	return true;
}

void gameWindow::ResetBackground(std::shared_ptr<uiElement> newThing){
	background.clear();
	background.push_back(newThing);
}

void gameWindow::AddToBackground(std::shared_ptr<uiElement> newThing){
	background.push_back(newThing);
}

void gameWindow::ResetObjects(){
	objects.clear();
	clickables.clear();
	topLevelUI.clear();
}

void gameWindow::AddObject(std::shared_ptr<entity> newThing){
	objects.push_back(newThing);
}

void gameWindow::AddClickable(std::shared_ptr<entity> newThing){
	clickables.push_back(newThing);
}

void gameWindow::AddTopLevelUI(std::shared_ptr<uiAggregate> newThing){
	topLevelUI.push_back(newThing);
}

signal_t gameWindow::HandleKeyPress(SDL_Keycode key,
		std::shared_ptr<entity> selected, std::shared_ptr<map> theMap){
	switch(key){
		case SDLK_c:
		case SDLK_s:
			return SCREEN_CHANGE;
		case SDLK_ESCAPE:
			return QUIT;
		case SDLK_RETURN:
			return NEXT_TURN;
		case SDLK_LEFT:
			if(theMap) theMap->MoveView(VIEW_LEFT);
			break;
		case SDLK_RIGHT:
			if(theMap) theMap->MoveView(VIEW_RIGHT);
			break;
		case SDLK_UP:
			if(theMap) theMap->MoveView(VIEW_UP);
			break;
		case SDLK_DOWN:
			if(theMap) theMap->MoveView(VIEW_DOWN);
			break;
		case SDLK_w:
			if(selected && std::dynamic_pointer_cast<person>(selected) && theMap)
				MoveUpLeft(std::dynamic_pointer_cast<person>(selected), theMap);
			break;
		case SDLK_e:
			if(selected && std::dynamic_pointer_cast<person>(selected) && theMap)
				MoveUpRight(std::dynamic_pointer_cast<person>(selected), theMap);
			break;
		case SDLK_a:
			if(selected && std::dynamic_pointer_cast<person>(selected) && theMap)
				MoveLeft(std::dynamic_pointer_cast<person>(selected), theMap);
			break;
		case SDLK_d:
			if(selected && std::dynamic_pointer_cast<person>(selected) && theMap)
				MoveRight(std::dynamic_pointer_cast<person>(selected), theMap);
			break;
		case SDLK_z:
			if(selected && std::dynamic_pointer_cast<person>(selected) && theMap)
				MoveDownLeft(std::dynamic_pointer_cast<person>(selected), theMap);
			break;
		case SDLK_x:
			if(selected && std::dynamic_pointer_cast<person>(selected) && theMap)
				MoveDownRight(std::dynamic_pointer_cast<person>(selected), theMap);
			break;
		default:
			break;
	}
	return NOTHING;
}

signal_t gameWindow::ColonyScreen(std::shared_ptr<colony> col){
	col->MakeColonyScreen(shared_from_this());

	SDL_Event e;
	bool quit = false;
	while(!quit){
		while(SDL_PollEvent(&e)){
			switch(e.type){
				case SDL_QUIT:				
					quit = true;
					break;
				case SDL_KEYUP:			
					switch(HandleKeyPress(e.key.keysym.sym, selected, nullptr)){
						case SCREEN_CHANGE:
							return SCREEN_CHANGE;
						case NEXT_TURN:
							return NEXT_TURN;
						case QUIT:
							quit = true;
							break;
						default:
							break;
					}
				case SDL_MOUSEBUTTONDOWN:	
					if(e.button.button == SDL_BUTTON_LEFT){
						if(selected) selected->Deselect();
						selected = SelectedObject(e.button.x, e.button.y);
						if(selected){
							switch(selected->Select()/100){
								case NEXT_TURN/100:		return NEXT_TURN;
								case SCREEN_CHANGE/100:	return SCREEN_CHANGE;
								default:				break;
							}
						}
					}
					if(e.button.button == SDL_BUTTON_RIGHT && selected){
						std::shared_ptr<entity> obj =
							ClickedObject(e.button.x, e.button.y);
						if(obj && std::dynamic_pointer_cast<person>(selected) &&
								std::dynamic_pointer_cast<tile>(obj)){
							col->AssignWorker(
									std::dynamic_pointer_cast<person>(selected),
									std::dynamic_pointer_cast<tile>(obj));
						}
						if(obj && std::dynamic_pointer_cast<buildingPrototype>(selected)
								&& std::dynamic_pointer_cast<tile>(obj)){
							col->EnqueueBuilding(
									std::dynamic_pointer_cast<buildingPrototype>(selected)->Type(),
									std::dynamic_pointer_cast<tile>(obj));
						}
					}
					break;
			}
		}
		Render();
	}
	return QUIT;
}

signal_t gameWindow::MapScreen(std::shared_ptr<map> theMap, int centerRow,
		int centerColm){
	// we should be able to handle NEXT_TURN without resetting the screen
	// entirely. Put the loop in a separate function.
	MapScreenCenteredOn(theMap, centerRow, centerColm);

	SDL_Event e;
	bool quit = false;
	while(!quit){
		while(SDL_PollEvent(&e)){
			if(std::dynamic_pointer_cast<person>(selected) && 
					std::dynamic_pointer_cast<person>(selected)->Dead()){
				selected.reset();
			}
			switch(e.type){
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_KEYUP:
					switch(HandleKeyPress(e.key.keysym.sym, selected, theMap)){
						case SCREEN_CHANGE:
							return SCREEN_CHANGE;
						case NEXT_TURN:
							return NEXT_TURN;
						case QUIT:
							quit = true;
							break;
						default:
							break;
					}
				case SDL_MOUSEBUTTONDOWN:
					if(e.button.button == SDL_BUTTON_LEFT){
						if(selected){
							selected->Deselect();
							RemoveUnitInfoPanel();
						}
						selected = SelectedObject(e.button.x, e.button.y);
						if(selected){
							switch(selected->Select()/100){
								case NEXT_TURN/100:
									return NEXT_TURN;
								case SCREEN_CHANGE/100:
									return SCREEN_CHANGE;
								default:
									break;
							}
							MakeUnitInfoPanel(selected);
						}
					}
					if(e.button.button == SDL_BUTTON_RIGHT && selected){
						std::shared_ptr<entity> obj =
							ClickedObject(e.button.x, e.button.y);
						if(obj && std::dynamic_pointer_cast<person>(selected) &&
								std::dynamic_pointer_cast<tile>(obj)){
							std::cout << "This character would now move if it "
								"knew how to do so." << std::endl;
						}
					}
					break;
			}
		}
		Render();
	}
	return QUIT;
}

void gameWindow::MapScreenCenteredOn(std::shared_ptr<map> theMap, const int centerRow,
		const int centerColm){
	if(!theMap){
		std::cerr << "Error: attempted to draw from a non-existent map."
			<< std::endl;
		return;
	}
	ResetObjects();
	AddMapTiles(theMap, centerRow, centerColm);

	std::shared_ptr<uiElement> endTurnButton = std::make_shared<uiElement>(ren,
			"endturn", SCREEN_WIDTH-200, 200);
	endTurnButton->EnableButton(END_TURN);
	AddClickable(endTurnButton);
}

void gameWindow::AddMapTiles(std::shared_ptr<map> theMap, const int centerRow, 
		const int centerColm){
	/*std::cout << "Constructing a map centered on [" << centerRow << "," 
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
				/*std::cout << "The tile at [" << i << "," << j << "] should "
					<< "appear on the screen at (" << theMap->Terrain(i,j)->X()
					<< "," << theMap->Terrain(i,j)->Y() << ")." << std::endl;*/
			}
			if(theMap->Terrain(i,j)->HasColony()){
				AddClickable(theMap->Terrain(i,j));
			} else {
				AddObject(theMap->Terrain(i,j));
			}
			for(auto& occ : theMap->Terrain(i,j)->Occupants()){
				AddClickable(occ);
			}
		}
	}
}

void gameWindow::MakeUnitInfoPanel(const std::shared_ptr<entity> source){
	if(!std::dynamic_pointer_cast<person>(source)) return;
	AddTopLevelUI(std::make_shared<unitInfoPanel>(ren, 
				std::dynamic_pointer_cast<person>(source)));
}

void gameWindow::UpdateUnitInfoPanel(const std::shared_ptr<entity> source){
	if(!std::dynamic_pointer_cast<person>(source)) return;
	for(unsigned int i = 0; i < topLevelUI.size(); ++i){
		if(std::dynamic_pointer_cast<unitInfoPanel>(topLevelUI[i])){
			std::dynamic_pointer_cast<unitInfoPanel>(topLevelUI[i])->UpdateHealth(
					std::dynamic_pointer_cast<person>(source));
		}
	}
}

void gameWindow::RemoveUnitInfoPanel(){
	for(unsigned int i = 0; i < topLevelUI.size(); ++i){
		if(std::dynamic_pointer_cast<unitInfoPanel>(topLevelUI[i])){
			topLevelUI.erase(topLevelUI.begin() + i);
		}
	}
}

bool gameWindow::MoveOnMap(std::shared_ptr<person> mover, std::shared_ptr<map> theMap,
		const int newRow, const int newColm){
	if(!mover){
		std::cerr << "Error: attempted to move a person on the map but the "
			<< "person was a nullptr." << std::endl;
		return false;
	}
	if(!mover->Location()){
		std::cerr << "Error: attempted to move a person on the map but they "
			<< "had no initial location." << std::endl;
		return false;
	}
	std::shared_ptr<tile> newLoc = theMap->Terrain(newRow, newColm);
	if(!newLoc){
		std::cerr << "Error: attempted to move a person to an invalid tile."
			<< std::endl;
		return false;
	}
	if(mover->MovesLeft() < 1){
		std::cout << "This unit has used all of its moves." << std::endl;
		return false;
	}
	if(newLoc->Occupants().size() == 0 || mover->Faction() == newLoc->Owner()){
		mover->MoveToTile(newLoc);
	} else if(mover->CanAttack()) {
		person::Fight(mover, newLoc->Defender());
		UpdateUnitInfoPanel(mover);
		if(newLoc->Occupants().size() == 0) mover->MoveToTile(newLoc);
	}
	return true;
}

void gameWindow::MoveUpLeft(std::shared_ptr<person> mover, std::shared_ptr<map> theMap){
	MoveOnMap(mover, theMap, mover->Row()-1, mover->Colm()-1);
}

void gameWindow::MoveUpRight(std::shared_ptr<person> mover, std::shared_ptr<map> theMap){
	MoveOnMap(mover, theMap, mover->Row()-1, mover->Colm()+1);
}

void gameWindow::MoveLeft(std::shared_ptr<person> mover, std::shared_ptr<map> theMap){
	MoveOnMap(mover, theMap, mover->Row(), mover->Colm()-2);
}

void gameWindow::MoveRight(std::shared_ptr<person> mover, std::shared_ptr<map> theMap){
	MoveOnMap(mover, theMap, mover->Row(), mover->Colm()+2);
}

void gameWindow::MoveDownLeft(std::shared_ptr<person> mover, std::shared_ptr<map> theMap){
	MoveOnMap(mover, theMap, mover->Row()+1, mover->Colm()-1);
}

void gameWindow::MoveDownRight(std::shared_ptr<person> mover, std::shared_ptr<map> theMap){
	MoveOnMap(mover, theMap, mover->Row()+1, mover->Colm()+1);
}
