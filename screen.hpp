#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <SDL.h>

class Screen {
	public:
		bool quit = false;

		virtual void KeyPress(const SDL_Keycode) {}
		virtual void LeftClick(const int, const int) {}
		virtual void RightClick(const int, const int) {}

		// could also use virtual void Render(SDL_Renderer* ren) to not save ptr
		virtual void Render() {}

		virtual ~Screen() = default;
};

#endif
