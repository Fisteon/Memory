#include "Button.h"
#include "GameManager.h"

Button::Button()
{
}

Button::~Button()
{
}

void Button::setup(
	float sizeX,
	float sizeY,
	float originX,
	float originY,
	sf::Texture* texture,
	int offsetX,
	int offsetY,
	int textureX,
	int textureY,
	int textureWidth,
	int textureHeight)
{
	setSize(sf::Vector2f(sizeX, sizeY));
	setOrigin(originX, originY);
	setPosition(GameManager::windowWidth / 2 + offsetX, GameManager::windowHeight / 2 + offsetY);
	setTexture(texture);
	setTextureRect(sf::IntRect(textureX, textureY, textureWidth, textureHeight));
}

bool Button::isClicked(int mouseX, int mouseY) {
	sf::FloatRect x = getGlobalBounds();
	return x.contains(mouseX, mouseY);
}