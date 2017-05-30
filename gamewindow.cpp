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
	for(unsigned int i = 0; i < background.size(); ++i) background[i]->Render();
	for(unsigned int i = 0; i < objects.size(); ++i) objects[i]->Render();
	for(unsigned int i = 0; i < clickables.size(); ++i) clickables[i]->Render();
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
}

void gameWindow::AddObject(std::shared_ptr<entity> newThing){
	objects.push_back(newThing);
}

void gameWindow::AddClickable(std::shared_ptr<entity> newThing){
	clickables.push_back(newThing);
}

signal_t gameWindow::ColonyScreen(std::shared_ptr<colony> col){
	//	there's a segfault in here somewhere having to do with assigning a
	//	colonist to work an illegal space, possibly by doing so twice in a row
	//	under certain circumstances
	col->MakeColonyScreen(shared_from_this());

	SDL_Event e;
	bool quit = false;
	while(!quit){
		while(SDL_PollEvent(&e)){
			switch(e.type){
				case SDL_QUIT:				
					quit = true;
					break;
				case SDL_KEYDOWN:			
					quit = true;
					break;
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

signal_t gameWindow::MapScreen(std::shared_ptr<map> theMap, int centerColm,
		int centerRow){
	int widthToDisplay = SCREEN_WIDTH/TILE_HEIGHT+2;
	int heightToDisplay = SCREEN_WIDTH/TILE_WIDTH+2;
	std::vector<std::vector<std::shared_ptr<tile>>> displayTerrain = 
		theMap->SurroundingTerrain(centerColm, centerRow, 
				widthToDisplay, heightToDisplay);
	std::cout << "This is where we'd display a map screen if one existed, but "
		<< "it's not done yet so instead we'll just show you its first colony."
		<< std::endl;
	return ColonyScreen(theMap->Colony(0));
}
