#include "Util.h"
#include "GameManager.h"

int Util::textToInt(sf::Text* t) {
	try {
		return std::stoi(t->getString().toAnsiString());
	}
	catch (std::invalid_argument& e) {
		return -1;
	}
	catch (std::out_of_range& e) {
		return -2;
	}
}



std::string Util::removeLeadingZeros(std::string number) {
	int firstNonZero = number.find_first_not_of('0');
	return (firstNonZero == -1) ? "0" : number.substr(firstNonZero, number.size() - firstNonZero);
}

void Util::setupTextElement(sf::Text* t, sf::Font* font, std::string text, int offsetX, int offsetY, bool editable) {
	t->setFont(*font);
	if (!editable) {
		t->setOutlineColor(sf::Color::White);
		t->setOutlineThickness(1.f);
		t->setFillColor(sf::Color::Black);
	}
	t->setString(text);

	sf::FloatRect bounds = t->getGlobalBounds();
	t->setOrigin(bounds.width / 2, (editable ? 0 : bounds.height / 2));
	t->setPosition(
		GameManager::windowWidth / 2 /*- bounds.width / 2*/ + offsetX,
		GameManager::windowHeight / 2 /*- (editable ? 0 : bounds.height / 2)*/ + offsetY);
}