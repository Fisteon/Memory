#include <random>
#include <array>
#include <numeric>
#include <cmath>
#include <algorithm>

#include "GameManager.h"
#include "Util.h"

int GameManager::windowWidth = 800;
int GameManager::windowHeight = 800;

GameManager::GameManager(sf::RenderWindow* _window) :
	mainMenu(_window, &playerNames),
	cardsRemaining(0),
	columns(2),
	rows(2),
	currentPlayer(0),
	gameState(GameState::Menu),
	playerCount(0),
	startX(0),
	startY(0),
	endX(800),
	endY(800)
{
	window = _window;
	window->create(sf::VideoMode(windowWidth, windowHeight), "Memory");
	
	if (!texture.loadFromFile("Resources/textures.png") ||
		!cardTexture.loadFromFile("Resources/memorycards.png")) {
		std::cout << "Error loading textures";
		window->close();
		return;
	}

	if (!font.loadFromFile("Resources/Candara.ttf") ||
		!scoreFont.loadFromFile("Resources/cour.ttf")) {
		std::cout << "Error loading fonts";
		window->close();
		return;
	}

	backgroundMusic.openFromFile("Resources/BackgroundMusic.wav");
	loadSound(&bufferCardTurn,		&soundCardTurn,		"Resources/CardTurn.wav");
	loadSound(&bufferPairHit,		&soundPairHit,		"Resources/PairHit.wav");
	loadSound(&bufferPairMiss,		&soundPairMiss,		"Resources/PairMiss.wav");
	loadSound(&bufferGameOver,		&soundGameOver,		"Resources/GameOver.wav");
	loadSound(&bufferNextPlayer,	&soundNextPlayer,	"Resources/NextPlayer.wav");
	backgroundMusic.setVolume(50);
	backgroundMusic.setLoop(true);
	backgroundMusic.play();

	mainMenu.initialSetup(&texture, &font);
	scoreboard.initialSetup(&scoreFont);

	menuButton.setup(40.f, 40.f, 20.f, 20.f, &texture, -windowWidth / 2 + 35.f, windowHeight / 2 - 27.5f, 0, 160, 75, 75);
	scoreButton.setup(40.f, 40.f, 20.f, 20.f, &texture, -windowWidth / 2 + 90.f, windowHeight / 2 - 27.5f, 75, 160, 75, 75);
	toggleRotationButton.setup(40.f, 40.f, 20.f, 20.f, &texture, -windowWidth / 2 + 145.f, windowHeight / 2 - 27.5f, 300, 160, 75, 75);


	setupView(&viewNames, 0, 0, 1, 1);
	setupView(&viewCards, 0.1f, 0.1f, 0.8f, 0.8f);
	viewMenu = window->getDefaultView();
	window->setView(viewMenu);
	
	gameState = GameState::Menu;
}


void GameManager::startGame() {
	if (gameState != GameState::Menu) return;
	gameState = GameState::Game;

	// Process data enetered in the menu
	mainMenu.passGameParameters(&rows, &columns, &playerCount);
	players.clear();
	players.resize(playerCount);

	// If there are players without a name entered use a generic one
	for (int i = 0; i < playerCount; i++) {
		if (playerNames.size() <= i || playerNames[i] == "") {
			players[i].name = "Player " + std::to_string(i + 1);
		}
		else {
			players[i].name = playerNames[i];
		}
	}
	playerNames.resize(playerCount);
	playerNames.shrink_to_fit();

	// Check if bottom right card should be used
	lastCardMissing = rows * columns % 2 == 1 ? true : false;
	cardsRemaining = lastCardMissing ? rows * columns - 1 : rows * columns;

	cards.clear();
	cards.resize(rows, std::vector<Card>(columns));
	if (lastCardMissing) {
		cards[rows - 1].pop_back();
	}

	// Create pairs
	// Shuffle a vector<int> filled with sequential numbers from 0 .. number of cards.
	// Use that random sequence to set up card positions and their picture.
	// There are 256 unique images, so that's enough for 512 cards (2 cards must have the
	// same image). After 512 cards have been used, background color will change, and the
	// images will repeat. So, the game is played by finding an image pair with the same
	// symbol and background color.
	std::vector<int> shuffledOrder(cardsRemaining);
	std::random_device rd;

	std::iota(std::begin(shuffledOrder), std::end(shuffledOrder), 0);
	std::shuffle(shuffledOrder.begin(), shuffledOrder.end(), rd);
	
	for (int i = 0; i < cardsRemaining; i += 2) {
		int rowFirst, rowSecond, columnFirst, columnSecond, offsetX, offsetY, colorNumber;
		// Get 2d coordinates from a single value
		rowFirst = (shuffledOrder[i] / columns) % rows;
		columnFirst = shuffledOrder[i] % columns;

		rowSecond = (shuffledOrder[i+1] / columns) % rows;
		columnSecond = shuffledOrder[i+1] % columns;

		colorNumber = shuffledOrder[i] / 512;

		// Card texture is a 16x16 grid
		offsetX = (shuffledOrder[i] / 16) % 16;
		offsetY = shuffledOrder[i] % 16;

		cards[rowFirst][columnFirst].patternNumber = i;
		setupCard(&cards[rowFirst][columnFirst], rowFirst, columnFirst, offsetX, offsetY, colorNumber);
		cards[rowSecond][columnSecond].patternNumber = i;
		setupCard(&cards[rowSecond][columnSecond], rowSecond, columnSecond, offsetX, offsetY, colorNumber);
	}

	calculateNewDrawArea();

	// Set up to 4 players' names on the board.
	// Left and right one are rotated 90 degrees
	currentPlayer = 0;
	for (int i = 0; i < 4; i++) {
		playerNamesTexts[i]->setRotation(0);
		Util::setupTextElement(
			playerNamesTexts[i],
			&font, 
			i < playerCount ? players[i].name : "",
			initialPlayerNamesPositions[i][0], 
			initialPlayerNamesPositions[i][1]
		);
		std::cout << playerNamesTexts[i]->getOrigin().x << "\t" << playerNamesTexts[i]->getOrigin().y << "\n";
		if (i < playerCount) players[i].position = i;
	}
	playerNameTop.setFillColor(sf::Color::White);

	playerNameLeft.setRotation(-90);
	playerNameRight.setRotation(90);
	viewCards.setRotation(0);

	elapsedTime = 0;
	firstSelectedCard = nullptr;
	secondSelectedCard = nullptr;
	scoreUpToDate = false;
	gameInProgress = true;
}

void GameManager::update(float dt) {
	// Menu is separate from everything else, so if the game is 
	// currently in the menu, that's the only relevant part of update
	if (gameState == GameState::Menu) {
		mainMenu.drawUI();
		// Back to game button is clicked while there's an active
		// game running "in the background"
		if (mainMenu.backToGame && gameInProgress) {
			mainMenu.backToGame = false;
			gameState = GameState::Game;
		}
		if (mainMenu.readyToStart) {
			mainMenu.readyToStart = false;
			startGame();
		}
		return;
	}

	deltaTime = dt;

	switch (gameState) {
	case (GameState::ShowingCard):
		if (!rotateCard(rotatingCard)) break;
		if (secondSelectedCard == nullptr) {
			gameState = GameState::Game;
		}
		else {
			if (isMatch()) {
				soundPairHit.play();
				gameState = GameState::Game;
				players[currentPlayer].score++;
				cardsRemaining -= 2;
				if (cardsRemaining == 0) {
					gameState = GameState::GameOver;
					soundGameOver.play();
					elapsedTime = 0;
					gameInProgress = false;
				}
				scoreUpToDate = false;
			}
			else {
				showingFace = false;
				gameState = GameState::LookingAtCards;
			}
		}
		break;

	case (GameState::LookingAtCards):
		elapsedTime += deltaTime;
		if (elapsedTime < showingCardsDuration) break;
		elapsedTime = 0;
		gameState = GameState::HidingCards;
		soundPairMiss.play();
		break;

	case (GameState::HidingCards):
		if (!rotateCards(firstSelectedCard, secondSelectedCard)) break;
		gameState = GameState::Game;
		firstSelectedCard = nullptr;
		secondSelectedCard = nullptr;

		// Depending on the number of players, player transition differs
		if (playerCount == 1) {
			gameState = GameState::Game;
		}
		else if (playerCount > 4) {
			gameState = GameState::MovingPlayerNameOut;
		}
		else {
			gameState = GameState::DarkenPlayerName;
		}
		break;

	case (GameState::MovingPlayerNameOut):
		if (!movePlayerNameOut()) break;
		gameState = GameState::BringingNewPlayerIn;
		break;

	case (GameState::BringingNewPlayerIn):
		if (!bringNewPlayerIn()) break;
		gameState = GameState::StartingNextPlayersTurn;
		players[(currentPlayer + 4) % playerCount].position = players[currentPlayer].position;
		currentPlayer = (currentPlayer + 1) % playerCount;
		rotateBoard();
		break;

	case (GameState::DarkenPlayerName):
		if (!darkenPlayerName()) break;
		elapsedTime = 0;
		currentPlayer = (currentPlayer + 1) % playerCount;
		rotateBoard();
		gameState = GameState::StartingNextPlayersTurn;
		break;

	case (GameState::StartingNextPlayersTurn):
		if (!startNewPlayersTurn()) break;
		soundNextPlayer.play();
		gameState = GameState::Game;
		break;

	case (GameState::GameOver):
		if (!waitForScoreboard()) break;
		gameState = GameState::Game;
		break;
	}

	// Draw everything on the viewNames first
	window->setView(viewNames);
	window->draw(menuButton);
	window->draw(scoreButton);
	window->draw(toggleRotationButton);
	window->draw(playerNameTop);
	window->draw(playerNameBottom);
	window->draw(playerNameLeft);
	window->draw(playerNameRight);

	window->setView(viewCards);

	// Draw only cards that are visible in the current viewport
	for (int i = startY; i < endY; i++) {
		for (int j = startX; j < endX; j++) {
			if (lastCardMissing && (i == rows - 1 && j == columns - 1)) continue;
			window->draw(cards[i][j].sprite3d);
		}
	}

	// If shown, scoreboard goes on top of everything, so it is drawn last
	if (showingScoreboard) {
		window->setView(viewNames);
		window->draw(scoreboard.frame);
		window->draw(scoreboard.scores);
	}
}

void GameManager::setupCard(Card* card, int row, int column, int offsetX, int offsetY, int colorNumber) {
	card->color = backgroundColors[colorNumber];

	card->sprite3d.setOrigin(sf::Vector2f(57.5f, 0));
	card->sprite3d.setPosition((
		cardDimension + spaceBetweenCards) * column + card->sprite3d.getOrigin().x,
		(cardDimension + spaceBetweenCards) * row + card->sprite3d.getOrigin().y);
	card->sprite3d.setTexture(texture);
	card->sprite3d.setTextureRect(sf::IntRect(160, 0, 115, 125));
	card->sprite3d.setBackTexture(cardTexture);
	card->sprite3d.setBackTextureOffset(sf::Vector2i(offsetX * 115, offsetY * 125));
	card->sprite3d.setScale(cardDimension / 115.f, cardDimension / 125.f);
}

void GameManager::setupView(sf::View* view, float x, float y, float width, float height) {
	view->setViewport(sf::FloatRect(x, y, width, height));
	view->setCenter(windowWidth / 2, windowHeight / 2);
	view->setSize(windowWidth, windowHeight);
}

void GameManager::loadSound(sf::SoundBuffer* buffer, sf::Sound* sound, std::string path) {
	if (buffer->loadFromFile(path)) {
		sound->setBuffer(*buffer);
	}
}

bool GameManager::isMatch() {
	if (firstSelectedCard->color == secondSelectedCard->color &&
		firstSelectedCard->patternNumber == secondSelectedCard->patternNumber) {
		firstSelectedCard->guessed = true;
		firstSelectedCard = nullptr;
		secondSelectedCard->guessed = true;
		secondSelectedCard = nullptr;
		return true;
	}
	return false;
}

void GameManager::returnToMenu() {
	if (gameState != GameState::Game) return;
	gameState = GameState::Menu;
	showingScoreboard = false;
	resizeHandler();
	window->setView(viewMenu);
}

// If there are more cards than it fits in the current viewport
// we can "scroll" it. Since the cards are being drawn based on
// viewport center and size, it's important it never moves out 
// of bounds of the vector that's holding the cards
void GameManager::scrollView(int x, int y) {
	if (windowWidth / 100 > columns && x != 0) return;
	if (windowHeight / 100 > rows && y != 0) return;
	int startX = viewCards.getCenter().x - viewCards.getSize().x / 2;
	int startY = viewCards.getCenter().y - viewCards.getSize().y / 2;
	int endX = viewCards.getCenter().x + viewCards.getSize().x / 2;
	int endY = viewCards.getCenter().y + viewCards.getSize().y / 2;

	if ((endX + x) / 100 >= columns) x = std::max(columns * 100 - endX - 1, 0);
	if ((endY + y) / 100 >= rows) y = std::max(rows * 100 - endY - 1, 0);
	if ((startX + x) % 100 < 0) x = -startX;
	if ((startY + y) % 100 < 0) y = -startY;

	// If it turns out move would be "illegal", don't do anything
	if (x == 0 && y == 0) return;
	viewCards.move(x, y);
	calculateNewDrawArea();
}

void GameManager::calculateNewDrawArea() {
	// ViewPortSize/Center
	sf::Vector2f vps = viewCards.getSize();
	sf::Vector2f vpc = viewCards.getCenter();

	startX = (vpc.x - vps.x / 2) / 100;
	startX = startX < 0 ? 0 : startX;
	startY = (vpc.y - vps.y / 2) / 100;
	startY = startY < 0 ? 0 : startY;

	endX = (vpc.x + vps.x / 2) / 100 + 1;
	endX = endX >= columns ? columns : endX;
	endY = (vpc.y + vps.y / 2) / 100 + 1;
	endY = endY >= rows ? rows : endY;
}

// All animations are done using "lerp" function. By giving it a 
// starting and an ending value, combined with a ratio of time spent
// in animation and total animation duration, a smooth transition
// (animation) can be achieved
bool GameManager::rotateCard(Card* card) {
	elapsedTime += deltaTime;
	float newRotation = Util::lerp(showingFace ? 0.f : 180.f, showingFace ? 180.f : 0.f, elapsedTime / flipDuration);
	if (newRotation >= 90) {
		if (card->sprite3d.getColor() != card->color) {
			card->sprite3d.setColor(card->color);
		}
	}
	else if (newRotation <= 90) {
		if (card->sprite3d.getColor() != sf::Color::White) {
			card->sprite3d.setColor(sf::Color::White);
		}
	}
	if (elapsedTime < flipDuration) {
		card->sprite3d.setYaw(newRotation);
		return false;
	}
	else {
		card->sprite3d.setYaw(showingFace ? 180.f : 0.f);
		elapsedTime = 0;
		return true;
	}
}

bool GameManager::rotateCards(Card* cardA, Card* cardB) {
	elapsedTime += deltaTime;
	float newRotation = Util::lerp(180.f, 0.f, elapsedTime / flipDuration);
	if (newRotation <= 90) {
		if (cardA->sprite3d.getColor() != sf::Color::White) {
			cardA->sprite3d.setColor(sf::Color::White);
		}
		if (cardB->sprite3d.getColor() != sf::Color::White) {
			cardB->sprite3d.setColor(sf::Color::White);
		}
	}
	if (elapsedTime < flipDuration) {
		cardA->sprite3d.setYaw(newRotation);
		cardB->sprite3d.setYaw(newRotation);
		return false;
	}
	else {
		cardA->sprite3d.setYaw(0.f);
		cardB->sprite3d.setYaw(0.f);
		elapsedTime = 0;
		return true;
	}

}

bool GameManager::movePlayerNameOut() {
	elapsedTime += deltaTime;
	// CurrentPlayerPosition
	int cpp = players[currentPlayer].position;
	sf::Text* playerNameText = playerNamesTexts[cpp];
	if (elapsedTime < movingPlayerNameDuration) {
		float newOffset = Util::lerp(
			initialPlayerNamesPositions[cpp][cpp % 2 == 1 ? 0 : 1],
			initialPlayerNamesPositions[cpp][cpp % 2 == 1 ? 0 : 1] + 150 * (cpp % 3 == 0 ? -1 : 1),
			elapsedTime / movingPlayerNameDuration);
		playerNameText->setPosition(
			windowWidth / 2 + (cpp % 2 == 0 ? 0 : newOffset),
			windowHeight / 2 + (cpp % 2 == 0 ? newOffset : 0));
		darkenPlayerName(elapsedTime);
	}

	if (elapsedTime >= movingPlayerNameDuration) {
		playerNameText->setString(players[(currentPlayer + 4) % playerCount].name);
		sf::FloatRect bounds = playerNameText->getGlobalBounds();
		playerNameText->setOrigin(
			(cpp % 2 == 1 ? bounds.height / 2 : bounds.width / 2),
			(cpp % 2 == 1 ? bounds.width / 2 : bounds.height / 2));
		playerNameText->setPosition(
			windowWidth / 2 + initialPlayerNamesPositions[cpp][0],
			windowHeight / 2 + initialPlayerNamesPositions[cpp][1]);
		playerNameText->setCharacterSize(0);
		playerNameText->setFillColor(sf::Color::Black);
		elapsedTime = 0;
		return true;
	}
	return false;
}

bool GameManager::darkenPlayerName(float et) {
	sf::Text* playerNameText = playerNamesTexts[players[currentPlayer].position];
	if (et < 0) elapsedTime += deltaTime;
	else elapsedTime = et;
	float newColor = Util::lerp(255, 0, elapsedTime / changePlayerNameColorDuration);
	playerNameText->setFillColor(sf::Color(newColor, newColor, newColor));
	if (elapsedTime > changePlayerNameColorDuration) {
		playerNameText->setFillColor(sf::Color::Black);
		return true;
	}
	return false;
}

bool GameManager::bringNewPlayerIn() {
	elapsedTime += deltaTime;
	sf::Text* playerNameText = playerNamesTexts[players[currentPlayer].position];
	if (elapsedTime < zoomingNextPlayerNameDuration) {
		playerNameText->setCharacterSize(Util::lerp(0, 30, elapsedTime / zoomingNextPlayerNameDuration));
	}
	if (elapsedTime >= zoomingNextPlayerNameDuration) {
		elapsedTime = 0;
		return true;
	}
	return false;
}

bool GameManager::startNewPlayersTurn() {
	elapsedTime += deltaTime;
	sf::Text* playerNameText = playerNamesTexts[players[currentPlayer].position];
	float newColor = Util::lerp(0, 255, elapsedTime / changePlayerNameColorDuration);
	playerNameText->setFillColor(sf::Color(newColor, newColor, newColor));
	if (elapsedTime > changePlayerNameColorDuration) {
		elapsedTime = 0;
		return true;
	}
	return false;
}

bool GameManager::waitForScoreboard() {
	elapsedTime += deltaTime;
	if (soundGameOver.getStatus() == sf::Sound::Status::Playing) {
		if (elapsedTime >= showScoreboardAfterDuration) {
			toggleScoreboard();
			return true;
		}
	}
	return false;
}


void GameManager::toggleScoreboard() {
	if (gameState == GameState::Menu) return;
	
	// Instead of calculating the scoreboard every time it's shown
	// we only update it when there's a score change
	if (!scoreUpToDate) {
		std::unique_ptr<std::vector<Player>> highscore = std::make_unique<std::vector<Player>>(std::min(10, (int)players.size()));
		std::partial_sort_copy(players.begin(), players.end(), (*highscore).begin(), (*highscore).end(), std::greater<Player>());

		int lastScore = -1;
		std::string scoreList = "";
		for (int i = 0; i < (*highscore).size(); i++) {
			if ((*highscore)[i].score != lastScore) {
				scoreList += std::to_string(i + 1) + ".  ";
				lastScore = (*highscore)[i].score;
			}
			else {
				scoreList += "    ";
			}
			scoreList += (*highscore)[i].name +
				std::string(10 - (*highscore)[i].name.length(), ' ') +
				"   " +
				std::to_string((*highscore)[i].score) +
				"\n";
		}

		scoreboard.scores.setString(scoreList);
		scoreUpToDate = true;
	}

	showingScoreboard = !showingScoreboard;
}

void GameManager::textEnteredHandler(sf::String t) {
	if (gameState == GameState::Menu)
		mainMenu.updateText(t.toAnsiString());
}

void GameManager::mouseClickHandler()
{
	if (gameState == GameState::Menu) {
		mainMenu.mouseClicked();
		return;
	}
	if (gameState == GameState::Game) {
		// In order to get accurate click position, both views need to be checked
		// Checking for UI button clicks
		window->setView(viewNames);
		int mouseButtonX = window->mapPixelToCoords(sf::Mouse::getPosition(*window)).x;
		int mouseButtonY = window->mapPixelToCoords(sf::Mouse::getPosition(*window)).y;

		if (menuButton.isClicked(mouseButtonX, mouseButtonY)) {
			returnToMenu();
			return;
		}
		else if (scoreButton.isClicked(mouseButtonX, mouseButtonY)) {
			toggleScoreboard();
			return;
		}
		else if (toggleRotationButton.isClicked(mouseButtonX, mouseButtonY)) {
			rotationOn = !rotationOn;
			toggleRotationButton.setTextureRect(sf::IntRect(225 + (rotationOn ? 0 : 75), 160, 75, 75));
			return;
		}

		// Checking for memory card clicks
		window->setView(viewCards);
		int mouseX = window->mapPixelToCoords(sf::Mouse::getPosition(*window)).x;
		int mouseY = window->mapPixelToCoords(sf::Mouse::getPosition(*window)).y;

		int cardRow = -1;
		int cardColumn = -1;

		if (mouseX > (viewCards.getCenter().x - viewCards.getSize().x / 2) &&
			mouseY > (viewCards.getCenter().y - viewCards.getSize().y / 2) &&
			mouseX < (viewCards.getCenter().x + viewCards.getSize().x / 2) && 
			mouseY < (viewCards.getCenter().y + viewCards.getSize().y / 2)) {
			cardRow = mouseY / (cardDimension + spaceBetweenCards);
			cardColumn = mouseX / (cardDimension + spaceBetweenCards);

			if (cardRow < rows && cardColumn < columns && cards[cardRow].size() > cardColumn) {
				if (cards[cardRow][cardColumn].guessed) return;
				if (firstSelectedCard == nullptr) {
					firstSelectedCard = &cards[cardRow][cardColumn];
					gameState = GameState::ShowingCard;
					rotatingCard = firstSelectedCard;
					soundCardTurn.play();
					showingFace = true;
				}
				else if (&cards[cardRow][cardColumn] != firstSelectedCard)
				{
					gameState = GameState::ShowingCard;
					showingFace = true;
					secondSelectedCard = &cards[cardRow][cardColumn];
					rotatingCard = secondSelectedCard;
					soundCardTurn.play();
				}
			}
		}
	}
}

void GameManager::resizeHandler()
{
	int newX = window->getSize().x;
	int newY = window->getSize().y;

	int min = std::min(newX, newY);
	window->setSize(sf::Vector2u(
		min > minimalWindowSize ? min : minimalWindowSize,
		min > minimalWindowSize ? min : minimalWindowSize));

	for (auto& x : initialPlayerNamesPositions) {
		x[0] *= window->getSize().x / (float)windowWidth;
		x[1] *= window->getSize().y / (float)windowHeight;
	}

	windowWidth = window->getSize().x;
	windowHeight = window->getSize().y;

	setupView(&viewCards, 0.1f, 0.1f, 0.8f, 0.8f);
	setupView(&viewNames, 0, 0, 1, 1);

	repositionViewNamesElements();
	calculateNewDrawArea();
	if (gameState != GameState::Menu)
		window->setView(viewNames);
}

void GameManager::rotateBoard()
{
	if (rotationOn)
		viewCards.setRotation(players[currentPlayer].position * 90);
}

void GameManager::repositionViewNamesElements() {
	int i = 0;
	sf::View tempView = window->getView();
	window->setView(viewNames);
	for (auto& x : playerNamesTexts) {
		x->setPosition(
			windowWidth / 2 + initialPlayerNamesPositions[i][0],
			windowHeight / 2 + initialPlayerNamesPositions[i][1]
		);
		i++;
	}
	scoreboard.frame.setPosition(windowWidth / 2, windowHeight / 2);
	scoreboard.scores.setPosition(scoreboard.frame.getGlobalBounds().left + 30, scoreboard.frame.getGlobalBounds().top + 50);
	menuButton.setPosition(35.f, windowHeight - 27.5f);
	scoreButton.setPosition(90.f, windowHeight - 27.5f);
	toggleRotationButton.setPosition(145.f, windowHeight - 27.5f);
	window->setView(tempView);
}

