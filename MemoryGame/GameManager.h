#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <set>
#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Card.h"
#include "Player.h"
#include "MainMenu.h"

#include "Sprite3d.h"

class GameManager
{
public:
	GameManager(sf::RenderWindow* _window);

	void startGame();
	void update(float deltaTime);
	
	void returnToMenu();

	void scrollView(int x, int y);

	void toggleScoreboard();
	void textEnteredHandler(sf::String t);
	void mouseClickHandler();
	void resizeHandler();

	enum class GameState {
		Menu,
		Game,
		ShowingCard,
		LookingAtCards,
		HidingCards,
		MovingPlayerNameOut,
		BringingNewPlayerIn,
		DarkenPlayerName,
		StartingNextPlayersTurn,
		GameOver
	};
	GameState gameState;

	static const int MAX_PLAYERS = 1000;
	static const int MAX_ROWS_COLUMNS = 90;

	static int windowWidth;
	static int windowHeight;

private:

	void setupCard(Card* card, int row, int column, int offsetX, int offsetY, int colorNumber);
	void setupView(sf::View* view, float x, float y, float width, float height);
	void loadSound(sf::SoundBuffer* buffer, sf::Sound* sound, std::string path);

	bool isMatch();

	void calculateNewDrawArea();
	void rotateBoard();
	void repositionViewNamesElements();

	// Animation methods 
	bool rotateCard(Card* card);
	bool rotateCards(Card* cardA, Card* CardB);
	bool movePlayerNameOut();
	bool darkenPlayerName(float elapsedTime = -1);
	bool bringNewPlayerIn();
	bool startNewPlayersTurn();

	bool waitForScoreboard();

	const int cardDimension		= 75;
	const int spaceBetweenCards = 25;
	const int minimalWindowSize = 600;

	sf::RenderWindow* window;

	sf::Texture texture;
	sf::Texture cardTexture;
	sf::Font font;
	sf::Font scoreFont;

	sf::View viewNames;
	sf::View viewCards;
	sf::View viewMenu;

	Button menuButton;
	Button scoreButton;
	Button toggleRotationButton;

	MainMenu mainMenu;

	sf::Music		backgroundMusic;
	sf::SoundBuffer bufferCardTurn;
	sf::SoundBuffer bufferPairHit;
	sf::SoundBuffer bufferPairMiss;
	sf::SoundBuffer bufferGameOver;
	sf::SoundBuffer bufferNextPlayer;
	sf::Sound		soundCardTurn;
	sf::Sound		soundPairHit;
	sf::Sound		soundPairMiss;
	sf::Sound		soundGameOver;
	sf::Sound		soundNextPlayer;

	sf::Text columnCoordinate;
	sf::Text rowCoordinate;

	sf::Text playerNameTop;
	sf::Text playerNameRight;
	sf::Text playerNameBottom;
	sf::Text playerNameLeft;
	const std::vector<sf::Text*> playerNamesTexts = {
		&playerNameTop,
		&playerNameRight,
		&playerNameBottom,
		&playerNameLeft
	};
	float initialPlayerNamesPositions[4][2] = {
		{0,-370},
		{370,0},
		{0, 350},
		{-360, 0}
	};

	Scoreboard scoreboard;
	bool showingScoreboard	= false;
	bool scoreUpToDate		= false;

	bool gameInProgress		= false;
	bool rotationOn			= false;

	std::vector<Player> players;
	std::vector<std::string> playerNames = { "" };
	std::vector<std::vector<Card>> cards;

	int playerCount;
	int currentPlayer;
	int cardsRemaining;

	const std::vector<sf::Color> backgroundColors = {
		sf::Color::White,
		sf::Color::Red,
		sf::Color::Blue,
		sf::Color::Green,
		sf::Color(255, 255, 0),
		sf::Color(255, 0, 255),
		sf::Color(0, 255, 255),
		sf::Color(128, 255, 0),
		sf::Color(255, 128, 0),
		sf::Color(0, 255, 128),
		sf::Color(0, 128, 255),
		sf::Color(255, 0, 128),
		sf::Color(128, 0, 255),
		sf::Color(128, 128, 0),
		sf::Color(128, 0, 128),
		sf::Color(0, 128, 128),
		sf::Color(128,128,128)
	};

	int rows;
	int columns;

	// Animation variables
	float deltaTime;
	float elapsedTime = 0.f;
	const float flipDuration = .5f;
	const float showingCardsDuration = 1.5f;
	const float movingPlayerNameDuration = .75f;
	const float changePlayerNameColorDuration = 0.3f;
	const float zoomingNextPlayerNameDuration = 0.5f;
	const float showScoreboardAfterDuration = 1.7f;
	bool showingFace = true;
	Card* rotatingCard;

	Card* firstSelectedCard;
	Card* secondSelectedCard;

	int startX, startY, endX, endY;

	bool lastCardMissing;
};

