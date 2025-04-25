#include "GameManager.h"
#include <SFML/Graphics.hpp>

GameManager::GameManager(sf::RenderWindow& window)
    : window_(window) {
    intro_manager_ = std::make_unique<IntroManager>(window_);
    stage_one_manager_ = std::make_unique<StageOneManager>(window_);
}

GameManager::~GameManager() {
    // Cleanup if needed
}

void GameManager::run() {
    // Run the stages in sequence
    intro_manager_->run();
    stage_one_manager_->run();
}
