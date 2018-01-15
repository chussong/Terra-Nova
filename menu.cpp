#include "menu.hpp"

namespace TerraNova {

namespace {
	const std::string MENU_BACKGROUND = "space-colony";

	const std::string MENU_BGM = "menu_bgm";

	const std::string PLAY_SCENE_BUTTON_NAME = "play_movie_button";
	constexpr int PLAY_SCENE_BUTTON_X = 300;
	constexpr int PLAY_SCENE_BUTTON_Y = 100;

	const std::string START_GAME_BUTTON_NAME = "start_game_button";
	constexpr int START_GAME_BUTTON_X = 300;
	constexpr int START_GAME_BUTTON_Y = 300;

	const std::string QUIT_BUTTON_NAME = "quit_button";
	constexpr int QUIT_BUTTON_X = 300;
	constexpr int QUIT_BUTTON_Y = 500;
} // anonymous namespace

Menu::Menu(SDL_Renderer* ren) {
	this->ren = ren;
	SetBackground(MENU_BACKGROUND);
	PopulateButtons();
	Audio::PlayMusic(MENU_BGM);
	//std::cout << "Menu screen constructed with renderer at " << ren << "." 
		//<< std::endl;
}

Menu::~Menu() {
}

void Menu::SetBackground(const std::string& filename) {
	background.clear();
	File::Path filePath = File::Path("backgrounds") / filename;
	background.push_back(std::make_unique<UIElement>(ren, filePath.string(),
				0, 0));
	//std::cout << "Menu screen background set." << std::endl;
}

void Menu::PopulateButtons() {
	buttons.clear();
	buttons.push_back(std::make_unique<Button>(ren, PLAY_SCENE_BUTTON_NAME,
				PLAY_SCENE_BUTTON_X, PLAY_SCENE_BUTTON_Y, CutsceneBtnFunc(this)));
	buttons.push_back(std::make_unique<Button>(ren, START_GAME_BUTTON_NAME,
				START_GAME_BUTTON_X, START_GAME_BUTTON_Y, StartBtnFunc(this)));
	buttons.push_back(std::make_unique<Button>(ren, QUIT_BUTTON_NAME,
				QUIT_BUTTON_X, QUIT_BUTTON_Y, QuitBtnFunc(this)));
	//std::cout << "Menu screen buttons added." << std::endl;
}

void Menu::KeyPress(const SDL_Keycode key) {
	switch(key) {
		case SDLK_RETURN: {
							  StartGame();
							  return;
						  }
	}
}

void Menu::LeftClick(const int x, const int y) {
	for (auto& button : buttons) {
		if (button->InsideQ(x, y)) {
			button->Click();
			return;
		}
	}
}

void Menu::RightClick(const int, const int) {
}

void Menu::Render() {
	//std::cout << "Rendering menu screen which has " << background.size() 
		//<< " background objects and " << buttons.size() << " buttons." 
		//<< std::endl;
	for (auto& bg : background) bg->Render();
	for (auto& button : buttons) button->Render();
}

void Menu::PlayCutscene() {
	wantScreen = CUTSCENE_SCREEN;
}

std::function<void()> Menu::CutsceneBtnFunc(Menu* menuPtr) {
	return std::bind(&Menu::PlayCutscene, menuPtr);
}

void Menu::StartGame() {
	wantScreen = GAME_SCREEN;
}

std::function<void()> Menu::StartBtnFunc(Menu* menuPtr) {
	return std::bind(&Menu::StartGame, menuPtr);
}

void Menu::Quit() {
	quit = true;
}

std::function<void()> Menu::QuitBtnFunc(Menu* menuPtr) {
	return std::bind(&Menu::Quit, menuPtr);
}

} // namespace TerraNova
