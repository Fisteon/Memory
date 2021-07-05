#pragma once

#include "Button.h"

class Field : public Button
{
public:
	Field();
	~Field();

	void setup(
		float sizeX,
		float sizeY,
		float originX,
		float originY,
		sf::Text* linkedTextField,
		bool numberOnly,
		sf::Texture* texture,
		int offsetX = 0,
		int offsetY = 0,
		int textureX = 0,
		int textureY = 0,
		int textureWidth = 0,
		int textureHeight = 0);

	sf::Text* linkedTextField;
	bool numberOnly;
};

