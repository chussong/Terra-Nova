#ifndef DIALOGUE_HPP
#define DIALOGUE_HPP

#include <vector>
#include <string>
#include <regex>
#include "ui.hpp"
#include "unit.hpp"

class Dialogue {
	public:
		struct DecisionPoint {
			std::string commands;
			size_t beforeLine;
			std::vector<std::string> options;
			std::vector<size_t> jumpTo;

			DecisionPoint(size_t beforeLine, std::vector<std::string> options,
					std::vector<size_t> jumpTo): 
				beforeLine(beforeLine), options(options), jumpTo(jumpTo)
				{ if(options.size() != jumpTo.size()) 
					throw(std::runtime_error("Mismatched options & jumpTo")); }
		};

	private:
		std::vector<std::string> characters;
		std::vector<std::string> lines;
		std::vector<DecisionPoint> decisionPoints;

	public:
		void AddLine(std::string line);
		void AddDecisionPoint(DecisionPoint dp);
		size_t Length() const;
		const std::string& Line(const size_t lineNum) const;

		DecisionPoint* DecisionAt(const size_t);
		bool JumpTo(const size_t destLine);

		void AddCharacter(const std::string& name);
		const std::vector<std::string>& Characters() const;
};

// note: portraits can be mirrored by retooling their Render() to use
// SDL_RenderCopyEx instead of SDL_RenderCopy; this will require modifying
// Sprite as well
class DialoguePortrait : public GFXObject {
	static constexpr int DialoguePortraitX(const unsigned int position){
		//static_assert(position < 4, "Only four character positions supported "
				//"in a dialogue at once.");
		switch(position){
			case 0: return 100;
			case 1: return 200;
			case 2: return 800;
			case 3: return 900;
			default: return 0;
		}
	}
	static constexpr int DialoguePortraitY(const unsigned int position){
		//static_assert(position < 4, "Only four character positions supported "
				//"in a dialogue at once.");
		switch(position){
			case 0: return 200;
			case 1: return 100;
			case 2: return 100;
			case 3: return 200;
			default: return 0;
		}
	}

	std::string name = "CHARACTER_NAME";

	public:
		DialoguePortrait(SDL_Renderer* ren, const std::string& name, 
				const int position): GFXObject(ren, "units/" + name + "/dialogue",
					DialoguePortraitX(position), DialoguePortraitY(position)),
				name(name) {layout.w = 280; layout.h = 400;}

		const std::string& Name() const { return name; }
};

class DialogueBox : public UIElement {
	std::vector<DialoguePortrait> portraits;
	size_t currentSpeaker = 0;
	std::unique_ptr<UIElement> speakerName = nullptr;

	Dialogue* dialogue = nullptr;
	size_t currentLine = 0;
	UIElement advanceArrow;

	Dialogue::DecisionPoint* currentDecision = nullptr;

	void ParseCommand(const std::string& command);

	public:
		DialogueBox(SDL_Renderer* ren, Dialogue* dialogue = nullptr);

		void SetDialogue(Dialogue* newDialogue);
		bool Advance();
		void DisplayLine();
		std::string CurrentLine();

		bool CanAdvance() const;
		void DisplayDecision();
		bool MakeDecision(const unsigned int n);

		void ActivateSpeaker(const size_t speakerNumber);

		void Render() const;
};


#endif
