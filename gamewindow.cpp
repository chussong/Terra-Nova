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
		if(clickables[i]->LeftEdge() < x
				&& x < clickables[i]->RightEdge()
				&& clickables[i]->TopEdge() < y
				&& y < clickables[i]->BottomEdge()){
			return clickables[i];
		}
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
