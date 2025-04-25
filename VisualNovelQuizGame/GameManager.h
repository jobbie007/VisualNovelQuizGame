#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "IntroManager.h"
#include "StageOneManager.h"

class GameManager {
public:
    GameManager(sf::RenderWindow& window);
    ~GameManager();

    void run();

private:
    sf::RenderWindow& window_;
    std::unique_ptr<IntroManager> intro_manager_;
    std::unique_ptr<StageOneManager> stage_one_manager_;
}; 