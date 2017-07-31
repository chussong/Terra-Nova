#include "dialogue.hpp"

void Dialogue::AddLine(std::string line){
	//if(line.length() > 80) line.insert(80, "\n");
	lines.push_back(line);
}

void Dialogue::AddDecisionPoint(DecisionPoint dp){
	decisionPoints.push_back(dp);
}

size_t Dialogue::Length() const{
	return lines.size();
}

const std::string& Dialogue::Line(const size_t lineNum) const{
	return lines[lineNum];
}

Dialogue::DecisionPoint* Dialogue::DecisionAt(const size_t n) {
	for(auto& dp : decisionPoints){
		if(dp.beforeLine == n) return &dp;
	}
	return nullptr;
}

DialogueBox::DialogueBox(SDL_Renderer* ren, Dialogue* dialogue):
	UIElement(ren, "dialogue_box_background", DIALOGUE_BOX_X,
			DIALOGUE_BOX_Y), 
	advanceArrow(ren, "dialogue_box_arrow", 
			DIALOGUE_BOX_X + 9*DIALOGUE_BOX_WIDTH/10,
			DIALOGUE_BOX_Y + 7*DIALOGUE_BOX_HEIGHT/10){
	SetDialogue(dialogue);
}

void DialogueBox::SetDialogue(Dialogue* newDialogue){
	dialogue = newDialogue;
	currentLine = -1;
	Advance();
}

bool DialogueBox::Advance(){
	//std::cout << "Advance: " << currentLine << "->";
	if(!CanAdvance()) return false;
	currentLine = (currentLine + 1);
	if(currentLine == dialogue->Length()) return true;
	currentDecision = dialogue->DecisionAt(currentLine);
	//std::cout << "Current line: " << currentLine << "\nAddress of current "
		//<< "decision: " << currentDecision << std::endl;
	if(currentDecision){
		DisplayDecision();
	} else {
		DisplayLine();
	}
	return false;
}

void DialogueBox::DisplayLine(){
	//std::cout << currentLine << " of " << dialogue->Length() << std::endl;
	if(currentLine >= dialogue->Length()){
		std::cerr << "Error: asked to display line " << currentLine << " which "
			<< "is higher than the dialogue length " << dialogue->Length()
			<< "." << std::endl;
		return;
	}
	SDL_Rect boundingBox;
	boundingBox.x = DIALOGUE_BOX_WIDTH/2;
	boundingBox.y = DIALOGUE_BOX_HEIGHT/2;
	boundingBox.w = DIALOGUE_BOX_WIDTH*9/10;
	boundingBox.h = DIALOGUE_BOX_HEIGHT*9/10;
	AddText(CurrentLine(), boundingBox, dialogueFont);
}

std::string DialogueBox::CurrentLine() const{
	return dialogue->Line(currentLine);
}

bool DialogueBox::CanAdvance() const{
	return currentDecision == nullptr;
}

void DialogueBox::DisplayDecision(){
	if(!currentDecision){
		std::cerr << "Error: asked to display a non-existent decision." << std::endl;
		return;
	}
	std::string decText = "";
	for(auto i = 0u; i < currentDecision->options.size(); ++i){
		decText += std::to_string(i+1) + ". " + currentDecision->options[i] + "\n";
	}
	SDL_Rect boundingBox;
	boundingBox.x = DIALOGUE_BOX_WIDTH/2;
	boundingBox.y = DIALOGUE_BOX_HEIGHT/2;
	boundingBox.w = DIALOGUE_BOX_WIDTH*9/10;
	boundingBox.h = DIALOGUE_BOX_HEIGHT*9/10;
	AddText(decText, boundingBox, dialogueFont);
}

bool DialogueBox::MakeDecision(const unsigned int n){
	if(!currentDecision) return true;
	if(n-1 >= currentDecision->options.size()) return false;
	currentLine = currentDecision->jumpTo[n-1];
	if(currentLine >= dialogue->Length()){
		std::cerr << "Error: decision point jumped a dialogue to an invalid "
			<< "address: " << currentLine << " out of " << dialogue->Length()
			<< "." << std::endl;
	}
	currentDecision = nullptr;
	DisplayLine();
	return true;
}

void DialogueBox::Render() const{
	UIElement::Render();
	if(CanAdvance()) advanceArrow.Render();
}
