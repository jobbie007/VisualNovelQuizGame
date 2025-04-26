// main.cpp
#include <SFML/Graphics.hpp>
#include "JSONManager.h"

int main() {
    // Create window
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Visual Novel");

    // Initialize JSONManager with your script path
    JSONManager manager(window, "Assets/Jsons/DialogueTest.json");
    manager.run();  // Play through dialogue (Space advances)

    // After run() completes, keep displaying the last frame
    sf::Event evt;
    while (window.isOpen()) {
        while (window.pollEvent(evt)) {
            if (evt.type == sf::Event::Closed)
                window.close();
        }
        manager.drawCurrentFrame();
    }
    return 0;
}