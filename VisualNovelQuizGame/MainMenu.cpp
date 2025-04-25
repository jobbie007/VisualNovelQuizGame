#include "GameManager.cpp"  // Correctly include the header, not the .cpp
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <cstdlib>
#include <ctime>

const int WIN_WIDTH = 1280;
const int WIN_HEIGHT = 720;

int main() {
    sf::RenderWindow window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "Visual Novel");

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    GameManager manager(window);
    manager.run();

    return 0;
}
