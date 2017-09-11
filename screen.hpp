#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <SDL.h>

enum ScreenID { SAME_SCREEN, MENU_SCREEN, GAME_SCREEN, CUTSCENE_SCREEN };

class Screen {
	public:
		SDL_Renderer* ren;

		bool quit = false;
		ScreenID wantScreen = SAME_SCREEN;

		virtual void KeyPress(const SDL_Keycode) {}
		virtual void LeftClick(const int, const int) {}
		virtual void RightClick(const int, const int) {}

		// could also use virtual void Render(SDL_Renderer* ren) to not save ptr
		virtual void Render() {}

		virtual ~Screen() = default;
};

#endif
