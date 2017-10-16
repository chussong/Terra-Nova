#ifndef CUTSCENE_HPP
#define CUTSCENE_HPP

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
#include "dialogue.hpp"
#include "file.hpp"

namespace TerraNova {

class CutsceneScreen : public Screen {
	std::vector<std::unique_ptr<UIElement>> background;
	std::unique_ptr<Dialogue> dialogue;
	std::unique_ptr<DialogueBox> dialogueBox;

	ScreenID nextScreen;

	void SetBackground(const std::string& filename);

	void Click(const int x, const int y);

	void AdvanceDialogue();
	void BackstepDialogue();
	void ScreenHandoff();

	std::string backgroundName;
	std::string bgmName;

	public:
		CutsceneScreen(SDL_Renderer* ren);

		void Render();

		void KeyPress(const SDL_Keycode key);
		void LeftClick(const int x, const int y);
		void RightClick(const int x, const int y);

		~CutsceneScreen();
};

} // namespace TerraNova

#endif
