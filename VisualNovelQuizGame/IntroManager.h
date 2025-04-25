#pragma once
#include <SFML/Graphics.hpp>
#include "JSONLoader.h"

class IntroManager {
public:
    IntroManager(sf::RenderWindow& window);
    void run();

private:
    sf::RenderWindow& window_;
    std::unique_ptr<JSONLoader> json_loader_;
}; 