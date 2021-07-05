#pragma once

#include <string>

class Player
{
public:

	Player();

	std::string name;
	int position;
	int score = 0;

	// Comparison operators done for scoreboard calculation
	friend bool operator>(const Player& c1, const Player& c2) { 
		if (c1.score == c2.score) {
			return c1.name < c2.name;
		}
		return c1.score > c2.score;
	}
	friend bool operator==(const Player& c1, const Player& c2) {
		return c1.score == c2.score;
	}
};

