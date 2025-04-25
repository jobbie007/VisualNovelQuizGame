#pragma once
#include <SFML/Graphics.hpp>
#include "MainMenu.h"
#include "IntroManager.h"
#include "StageOneManager.h"
#include "RhythmGame.h"

class VisualNovelQuizGame {
public:
    VisualNovelQuizGame();
    void run();

private:
    sf::RenderWindow window_;
    std::unique_ptr<MainMenu> main_menu_;
    std::unique_ptr<IntroManager> intro_manager_;
    std::unique_ptr<StageOneManager> stage_one_manager_;
    std::unique_ptr<RhythmGame> rhythm_game_;
    
    void initialize_window();
    void handle_events();
    void update();
    void render();
}; 