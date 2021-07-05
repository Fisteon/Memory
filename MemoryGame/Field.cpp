#include "Field.h"
#include "GameManager.h"

Field::Field()
{
}

Field::~Field()
{
}

void Field::setup(
	float sizeX,
	float sizeY, 
	float originX, 
	float originY, 
	sf::Text* linkedTextField, 
	bool numberOnly, 
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

	this->linkedTextField = linkedTextField;
	this->numberOnly = numberOnly;
}
