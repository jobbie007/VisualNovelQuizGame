#pragma once // Header guard

#include <SFML/Graphics.hpp> // SFML Graphics
#include <SFML/Audio.hpp>    // SFML Audio
#include <nlohmann/json.hpp> // nlohmann JSON
#include <string>            // std::string
#include <fstream>           // std::ifstream
#include <iostream>          // std::cerr, std::cout
#include <vector>            // std::vector
#include <map>               // std::map
#include <stdexcept>         // std::runtime_error
#include <chrono>            // std::chrono
#include <thread>            // std::this_thread
#include <optional>          // std::optional

// JSON namespace alias
using json = nlohmann::json;

/**
 * @class JSONManager
 * @brief Loads and manages the flow of events defined in a JSON file,
 * including updating scene assets (sprites, audio).
 */
class JSONManager {

private:
    // --- SFML Resources ---
    sf::RenderWindow& windowRef;        // Reference to the game window
    sf::Texture backgroundTexture;      // Texture for the background
    sf::Texture leftTexture;            // Texture for the left character sprite
    sf::Texture rightTexture;           // Texture for the right character sprite
    sf::Sprite backgroundSprite;        // Sprite for the background
    sf::Sprite leftSprite;              // Sprite for the left character
    sf::Sprite rightSprite;             // Sprite for the right character
    sf::Music music;                    // Music object for handling audio playback

    // --- State Tracking ---
    json jsonData;                      // Parsed JSON data
    std::string currentBackgroundPath;  // Path of the currently loaded background
    std::string currentLeftPath;        // Path of the currently loaded left sprite
    std::string currentRightPath;       // Path of the currently loaded right sprite
    std::string currentAudioPath;       // Path of the currently loaded/playing audio
    bool currentAudioLoop = false;      // Loop status of the current audio

    // --- Helper to get the next event key sequentially (use with caution) ---
    std::optional<std::string> getNextEventKey(const json& object, const std::string& currentKey) {
        auto it = object.find(currentKey);
        if (it == object.end()) return std::nullopt;
        ++it;
        if (it != object.end()) return it.key();
        return std::nullopt;
    }

    /**
     * @brief Updates the scene's visual and audio assets based on provided paths.
     * Loads textures and audio only if the paths have changed or were previously empty.
     *
     * @param bgPath Path to the background sprite image.
     * @param leftPath Path to the left sprite image.
     * @param rightPath Path to the right sprite image.
     * @param audioPath Path to the audio file.
     * @param loopAudio Whether the audio file should loop.
     */
    void updateSceneAssets(const std::string& bgPath, const std::string& leftPath, const std::string& rightPath, const std::string& audioPath, bool loopAudio) {
        // --- Update Background ---
        if (bgPath != currentBackgroundPath) {
            if (!bgPath.empty()) {
                if (!backgroundTexture.loadFromFile(bgPath)) {
                    std::cerr << "Warning: Failed to load background texture: " << bgPath << std::endl;
                    currentBackgroundPath = ""; // Mark as failed/empty
                }
                else {
                    std::cout << "  Loaded background: " << bgPath << std::endl;
                    backgroundSprite.setTexture(backgroundTexture, true); // Reset texture rect
                    currentBackgroundPath = bgPath;
                }
            }
            else {
                // If the new path is empty, clear the current path
                currentBackgroundPath = "";
                // Optional: Clear the sprite's texture? backgroundSprite.setTextureRect(sf::IntRect());
            }
        }

        // --- Update Left Sprite ---
        if (leftPath != currentLeftPath) {
            if (!leftPath.empty()) {
                if (!leftTexture.loadFromFile(leftPath)) {
                    std::cerr << "Warning: Failed to load left texture: " << leftPath << std::endl;
                    currentLeftPath = "";
                }
                else {
                    std::cout << "  Loaded left sprite: " << leftPath << std::endl;
                    leftSprite.setTexture(leftTexture, true);
                    currentLeftPath = leftPath;
                }
            }
            else {
                currentLeftPath = "";
            }
        }

        // --- Update Right Sprite ---
        if (rightPath != currentRightPath) {
            if (!rightPath.empty()) {
                if (!rightTexture.loadFromFile(rightPath)) {
                    std::cerr << "Warning: Failed to load right texture: " << rightPath << std::endl;
                    currentRightPath = "";
                }
                else {
                    std::cout << "  Loaded right sprite: " << rightPath << std::endl;
                    rightSprite.setTexture(rightTexture, true);
                    currentRightPath = rightPath;
                }
            }
            else {
                currentRightPath = "";
            }
        }

        // --- Update Audio ---
        if (audioPath != currentAudioPath || (audioPath == currentAudioPath && !audioPath.empty() && loopAudio != currentAudioLoop)) {
            // Stop current music if path changes or if path is same but loop status needs update
            if (music.getStatus() != sf::Music::Stopped) {
                music.stop();
            }

            if (!audioPath.empty()) {
                if (!music.openFromFile(audioPath)) {
                    std::cerr << "Warning: Failed to open audio file: " << audioPath << std::endl;
                    currentAudioPath = ""; // Mark as failed/empty
                    currentAudioLoop = false;
                }
                else {
                    std::cout << "  Loaded audio: " << audioPath << " (Loop: " << (loopAudio ? "true" : "false") << ")" << std::endl;
                    currentAudioPath = audioPath;
                    currentAudioLoop = loopAudio;
                    music.setLoop(currentAudioLoop);
                    music.play();
                }
            }
            else {
                // If the new path is empty, clear the current path and ensure music is stopped
                currentAudioPath = "";
                currentAudioLoop = false;
                // Music was already stopped above if it was playing
            }
        }
    }


public:

    /**
     * @brief Constructor for JSONManager.
     * @param window Reference to the SFML RenderWindow.
     * @param jsonFilePath Path to the JSON file to load.
     */
    JSONManager(sf::RenderWindow& window, const std::string& jsonFilePath)
        : windowRef(window) // Initialize window reference
    {
        // Open the JSON file
        std::ifstream jsonFileStream(jsonFilePath);
        if (!jsonFileStream.is_open()) {
            std::cerr << "Error: Could not open JSON file: " << jsonFilePath << std::endl;
            jsonData = nullptr;
            return;
        }

        // Parse the JSON data
        try {
            jsonData = json::parse(jsonFileStream);
            std::cout << "Successfully loaded and parsed JSON file: " << jsonFilePath << std::endl;
        }
        catch (json::parse_error& e) {
            std::cerr << "Error: Failed to parse JSON: " << jsonFilePath << "\n"
                << "Parse error details: " << e.what() << std::endl;
            jsonData = nullptr;
        }
    }

    /**
     * @brief Processes the events defined in the loaded JSON data sequentially.
     * This function focuses on iterating through the JSON and calling asset updates.
     * It simulates pauses where the game loop would take over for rendering and input.
     */
    void processEvents() {
        if (jsonData.is_null()) { /* ... Error check ... */ return; }
        if (!jsonData.contains("events") || !jsonData["events"].is_object() || jsonData["events"].empty()) { /* ... Error check ... */ return; }

        std::string currentEventName = jsonData["events"].begin().key();
        std::cout << "Info: Assuming first event '" << currentEventName << "' as start." << std::endl;

        while (!currentEventName.empty()) {
            std::cout << "\n--- Processing Event: '" << currentEventName << "' ---" << std::endl;

            if (!jsonData["events"].contains(currentEventName)) { /* ... Error check ... */ break; }
            const json& currentEvent = jsonData["events"][currentEventName];

            if (!currentEvent.contains("type") || !currentEvent["type"].is_string()) { /* ... Error check ... */ break; }
            std::string eventType = currentEvent["type"].get<std::string>();
            std::cout << "Event Type: " << eventType << std::endl;

            // --- Dialogue Event Handling ---
            if (eventType == "dialogue") {
                if (!currentEvent.contains("data") || !currentEvent["data"].is_array()) { /* ... Error check ... */ break; }
                const json& dialogueData = currentEvent["data"];
                std::cout << "Dialogue Items to process: " << dialogueData.size() << std::endl;

                for (const auto& item : dialogueData) {
                    if (!item.is_object()) { /* ... Error check ... */ continue; }

                    // Extract dialogue variables
                    std::string speaker = item.value("speaker", "Narrator"); // Default speaker
                    std::string line = item.value("line", "...");           // Default line
                    std::string background_sprite = item.value("background_sprite", "");
                    std::string left_sprite = item.value("left_sprite", "");
                    std::string right_sprite = item.value("right_sprite", "");

                    // Extract audio data (handling the new object structure)
                    std::string audioPath = "";
                    bool audioLoop = false;
                    if (item.contains("audio") && item["audio"].is_object()) {
                        const json& audioObj = item["audio"];
                        audioPath = audioObj.value("path", ""); // Get path, default to empty
                        audioLoop = audioObj.value("loop", false); // Get loop, default to false
                    }
                    else if (item.contains("audio") && item["audio"].is_string()) {
                        // Optional: Handle legacy string format if needed
                        // audioPath = item.value("audio", "");
                        // audioLoop = false; // Default loop for legacy string
                        std::cerr << "Warning: Found legacy string format for 'audio' in event '"
                            << currentEventName << "'. Expected object format." << std::endl;
                    }


                    // --- Update Assets based on current dialogue item ---
                    updateSceneAssets(background_sprite, left_sprite, right_sprite, audioPath, audioLoop);

                    // --- Output for Debugging ---
                    std::cout << "  [" << speaker << "]: " << line << std::endl;
                    // (No need to print asset paths here as updateSceneAssets logs loading)


                    // <<<< PAUSE POINT >>>>
                    // In a real game loop:s
                    // 1. This function (`processEvents` or a stateful equivalent) would return/yield here.
                    // 2. The main loop calls `renderCurrentScene(windowRef)` to draw sprites.
                    // 3. The main loop draws the dialogue text (`speaker`, `line`) onto the window.
                    // 4. The main loop calls `windowRef.display()`.
                    // 5. The main loop waits for user input (e.g., space press, mouse click).
                    // 6. On input, the main loop calls a function to advance to the next dialogue item
                    //    or the next event, resuming this `processEvents` logic.
                    std::cout << "    (Simulating display & waiting for user to continue...)" << std::endl;
                    // std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Simulate short delay

                } // --- End of loop through dialogue items ---

                // --- Transition to the next event (Dialogue specific) ---
                if (currentEvent.contains("next") && currentEvent["next"].is_string()) {
                    currentEventName = currentEvent["next"].get<std::string>();
                    std::cout << "Dialogue finished. Transitioning via 'next' to: '" << currentEventName << "'" << std::endl;
                }
                else {
                    std::cout << "Warning: Dialogue event '" << currentEventName << "' has no 'next' key. Attempting sequential." << std::endl;
                    std::optional<std::string> nextKey = getNextEventKey(jsonData["events"], currentEventName);
                    if (nextKey) {
                        currentEventName = *nextKey;
                        std::cout << "Dialogue finished. Transitioning sequentially to: '" << currentEventName << "'" << std::endl;
                    }
                    else {
                        std::cout << "Dialogue finished. No 'next' or sequential event. Ending processing." << std::endl;
                        currentEventName = "";
                    }
                }

                // --- Choice Event Handling ---
            }
            else if (eventType == "choice") {
                if (!currentEvent.contains("data") || !currentEvent["data"].is_object()) { /* ... Error check ... */ break; }
                if (!currentEvent.contains("branches") || !currentEvent["branches"].is_object()) { /* ... Error check ... */ currentEventName = ""; continue; } // Needs branches!

                const json& choiceData = currentEvent["data"];
                const json& branches = currentEvent["branches"];

                // Extract choice-level assets (including audio)
                std::string background_sprite = currentEvent.value("background_sprite", "");
                std::string left_sprite = currentEvent.value("left_sprite", "");
                std::string right_sprite = currentEvent.value("right_sprite", "");
                std::string audioPath = "";
                bool audioLoop = false;
                if (currentEvent.contains("audio") && currentEvent["audio"].is_object()) {
                    const json& audioObj = currentEvent["audio"];
                    audioPath = audioObj.value("path", "");
                    audioLoop = audioObj.value("loop", false);
                }

                // --- Update assets for the choice screen ---
                updateSceneAssets(background_sprite, left_sprite, right_sprite, audioPath, audioLoop);

                // Extract prompt
                std::string promptText = "Choose:";
                // Updated path for prompt text based on provided JSON snippet
                if (choiceData.contains("prompt") && choiceData["prompt"].is_object() && choiceData["prompt"].contains("text") && choiceData["prompt"]["text"].is_string()) {
                    promptText = choiceData["prompt"]["text"].get<std::string>();
                }
                else {
                    std::cerr << "Warning: Choice event '" << currentEventName << "' data is missing a valid 'prompt.text' string. Using default." << std::endl;
                }
                std::cout << "Prompt: " << promptText << std::endl;


                // Extract choices
                std::map<std::string, std::string> availableChoices;
                std::cout << "Choices Available:" << std::endl;
                for (auto& [key, value] : choiceData.items()) {
                    if (key == "prompt") continue; // Skip the prompt object itself

                    if (value.is_object() && value.contains("text") && value["text"].is_string()) {
                        availableChoices[key] = value["text"].get<std::string>();
                        std::cout << "  - [" << key << "]: " << availableChoices[key] << std::endl;
                    }
                    else {
                        std::cerr << "Warning: Choice key '" << key << "' in event '" << currentEventName << "' data is not an object with a valid 'text' string. Skipping." << std::endl;
                    }
                }

                if (availableChoices.empty()) { /* ... Error check ... */ currentEventName = ""; continue; }

                // <<<< PAUSE POINT >>>>

            }
            else {
                std::cerr << "Error: Unknown event type '" << eventType << "'..." << std::endl;
                currentEventName = "";
            }

        } // --- End of while loop ---

        std::cout << "\n--- Event processing finished ---" << std::endl;
        // Ensure music stops at the very end if it was playing
        if (music.getStatus() != sf::Music::Stopped) {
            music.stop();
        }
    }

   
    //draws the background and sprites
    void renderCurrentScene(sf::RenderTarget& target) {
        sf::Vector2u targetSize = target.getSize();
        float targetWidth = static_cast<float>(targetSize.x);
        float targetHeight = static_cast<float>(targetSize.y);

        // ---Draw Background ---
        if (!currentBackgroundPath.empty() && backgroundSprite.getTexture()) {
            // Scale to fit target size
            sf::Vector2u texSize = backgroundSprite.getTexture()->getSize();
            backgroundSprite.setScale(targetWidth / texSize.x, targetHeight / texSize.y);
            backgroundSprite.setPosition(0, 0);
            target.draw(backgroundSprite);
        }

        // --- Draw Left Sprite ---
        if (!currentLeftPath.empty() && leftSprite.getTexture()) {
            // Example scaling: 150px width, full height
            sf::Vector2u texSize = leftSprite.getTexture()->getSize();
            float desiredWidth = 150.0f;
            // Adjust desiredWidth if window is very small?
            if (targetWidth < desiredWidth * 2) desiredWidth = targetWidth / 4.0f; // Example adjustment

            leftSprite.setScale(desiredWidth / texSize.x, targetHeight / texSize.y);
            leftSprite.setPosition(0, 0);
            target.draw(leftSprite);
        }

        // --- Draw Right Sprite ---
        if (!currentRightPath.empty() && rightSprite.getTexture()) {
            // Example scaling: 150px width, full height
            sf::Vector2u texSize = rightSprite.getTexture()->getSize();
            float desiredWidth = 150.0f;
            if (targetWidth < desiredWidth * 2) desiredWidth = targetWidth / 4.0f; // Example adjustment

            rightSprite.setScale(desiredWidth / texSize.x, targetHeight / texSize.y);
            // Position at the right edge: target width - scaled width
            rightSprite.setPosition(targetWidth - (desiredWidth / texSize.x * texSize.x), 0);
            target.draw(rightSprite);
        }
    }


    //draws all the data in the jsons onto the window, until json is completely parsed through
    void run() {
        std::cout << "JSONManager run() called." << std::endl;
        if (!jsonData.is_null()) {
            processEvents(); // Executes the whole sequence with simulated pauses
        }
        else {
            std::cerr << "JSONManager run() aborted: JSON data was not loaded successfully." << std::endl;
        }
    }

}; // End of JSONManager class
