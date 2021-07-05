#include <iostream>
#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>

#include "GameManager.h"

int main() {

	sf::RenderWindow window;
	sf::Event event;

	GameManager MemoryGame(&window);

	sf::Clock clock = sf::Clock::Clock();
	sf::Time deltaTime;

	while (window.isOpen()) {
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
			if (event.type == sf::Event::KeyPressed) {	
				if (event.key.code == sf::Keyboard::Escape) {
					window.close();
				}
				if (event.key.code == sf::Keyboard::Enter) {
					MemoryGame.startGame();
				}
				if (event.key.code == sf::Keyboard::Up) {
					MemoryGame.scrollView(0, 25);
				}
				if (event.key.code == sf::Keyboard::Down) {
					MemoryGame.scrollView(0, -25);
				}
				if (event.key.code == sf::Keyboard::Left) {
					MemoryGame.scrollView(-25, 0);
				}
				if (event.key.code == sf::Keyboard::Right) {
					MemoryGame.scrollView(25, 0);
				}
				if (event.key.code == sf::Keyboard::S) {
					MemoryGame.toggleScoreboard();
				}
				if (event.key.code == sf::Keyboard::Space) {
					MemoryGame.returnToMenu();
				}
			}
			else if (event.type == sf::Event::MouseButtonPressed) {
				MemoryGame.mouseClickHandler();
			}
			else if (event.type == sf::Event::Resized) {
				MemoryGame.resizeHandler();
			}
			else if (event.type == sf::Event::TextEntered) {
				MemoryGame.textEnteredHandler(event.text.unicode);
			}
		}


		window.clear();
		MemoryGame.update(clock.restart().asSeconds());
		window.display();
	}

	return 0;
}