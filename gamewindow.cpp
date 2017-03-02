#include "gamewindow.hpp"

void LogSDLError(std::ostream& os, const std::string &msg){
	os << msg << " error: " << SDL_GetError() << std::endl;
}

gfxObject::gfxObject(SDL_Renderer* ren, const std::string& filename,
		const int x, const int y){
	// if(IMG not initialized) print error
	image = IMG_LoadTexture(ren, filename.c_str());
	if(image == nullptr) LogSDLError(std::cout, "LoadTexture");

	layout.x = x;
	layout.y = y;
	SDL_QueryTexture(image, NULL, NULL, &layout.w, &layout.h);
	if(layout.w >= SCREEN_WIDTH) layout.w = SCREEN_WIDTH - 1;
	if(layout.h >= SCREEN_HEIGHT) layout.h = SCREEN_HEIGHT - 1;
}

gfxObject::gfxObject(gfxObject&& other) noexcept: image(nullptr){
	std::swap(image, other.image);
	std::swap(layout, other.layout);
}

gfxObject::~gfxObject(){
	SDL_Cleanup(image);
}

void gfxObject::MoveTo(int x, int y){
	if(x < 0) x = 0;
	if(y < 0) y = 0;
	layout.x = std::min(x, SCREEN_WIDTH - layout.w);
	layout.y = std::min(y, SCREEN_HEIGHT - layout.h);
}

void gfxObject::Resize(int w, int h){
	if(w < 0) w = 0;
	if(h < 0) h = 0;
	layout.w = w;
	layout.h = h;
}

void gfxObject::RenderTo(SDL_Renderer* ren) const{
	if(SDL_RenderCopy(ren, image, nullptr, &layout))
		LogSDLError(std::cout, "RenderCopy");
}

int gfxObject::LeftEdge() const{
	return layout.x;
}

int gfxObject::RightEdge() const{
	return layout.x + layout.w - 1;
}

int gfxObject::TopEdge() const{
	return layout.y;
}

int gfxObject::BottomEdge() const{
	return layout.y + layout.h - 1;
}

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
		} else {
			background = std::make_unique<gfxObject>(ren, 
					GetSpritePath("sprites") + "Space-Colony.png", 0, 0);
		}
	}
}

gameWindow::~gameWindow(){
	SDL_Cleanup(ren, win);
}

void gameWindow::Render(){
	SDL_RenderClear(ren);
	if(background) background->RenderTo(ren);
	for(unsigned int i = 0; i < objects.size(); ++i) objects[i].RenderTo(ren);
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
	return true;
}

void gameWindow::QuitSDL(){
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void gameWindow::AddObject(std::string filename, const int x, const int y){
	objects.emplace_back(ren, filename, x, y);
}

gfxObject* gameWindow::Object(const int num){
	if(num > static_cast<long>(objects.size()) || num < 0){
		std::cout << "Error: tried to access gfxObject " << num <<
			", which does not exist." << std::endl;
		return nullptr;
	}
	return &objects[num];
}

gfxObject* gameWindow::ClickedObject(const int x, const int y){
	for(unsigned int i = objects.size()-1; i < objects.size(); --i){
		if(objects[i].LeftEdge() < x
				&& x < objects[i].RightEdge()
				&& objects[i].TopEdge() < y
				&& y < objects[i].BottomEdge()){
			return &objects[i];
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

colonyWindow::colonyWindow(std::shared_ptr<colony> col, const int x, const int y,
		const int w, const int h): gameWindow(col->Name(), x, y, w, h), col(col){
}

void colonyWindow::Render(){
	if(innerRing.size() < 6) innerRing = col->InnerRing(col->Column(), col->Row(), ren);
	if(outerRing.size() < 12) outerRing = col->OuterRing(col->Column(), col->Row(), ren);
	SDL_RenderClear(ren);
	background->RenderTo(ren);
	for(unsigned int i = 0; i < innerRing.size(); ++i) innerRing[i].RenderTo(ren);
	for(unsigned int i = 0; i < outerRing.size(); ++i) outerRing[i].RenderTo(ren);
	for(unsigned int i = 0; i < objects.size(); ++i) objects[i].RenderTo(ren);
	SDL_RenderPresent(ren);
}

