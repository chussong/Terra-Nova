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
class uiAggregate;
class tile;

class gameWindow : public std::enable_shared_from_this<gameWindow> {
	protected:
		SDL_Window* win;
		SDL_Renderer* ren;

		std::vector<std::shared_ptr<uiAggregate>> topLevelUI;
		std::vector<std::shared_ptr<entity>> clickables;
		std::vector<std::shared_ptr<entity>> objects;
		std::vector<std::shared_ptr<uiElement>> background;

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
		void AddTopLevelUI(std::shared_ptr<uiAggregate> newThing);

		static bool InitSDL();
		static void QuitSDL();

		signal_t HandleKeyPress(SDL_Keycode key, std::shared_ptr<entity> selected,
				std::shared_ptr<map> theMap);

		signal_t ColonyScreen(std::shared_ptr<colony> col);
		signal_t MapScreen(std::shared_ptr<map> theMap, int centerRow,
				int centerColm);

		void MapScreenCenteredOn(std::shared_ptr<map> theMap, 
				const int centerRow, const int centerColm);
		void AddMapTiles(std::shared_ptr<map> theMap,
				const int centerRow, const int centerColm);
		void MakeUnitInfoPanel(const std::shared_ptr<entity> unit);
		void UpdateUnitInfoPanel(const std::shared_ptr<entity> unit);
		void RemoveUnitInfoPanel();

		bool MoveOnMap(std::shared_ptr<person> mover, std::shared_ptr<map> theMap,
				const int newRow, const int newColm);
		bool MoveUnitToTile(std::shared_ptr<person> mover, tile* origin, tile* destination);
		void MoveUpLeft(std::shared_ptr<person> mover, std::shared_ptr<map> theMap);
		void MoveUpRight(std::shared_ptr<person> mover, std::shared_ptr<map> theMap);
		void MoveLeft(std::shared_ptr<person> mover, std::shared_ptr<map> theMap);
		void MoveRight(std::shared_ptr<person> mover, std::shared_ptr<map> theMap);
		void MoveDownLeft(std::shared_ptr<person> mover, std::shared_ptr<map> theMap);
		void MoveDownRight(std::shared_ptr<person> mover, std::shared_ptr<map> theMap);
};

#endif
