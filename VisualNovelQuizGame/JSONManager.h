// JSONManager.hpp
#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <optional>

using json = nlohmann::json;

class JSONManager {
public:
    // Construct with the render window and path to JSON script
    JSONManager(sf::RenderWindow& win, const std::string& path);

    // Process all events (dialogue & choice), blocking on Space
    void run();

    // Redraw the last frame (scene + dialogue) for display loops
    void drawCurrentFrame();

private:
    // Reference to the game window
    sf::RenderWindow& window;

    // Scene assets
    sf::Texture backgroundTexture, leftTexture, rightTexture;
    sf::Sprite  backgroundSprite, leftSprite, rightSprite;

    // Dialogue UI assets
    sf::Texture dialogueTexture, nameBoxTexture;
    sf::Sprite  dialogueSprite, nameSprite;

    // Font and current text
    sf::Font    font;
    std::string currentSpeaker;
    std::string currentLine;

    // Music playback
    sf::Music music;

    // Parsed JSON data
    json jsonData;

    // Track currently loaded asset paths to avoid redundant reloads
    std::string bgPathCurrent, leftPathCurrent, rightPathCurrent;
    std::string dialoguePathCurrent, namePathCurrent;
    std::string audioPathCurrent;
    bool        audioLoopCurrent = false;

    // Load next event key in JSON map order
    std::optional<std::string>
        getNextEventKey(const json& events, const std::string& currentKey);

    // Load textures and audio when paths change
    void updateAssets(const std::string& bgPath,
        const std::string& leftPath,
        const std::string& rightPath,
        const std::string& audioPath,
        bool loop);

    // Core event loop: dialogue & choice handling
    void processEvents();

    // Rendering helpers
    void renderScene(sf::RenderTarget& target);
    void renderDialogue(sf::RenderTarget& target);
    void renderButtons(sf::RenderTarget& target);
};
