#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <vector>
#include <string>
#include <memory>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_audio.h>

#include "audio.hpp"
#include "screen.hpp"
#include "menu.hpp"

class Window {
	SDL_Renderer* ren;
	SDL_Window* win;
	SDL_AudioSpec audioSpec;

	std::unique_ptr<Screen> screen;

	bool quit = false;

	void InitializeAudio();

	void ProcessEvent(const SDL_Event& event);
	void ProcessKeyPress(const SDL_Event& event);
	void ProcessMouseClick(const SDL_Event& event);

	void StartGame();

	static int numberOfWindows;
	static bool InitSDL();
	static void QuitSDL();

	public:
		Window(const std::string& title, const int x, const int y,
				const int w, const int h);

		Window(const Window& other) = delete;
		Window(Window&& other) = delete;
		Window& operator=(const Window& other) = delete;

		void InputLoop();

		void Render();

		void MenuScreen();
		void OptionScreen();
		void CampaignScreen();
		void GameScreen();

		~Window();
};

#endif
