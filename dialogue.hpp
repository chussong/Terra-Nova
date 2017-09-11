#ifndef DIALOGUE_HPP
#define DIALOGUE_HPP

#include <vector>
#include <string>
#include <regex>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include "ui.hpp"
#include "unit.hpp"
#include "file.hpp"

class Dialogue {
	public:
		struct DecisionPoint {
			std::string name;
			std::string commands; // not sure this is necessary
			//size_t beforeLine; // should deprecate this
			std::vector<std::string> options;
			std::vector<size_t> jumpTo;

			// this first constructor should be deprecated
			/*DecisionPoint(size_t beforeLine, std::vector<std::string> options,
					std::vector<size_t> jumpTo): 
				beforeLine(beforeLine), options(options), jumpTo(jumpTo)
				{ if(options.size() != jumpTo.size()) 
					throw(std::runtime_error("Mismatched options & jumpTo")); }*/

			DecisionPoint(std::string decisionName, 
					std::vector<std::string> options,
					std::vector<size_t> jumpTo): 
				name(decisionName), options(options), jumpTo(jumpTo)
				{ if(options.size() != jumpTo.size()) 
					throw(std::runtime_error("Mismatched options & jumpTo")); }
		};

		struct Hook {
			std::string name;
			size_t lineNumber;

			Hook(std::string name, size_t lineNumber): name(name),
				lineNumber(lineNumber) {}
		};

	private:
		std::vector<std::string> characters;
		std::vector<std::string> lines;
		std::vector<DecisionPoint> decisionPoints;
		std::vector<Hook> hooks;

		static std::vector<std::string> ParseLines(
				const std::vector<std::string>& source);
		static std::vector<Hook> ExtractHooks(std::vector<std::string>& lines);
		static std::vector<DecisionPoint> ParseDecisions(
				const std::vector<std::string>& source, 
				const std::vector<Hook>& hooks);
		static std::string FindOptionText(const std::string& sourceLine);
		static std::string FindHookName(const std::string& sourceLine);

	public:
		Dialogue() = default;
		explicit Dialogue(const std::string& filePath);
		explicit Dialogue(const File::fs::path& filePath);

		void AddLine(std::string line);
		void AddDecisionPoint(DecisionPoint dp);
		size_t Length() const;
		const std::string& Line(const size_t lineNum) const;

		const DecisionPoint* DecisionNamed(const std::string& name) const;
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
	size_t currentLineNumber = 0;
	size_t backstopLineNumber = 0;
	UIElement backstepArrow;
	UIElement advanceArrow;
	UIElement closeBox;

	std::string currentLine = "";
	Dialogue::DecisionPoint const* currentDecision = nullptr;
	bool makingDecision = false;
	bool endAfterCurrentLine = false;

	std::string LoadLine();
	void ParseCommand(const std::string& command);

	public:
		DialogueBox(SDL_Renderer* ren, Dialogue* dialogue = nullptr);

		void SetDialogue(Dialogue* newDialogue);
		bool Advance();
		void Backstep();
		void DisplayLine();
		const std::string& CurrentLine() const;

		bool CanAdvance() const;
		bool CanBackstep() const;
		void DisplayDecision();
		bool MakeDecision(const unsigned int n);

		void ActivateSpeaker(const size_t speakerNumber);

		void Render() const;
};


#endif
