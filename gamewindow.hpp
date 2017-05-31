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
#include "ui.hpp"
#include "person.hpp"
#include "building.hpp"
#include "tile.hpp"
#include "colony.hpp"
#include "map.hpp"

class map;
class colony;
class uiElement;
class gameWindow : public std::enable_shared_from_this<gameWindow> {
	protected:
		SDL_Window* win;
		SDL_Renderer* ren;

		std::vector<std::shared_ptr<uiElement>> background;
		std::vector<std::shared_ptr<entity>> clickables;
		std::vector<std::shared_ptr<entity>> objects;

		std::shared_ptr<entity> selected;

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

		SDL_Renderer* Renderer() const;
		std::shared_ptr<entity> Object(const int num);
		std::shared_ptr<entity> SelectedObject(const int x, const int y);
		std::shared_ptr<entity> ClickedObject(const int x, const int y);
		std::array<int, 4> Layout() const;
		bool Ready() const;

		void ResetBackground(std::shared_ptr<uiElement> newThing);
		void AddToBackground(std::shared_ptr<uiElement> newThing);
		void ResetObjects();
		void AddObject(std::shared_ptr<entity> newThing);
		void AddClickable(std::shared_ptr<entity> newThing);

		static bool InitSDL();
		static void QuitSDL();

		signal_t ColonyScreen(std::shared_ptr<colony> col);
		signal_t MapScreen(std::shared_ptr<map> theMap, int centerColm,
				int centerRow);

		void MapScreenCenteredOn(std::shared_ptr<map> theMap, 
				const int centerRow, const int centerColm);
		void AddMapTiles(std::shared_ptr<map> theMap,
				const int centerRow, const int centerColm);
};

#endif
