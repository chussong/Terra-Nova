#ifndef DIALOGUE_HPP
#define DIALOGUE_HPP

#include <vector>
#include <string>
#include "ui.hpp"
#include "unit.hpp"

class Dialogue {
	public:
		struct DecisionPoint {
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
		std::vector<std::string> lines;
		std::vector<DecisionPoint> decisionPoints;

	public:
		void AddLine(std::string line);
		void AddDecisionPoint(DecisionPoint dp);
		size_t Length() const;
		const std::string& Line(const size_t lineNum) const;

		DecisionPoint* DecisionAt(const size_t);
		bool JumpTo(const size_t destLine);
};

class DialogueBox : public UIElement {
	Unit* speaker = nullptr;
	Dialogue* dialogue = nullptr;
	size_t currentLine = 0;
	UIElement advanceArrow;

	Dialogue::DecisionPoint* currentDecision = nullptr;

	public:
		DialogueBox(SDL_Renderer* ren, Dialogue* dialogue = nullptr);

		void SetDialogue(Dialogue* newDialogue);
		bool Advance();
		void DisplayLine();
		std::string CurrentLine() const;

		bool CanAdvance() const;
		void DisplayDecision();
		bool MakeDecision(const unsigned int n);

		void Render() const;
};


#endif
