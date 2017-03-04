#ifndef GAMEWINDOW_HPP
#define GAMEWINDOW_HPP

#include <string>
#include <vector>
#include <array>
#include <iostream>
#include <memory>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "colony.hpp"
#include "ui.hpp"

class uiElement;
class gameWindow {
	protected:
		SDL_Window* win;
		SDL_Renderer* ren;

		std::vector<uiElement> background;
		std::vector<uiElement> clickable;
		std::vector<uiElement> objects;
		std::vector<uiElement> buttons;

	public:
		gameWindow() = delete;
		explicit gameWindow(const std::string& title, const int x, const int y,
			const int w, const int h);
		~gameWindow();

		gameWindow(const gameWindow& other) = delete;
//		gameWindow(gameWindow&& other);
		gameWindow& operator=(const gameWindow other) = delete;

		void Render();
		void AddObject(std::string filename, const int x = 0, const int y = 0);

		uiElement* Object(const int num);
		entity* ClickedObject(const int x, const int y);
		std::array<int, 4> Layout() const;
		bool Ready() const;

		void MakeColonyScreen(const std::shared_ptr<colony> col);

		void AddResourceElements(); // eventually these will be baked in
		void AddInnerRing(const std::shared_ptr<colony> col);
		void AddOuterRing(const std::shared_ptr<colony> col);
		void AddColonists(const std::shared_ptr<colony> col);
		void AddColonyMisc(const std::shared_ptr<colony> col);

		static bool InitSDL();
		static void QuitSDL();
};

#endif
