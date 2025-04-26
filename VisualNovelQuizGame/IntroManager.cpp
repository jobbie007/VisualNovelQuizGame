// IntroManager.cpp
#include "IntroManager.h"
#include "JSONManager.h"

IntroManager::IntroManager(sf::RenderWindow& win)
    : window(win) {
}

void IntroManager::run() {
    // Make sure this path matches where you put DialogueTest.json
    JSONManager manager(window, "Assets/Jsons/DialogueTest.json");
    manager.run();
}
