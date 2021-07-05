#pragma once

#include <string>
#include <vector>

#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>

#include "Scoreboard.h"
#include "Button.h"
#include "Field.h"

class MainMenu
{
public:

	MainMenu(sf::RenderWindow* _window, std::vector<std::string>* _playerNames);
	
	void initialSetup(sf::Texture* t, sf::Font* font);
	void updateText(std::string text);
	void mouseClicked();
	void changeSelection(Field* newSelection);
	void cyclePlayerNames(int direction);
	void passGameParameters(int* rows, int* columns, int* playerCount);

	void drawUI();

	bool readyToStart;
	bool backToGame;

private:
	sf::RenderWindow* window;

	Button playButton;
	Button returnToGameButton;

	Button arrowLeft;
	Button arrowRight;

	Field f_playerName;
	Field f_playerCount;
	Field f_rows;
	Field f_columns;

	std::vector<Button*> buttons{
		&playButton,
		&returnToGameButton,
		&arrowLeft,
		&arrowRight,
		&f_playerName,
		&f_playerCount,
		&f_rows,
		&f_columns
	};
	
	// Non editable texts
	sf::Text t_playerNameInstruction;
	sf::Text t_playerCountInstruction;
	sf::Text t_playerRowsInstruction;
	sf::Text t_playerColumnsInstruction;
	sf::Text t_play;

	// User input texts
	sf::Text t_playerName;
	sf::Text t_playerCount;
	sf::Text t_fieldRows;
	sf::Text t_fieldColumns;

	Field* selectedField;

	std::vector<std::string>* playerNames;

	int playerCount;
	int currentPlayername;
};

