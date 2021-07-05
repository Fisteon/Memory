#pragma once
#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>

class Button : public sf::RectangleShape
{
public:
	Button();
	~Button();
	void setup(
		float sizeX,
		float sizeY,
		float originX,
		float originY,
		sf::Texture* texture,
		int offsetX = 0,
		int offsetY = 0,
		int textureX = 0,
		int textureY = 0,
		int textureWidth = 0,
		int textureHeight = 0);

	bool isClicked(int mouseX, int mouseY);
};

