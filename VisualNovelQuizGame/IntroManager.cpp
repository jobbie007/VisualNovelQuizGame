#include "JSONManager.cpp"
#include <SFML/Graphics.hpp>

class IntroManager {
private:
    sf::RenderWindow& window;

public:
    IntroManager(sf::RenderWindow& win) : window(win) {}

    void run() {
        JSONManager intro(window, "Assets/Jsons/Intro/Intro.json");
        intro.run();
    }
};
