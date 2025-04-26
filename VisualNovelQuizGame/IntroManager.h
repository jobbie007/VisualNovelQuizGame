// IntroManager.hpp
#pragma once

#include <SFML/Graphics.hpp>

class IntroManager {
public:
    explicit IntroManager(sf::RenderWindow& win);
    void run();

private:
    sf::RenderWindow& window;
};
