#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class MainMenu {
public:
    MainMenu(sf::RenderWindow& window);
    void run();

private:
    sf::RenderWindow& window_;
    sf::Font font_;
    sf::Text title_text_;
    sf::Text start_text_;
    sf::Text exit_text_;
    sf::RectangleShape start_button_;
    sf::RectangleShape exit_button_;

    void setup_ui();
    void handle_input(const sf::Event& event);
    void draw();
}; 