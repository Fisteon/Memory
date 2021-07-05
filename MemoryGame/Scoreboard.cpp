#include "Scoreboard.h"
#include "GameManager.h"
#include "Util.h"

Scoreboard::Scoreboard()
{
}

Scoreboard::~Scoreboard()
{
}

void Scoreboard::initialSetup(sf::Font* font) {
	frame.setSize(sf::Vector2f(GameManager::windowWidth / 2, GameManager::windowHeight * 0.7f));
	frame.setOrigin(frame.getSize().x / 2, frame.getSize().y / 2);
	frame.setPosition(GameManager::windowWidth / 2, GameManager::windowHeight / 2);
	frame.setFillColor(sf::Color::Black);
	frame.setOutlineColor(sf::Color::Red);
	frame.setOutlineThickness(4.f);

	Util::setupTextElement(&scores, font, "", -frame.getGlobalBounds().width / 2 + 30, -frame.getGlobalBounds().height / 2 + 50);
}