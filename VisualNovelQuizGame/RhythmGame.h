#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <memory>

class RhythmGame {
public:
    RhythmGame(sf::RenderWindow& window);
    void run();

private:
    sf::RenderWindow& window_;
    sf::Font font_;
    sf::Music music_;
    sf::SoundBuffer hit_sound_buffer_;
    sf::Sound hit_sound_;
    
    struct Note {
        sf::RectangleShape shape;
        float speed;
        bool active;
    };
    
    std::vector<Note> notes_;
    float spawn_timer_;
    float spawn_interval_;
    int score_;
    sf::Text score_text_;
    
    void setup_ui();
    void spawn_note();
    void update_notes(float delta_time);
    void handle_input(const sf::Event& event);
    void draw();
    void check_collisions();
}; 