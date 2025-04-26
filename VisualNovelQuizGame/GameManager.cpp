// GameManager.cpp
#include "GameManager.h"

GameManager::GameManager(sf::RenderWindow& win)
    : window(win) {
}

void GameManager::run() {
    IntroManager intro(window);
    intro.run();
}
