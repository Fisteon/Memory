#pragma once
#include <SFML/Graphics.hpp>
#include "Sprite3d.h"

class Card {
public:
	Card();
	~Card();

	bool guessed;

	int patternNumber;
	sf::Color color;
		
	selbaward::Sprite3d sprite3d;
};

