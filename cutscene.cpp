#include "cutscene.hpp"

namespace {
	//const std::string CUTSCENE_BACKGROUND = "space-colony";

	//const std::string CUTSCENE_BGM = "cutscene_bgm.ogg";

	const std::string CUTSCENE_DIALOGUE = "chapters/1/cutscenes/opening.txt";
} // anonymous namespace

CutsceneScreen::CutsceneScreen(SDL_Renderer* ren) {
	this->ren = ren;
	dialogue = std::make_unique<Dialogue>(CUTSCENE_DIALOGUE);
	dialogueBox = std::make_unique<DialogueBox>(ren, dialogue.get());

	backgroundName = dialogueBox->BackgroundName();
	if (backgroundName.empty()) {
		std::cerr << "Error: dialogue box returned an empty background name. "
			<< "A valid name is required for cutscenes." << std::endl;
	}
	SetBackground(backgroundName);
	bgmName = dialogueBox->BGMName();
	if (bgmName.empty()) {
		std::cerr << "Error: dialogue box returned an empty BGM name. "
			<< "A valid name is required for cutscenes." << std::endl;
	}
	Audio::PlayMusic(bgmName);

	// should actually read this from some definition file
	nextScreen = GAME_SCREEN;
}

CutsceneScreen::~CutsceneScreen() {
}

void CutsceneScreen::SetBackground(const std::string& filename) {
	background.clear();
	background.push_back(std::make_unique<UIElement>(ren, 
				"backgrounds/" + filename, 0, 0));
	//std::cout << "Menu screen background set." << std::endl;
}

void CutsceneScreen::KeyPress(const SDL_Keycode key) {
	switch(key) {
		case SDLK_RETURN:
		case SDLK_RIGHT:  {
							  AdvanceDialogue();
							  return;
						  }
		case SDLK_LEFT:   {
							  BackstepDialogue();
							  return;
						  }
		case SDLK_1:	  {
							  dialogueBox->MakeDecision(1);
							  return;
						  }
		case SDLK_2:	  {
							  dialogueBox->MakeDecision(2);
							  return;
						  }
		case SDLK_3:	  {
							  dialogueBox->MakeDecision(3);
							  return;
						  }
		case SDLK_4:	  {
							  dialogueBox->MakeDecision(4);
							  return;
						  }
		case SDLK_5:	  {
							  dialogueBox->MakeDecision(5);
							  return;
						  }
	}
}

void CutsceneScreen::LeftClick(const int, const int) {
	AdvanceDialogue();
}

void CutsceneScreen::RightClick(const int, const int) {
	BackstepDialogue();
}

void CutsceneScreen::Render() {
	for (auto& bg : background) bg->Render();
	dialogueBox->Render();
}

void CutsceneScreen::AdvanceDialogue() {
	bool endOfDialogue = dialogueBox->Advance();
	if (dialogueBox->BackgroundName() != backgroundName) 
		backgroundName = dialogueBox->BackgroundName();
		SetBackground(backgroundName);
	if (dialogueBox->BGMName() != bgmName) {
		bgmName = dialogueBox->BGMName();
		Audio::PlayMusic(bgmName);
	}
	if (endOfDialogue) ScreenHandoff();
}

void CutsceneScreen::BackstepDialogue() {
	dialogueBox->Backstep();
}

void CutsceneScreen::ScreenHandoff() {
	wantScreen = nextScreen;
}
