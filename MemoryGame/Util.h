#pragma once

#include <SFML/Main.hpp>
#include <SFML/Graphics/Text.hpp>

#include <string>

namespace Util
{
	int textToInt(sf::Text* t);

	std::string removeLeadingZeros(std::string number);

	void setupTextElement(sf::Text* t, sf::Font* font, std::string text, int offsetX = 0, int offsetY = 0, bool editable = false);

	template<typename T>
	T lerp(T a, T b, float t) {
		return a + t * (b - a);
	}
}

