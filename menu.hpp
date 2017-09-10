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

// I feel like this and GameWindow should be implemented as inheriting from
// a generic Window class, but I don't care enough to actually do it yet.
class Menu : public Screen {
	SDL_Renderer* ren;

	std::vector<std::unique_ptr<UIElement>> background;
	std::vector<std::unique_ptr<Button>> buttons;

	void SetBackground(const std::string& filename);
	void PopulateButtons();

	void ProcessEvent(const SDL_Event& event);
	void ProcessKeyPress(const SDL_Event& event);
	void ProcessMouseClick(const SDL_Event& event);
	void Click(const int x, const int y);

	void StartGame();
	static std::function<void()> StartBtnFunc(Menu* menuPtr);

	void Quit();
	static std::function<void()> QuitBtnFunc(Menu* menuPtr);

	public:
		Menu(SDL_Renderer* ren);

		void InputLoop();

		void Render();

		void KeyPress(const SDL_Keycode key);
		void LeftClick(const int x, const int y);
		void RightClick(const int x, const int y);

		~Menu();
};

#endif
