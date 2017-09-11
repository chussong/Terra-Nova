#include "window.hpp"

int Window::numberOfWindows = 0;

Window::Window(const std::string& title, const int x, const int y,
		const int w, const int h) {
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
		//std::cout << "Renderer constructed at " << ren << "." << std::endl;
	}
	if (numberOfWindows == 0) {
		GFXManager::Initialize(ren);
		Audio::Initialize();
	}
	++numberOfWindows;

	SwitchToMenuScreen();
}

Window::~Window() {
	--numberOfWindows;
	if (numberOfWindows == 0) {
		QuitSDL();
	}
}

bool Window::InitSDL(){
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
	//Sprite::defaultFont = TTF_OpenFont(DEFAULT_FONT, DEFAULT_FONT_SIZE);
	defaultFont = TTF_OpenFont(DEFAULT_FONT, DEFAULT_FONT_SIZE);
	dialogueFont = TTF_OpenFont(DIALOGUE_FONT_NORMAL, DIALOGUE_FONT_SIZE);
	uiFont = TTF_OpenFont(UI_FONT, UI_FONT_SIZE);
	return true;
}

void Window::QuitSDL(){
	TTF_CloseFont(defaultFont);
	TTF_CloseFont(dialogueFont);
	TTF_CloseFont(uiFont);
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void Window::InputLoop() {
	SDL_Event event;
	while(!quit && !(screen && screen->quit)){
		ChangeScreenIfNeeded();

		while(SDL_PollEvent(&event)){
			ProcessEvent(event);
		}
		Render();
		SDL_Delay(17); // should really measure how long the other stuff takes
	}
}

void Window::ProcessEvent(const SDL_Event& event) {
	switch(event.type){
		case SDL_QUIT: {
			quit = true;
			break;
		}
		case SDL_KEYUP: {
			ProcessKeyPress(event);
			break;
		}
		case SDL_MOUSEBUTTONUP: {
			ProcessMouseClick(event);
			break;
		}
	}
}

void Window::ProcessKeyPress(const SDL_Event& event) {
	switch(event.key.keysym.sym) {
		case SDLK_ESCAPE:	{
								quit = true;
								return;
						 	}
		default:			{
								screen->KeyPress(event.key.keysym.sym);
								return;
							}
	}
}

void Window::ProcessMouseClick(const SDL_Event& event) {
	if(event.button.button == SDL_BUTTON_LEFT) {
		screen->LeftClick(event.button.x, event.button.y);
		return;
	}
	if(event.button.button == SDL_BUTTON_RIGHT) {
		screen->RightClick(event.button.x, event.button.y);
		return;
	}
}

void Window::Render() {
	SDL_RenderClear(ren);
	if(screen == nullptr) {
		std::cerr << "Error: asked to render a null screen." << std::endl;
		return;
	} else {
		screen->Render();
	}
	SDL_RenderPresent(ren);
}

void Window::ChangeScreenIfNeeded() {
	if (screen) {
		switch (screen->wantScreen) {
			case SAME_SCREEN: 	  {
								  	  return;
							  	  }
			case MENU_SCREEN: 	  {
								  	  SwitchToMenuScreen();
								  	  return;
							  	  }
			case GAME_SCREEN: 	  {
								  	  SwitchToGameScreen();
									  return;
								  }
			case CUTSCENE_SCREEN: {
									  SwitchToCutsceneScreen();
									  return;
								  }
		}
	}
}

void Window::SwitchToMenuScreen() {
	screen = std::make_unique<Menu>(ren);
}

void Window::SwitchToOptionScreen() {
}

void Window::SwitchToCampaignScreen() {
}

void Window::SwitchToGameScreen() {
	screen = std::make_unique<GameScreen>(ren);
	game = std::make_unique<Game>(dynamic_cast<GameScreen*>(screen.get()));
	game->Begin();
}

void Window::SwitchToCutsceneScreen() {
	screen = std::make_unique<CutsceneScreen>(ren);
}
