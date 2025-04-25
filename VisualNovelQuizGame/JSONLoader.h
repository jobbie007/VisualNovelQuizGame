#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
#include <string>
#include <nlohmann/json.hpp>
#include <vector>
#include <optional>
#include <memory>
#include <map>

class JSONLoader {
public:
    JSONLoader(sf::RenderWindow& window, const std::string& jsonPath);
    ~JSONLoader();

    void run();
    void handle_input(const sf::Event& event);
    void update(sf::Time dt);
    void draw();
    bool is_finished() const;

private:
    // Window and JSON data
    sf::RenderWindow& window_;
    std::string jsonPath_;
    nlohmann::json jsonData_;
    std::vector<std::string> event_keys_;
    size_t current_event_index_ = 0;
    std::optional<std::string> current_event_type_;
    nlohmann::json current_event_data_;
    size_t current_dialogue_index_ = 0;
    bool finished_ = false;

    // UI Elements
    sf::Font font_;
    sf::Text speaker_text_;
    sf::Text line_text_;
    sf::RectangleShape dialogue_box_shape_;
    sf::Text choice_prompt_text_;
    std::vector<struct ChoiceButton> choice_buttons_;

    // Sprites and Textures
    sf::Sprite background_sprite_;
    sf::Texture background_texture_;
    sf::Sprite left_sprite_sprite_;
    sf::Texture left_sprite_texture_;
    sf::Sprite right_sprite_sprite_;
    sf::Texture right_sprite_texture_;
    sf::Music audio_player_;

    // Helper Methods
    void loadJSON();
    void processEvents();
    void load_current_event();
    void advance_event();
    void advance_dialogue();
    void handle_choice(int choice_index);
    void draw_dialogue_scene();
    void draw_choice_scene();
    void setup_dialogue_ui();
    void setup_choice_ui();
    bool load_texture(sf::Texture& texture, const std::string& path);
    void load_dialogue_line_assets(const nlohmann::json& line_data);
    void load_choice_assets(const nlohmann::json& choice_event_data);
};

// Choice button structure
struct ChoiceButton {
    sf::RectangleShape shape;
    sf::Text text;
    int choice_index;
}; 