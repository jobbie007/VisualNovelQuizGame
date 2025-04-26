// GameManager.hpp
#pragma once

#include <SFML/Graphics.hpp>
#include "IntroManager.h"

class GameManager {
public:
    explicit GameManager(sf::RenderWindow& win);
    void run();

private:
    sf::RenderWindow& window;
};
