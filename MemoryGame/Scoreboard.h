#pragma once
#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>

class Scoreboard
{
public:

	Scoreboard();
	~Scoreboard();

	void initialSetup(sf::Font* font);

	sf::RectangleShape frame;
	sf::Text scores;
};

