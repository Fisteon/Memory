#include <cctype>

#include "GameManager.h"
#include "MainMenu.h"
#include "Util.h"

MainMenu::MainMenu(sf::RenderWindow* _window, std::vector<std::string>* _playerNames) :
	window(_window),
	playerNames(_playerNames)
{
	readyToStart = false;
	selectedField = nullptr;
	playerCount = 1;
	currentPlayername = 0;
}

void MainMenu::initialSetup(sf::Texture* t, sf::Font* font)
{
	arrowLeft.setup(30.f, 30.f, 15.f, 15.f, t, -180, -80, 0, 0, 80, 60);
	arrowRight.setup(30.f, 30.f, 15.f, 15.f, t, 180, -80, 80, 0, 80, 60);
	playButton.setup(300.f, 75.f, 150.f, 37.5f, t, 0, 300, 0, 60, 160, 50);
	returnToGameButton.setup(75.f, 75.f, 37.5f, 37.5f, t, -350, 350, 150, 160, 75, 75);

	f_playerName.setup(300.f, 50.f, 150.f, 25.f, &t_playerName, false, t, 0, -80, 0, 60, 160, 50);
	f_playerCount.setup(200.f, 50.f, 100.f, 25.f, &t_playerCount, true, t, 0, -220, 0, 60, 160, 50);
	f_rows.setup(200.f, 50.f, 100.f, 25.f, &t_fieldRows, true, t, 0, 60, 0, 60, 160, 50);
	f_columns.setup(200.f, 50.f, 100.f, 25.f, &t_fieldColumns, true, t, 0, 180, 0, 60, 160, 50);

	Util::setupTextElement(&t_playerName, font, "", 0, -100, true);
	Util::setupTextElement(&t_playerCount, font, "1", 0, -240, true);
	Util::setupTextElement(&t_fieldRows, font, "2", 0, 40, true);
	Util::setupTextElement(&t_fieldColumns, font, "2", 0, 160, true);
	Util::setupTextElement(&t_playerCountInstruction, font, "Enter the number of players", 0, -280);
	Util::setupTextElement(&t_playerNameInstruction, font, "Enter name for player 1", 0, -140);
	Util::setupTextElement(&t_playerRowsInstruction, font, "Enter number of rows", 0, 0);
	Util::setupTextElement(&t_playerColumnsInstruction, font, "Enter number of columns", 0, 120);
	Util::setupTextElement(&t_play, font, "PLAY", 0, 293);
}


void MainMenu::updateText(std::string text) {
	if (selectedField == nullptr) return;

	sf::Text* selectedTextBox = selectedField->linkedTextField;
	// Delete last character if Backspace pressed
	if (text == "\b") {
		selectedTextBox->setString(selectedTextBox->getString().substring(0, selectedTextBox->getString().getSize() - 1));
	}
	// If any "number-only" fields are selected and the character
	// entered is not a number - do nothing
	else if ((selectedTextBox == &t_playerCount ||
		selectedTextBox == &t_fieldRows ||
		selectedTextBox == &t_fieldColumns) && !isdigit(text[0])) {
		return;
	}
	if (isalnum(text[0])) {
		if (selectedTextBox->getString().getSize() < 10) {
			selectedTextBox->setString(Util::removeLeadingZeros(selectedTextBox->getString() + text));
		}
	}
	// Calculate new position of the text so it remains centered
	sf::FloatRect bounds = selectedTextBox->getGlobalBounds();
	selectedTextBox->setPosition(800 / 2 - bounds.width / 2, selectedTextBox->getPosition().y);

	if (selectedTextBox == &t_playerName) {
		(*playerNames)[currentPlayername] = t_playerName.getString().toAnsiString();
	}
	else if (selectedTextBox == &t_playerCount) {
		playerCount = Util::textToInt(&t_playerCount);
		if (playerCount == -1) {
			playerCount = 1;
		}
		else if (playerCount == -2 || playerCount > GameManager::MAX_PLAYERS) {
			playerCount = GameManager::MAX_PLAYERS;
			t_playerCount.setString(std::to_string(GameManager::MAX_PLAYERS));
		}
	}
	else {
		if (Util::textToInt(selectedTextBox) > GameManager::MAX_ROWS_COLUMNS) {
			selectedTextBox->setString(std::to_string(GameManager::MAX_ROWS_COLUMNS));
		}
	}
}

void MainMenu::mouseClicked() {
	int mouseX = window->mapPixelToCoords(sf::Mouse::getPosition(*window)).x;
	int mouseY = window->mapPixelToCoords(sf::Mouse::getPosition(*window)).y;

	sf::RectangleShape* clickedButtonOrField = nullptr;

	for (const auto& button : buttons) {
		if (button->isClicked(mouseX, mouseY)) {
			clickedButtonOrField = button;
			break;
		}
	}

	if (clickedButtonOrField == nullptr) {
		return;
	}
	else if (clickedButtonOrField == &playButton) {
		readyToStart = true;
	}
	else if (clickedButtonOrField == &returnToGameButton) {
		backToGame = true;
	}
	else if (clickedButtonOrField == &arrowLeft) {
		cyclePlayerNames(-1);
	}
	else if (clickedButtonOrField == &arrowRight) {
		cyclePlayerNames(1);
	}
	else if (clickedButtonOrField == &f_playerCount) {
		changeSelection(&f_playerCount);
	}
	else if (clickedButtonOrField == &f_playerName) {
		changeSelection(&f_playerName);
	}
	else if (clickedButtonOrField == &f_rows) {
		changeSelection(&f_rows);
	}
	else if (clickedButtonOrField == &f_columns) {
		changeSelection(&f_columns);
	}
}

void MainMenu::changeSelection(Field* newSelection) {
	if (selectedField != nullptr) {
		selectedField->setTextureRect(sf::IntRect(0, 60, 160, 50));
	}
	// Secure minimal player count and row/column count
	if (selectedField == &f_playerCount) {
		if (Util::textToInt(f_playerCount.linkedTextField) < 1) {
			f_playerCount.linkedTextField->setString("1");
			playerCount = 1;
		}
	}
	else if (selectedField == &f_rows || selectedField == &f_columns) {
		if (Util::textToInt(selectedField->linkedTextField) < 2) {
			selectedField->linkedTextField->setString("2");
		}
	}

	selectedField = newSelection;
	selectedField->setTextureRect(sf::IntRect(0, 110, 160, 50));
}

void MainMenu::cyclePlayerNames(int direction) {
	if (currentPlayername == 0 && direction == -1) {
		currentPlayername = 0;
		return;
	}
	if (currentPlayername == (playerCount - 1) && direction == 1) {
		return;
	}

	if (currentPlayername >= playerCount) {
		currentPlayername = playerCount - 1;
		t_playerName.setString((*playerNames)[currentPlayername]);
	}
	else {
		currentPlayername += direction;
		// Add new names to the vector only when the user tries to 
		// enter the name for that specific player, in order to save
		// on vector resizing. If a name remains unfilled, a generic
		// "placeholder" will be used anyway.
		if ((*playerNames).size() <= currentPlayername) {
			(*playerNames).push_back("");
		}
		t_playerName.setString((*playerNames)[currentPlayername]);
	}
	t_playerNameInstruction.setString("Enter name for player " + std::to_string(currentPlayername + 1));

	std::cout << "Current player : " << currentPlayername << std::endl;
}

void MainMenu::passGameParameters(int* rows, int* columns, int* playerCount)
{
	*rows = std::stoi(t_fieldRows.getString().toAnsiString());
	*columns = std::stoi(t_fieldColumns.getString().toAnsiString());
	*playerCount = this->playerCount;
}

void MainMenu::drawUI()
{
	for (const auto& element : buttons) {
		window->draw(*element);
	}
	window->draw(t_playerName);
	window->draw(t_playerCount);
	window->draw(t_fieldRows);
	window->draw(t_fieldColumns);
	window->draw(t_playerRowsInstruction);
	window->draw(t_playerColumnsInstruction);
	window->draw(t_playerNameInstruction);
	window->draw(t_playerCountInstruction);
	window->draw(t_play);
}
