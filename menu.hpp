#ifndef MENU_HPP
#define MENU_HPP

#include <vector>
#include <string>
#include <memory>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_audio.h>

#include "screen.hpp"
#include "sprite.hpp"
#include "ui.hpp"
#include "audio.hpp"

namespace TerraNova {

class Menu : public Screen {
	std::vector<std::unique_ptr<UIElement>> background;
	std::vector<std::unique_ptr<Button>> buttons;

	void SetBackground(const std::string& filename);
	void PopulateButtons();

	void Click(const int x, const int y);

	void PlayCutscene();
	static std::function<void()> CutsceneBtnFunc(Menu* menuPtr);
	void StartGame();
	static std::function<void()> StartBtnFunc(Menu* menuPtr);
	void Quit();
	static std::function<void()> QuitBtnFunc(Menu* menuPtr);

	public:
		Menu(SDL_Renderer* ren);

		void Render();

		void KeyPress(const SDL_Keycode key);
		void LeftClick(const int x, const int y);
		void RightClick(const int x, const int y);

		~Menu();
};

} // namespace TerraNova
#endif
