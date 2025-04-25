#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <map>

class MinigameRhythm {
private:
    struct FallingImage {
        sf::Sprite sprite;
        sf::Keyboard::Key keybind;
    };

    // Window and background
    sf::RenderWindow window;
    sf::Texture fullBackgroundTexture;
    sf::Sprite fullBackground;

    // Game elements
    std::map<std::string, sf::Texture> arrowTextures;
    std::vector<FallingImage*> fallingImages;
    float spawnTimer;
    float spawnInterval;
    float speedMultiplier;
    int score;
    sf::Font robotoFont;
    sf::Font fnfFont;
    sf::Text scoreText;
    sf::Text feedbackText;
    sf::Text gameOverText;
    float feedbackTimer;
    sf::RectangleShape detectionZone;

    // Timing and scaling
    float speedIncreaseTimer;
    const float ScalingRate = 0.5f;
    const float baseSpeed = 200.0f;

    // Positions and mappings
    std::vector<float> spawnPositions;
    std::vector<sf::Keyboard::Key> keybinds;
    std::map<sf::Keyboard::Key, std::string> keyToDirection;

    // Sizes
    const sf::Vector2f fullBackgroundSize{ 1920,1080 };
    const sf::Vector2f arrowSize{ 100,100 };
    const sf::Vector2f bottomBackgroundSize{ 1920,150 };

    sf::Clock clock;

    // Sound effects
    sf::SoundBuffer hitBuffer;
    sf::SoundBuffer missBuffer;
    sf::Sound hitSound;
    sf::Sound missSound;

    // Health bar
    int health = 15;
    const int maxHealth = 15;
    sf::Texture healthBarTexture;
    sf::Sprite healthBarSprite;
    sf::RectangleShape healthFill;
    sf::Vector2f healthBarSize = { 300.f, 30.f };
    sf::Vector2f healthBarPosition = { 30.f, 30.f };

    // Game state
    bool isGameOver = false;

public:
    MinigameRhythm()
        : window(sf::VideoMode(1920, 1080), "Rhythm Game", sf::Style::Fullscreen)
        , spawnTimer(0.0f)
        , spawnInterval(1.5f)
        , speedMultiplier(1.0f)
        , speedIncreaseTimer(0.0f)
        , score(0)
        , feedbackTimer(0.0f)
        , spawnPositions{ 810, 910, 1010, 1110 }
        , keybinds{ sf::Keyboard::Left, sf::Keyboard::Down, sf::Keyboard::Up, sf::Keyboard::Right }
    {
        initializeBackground();
        initializeArrows();
        initializeUI();
        initializeSounds();
        initializeHealthBar();
        std::srand(static_cast<unsigned>(std::time(nullptr)));
    }

    ~MinigameRhythm() {
        for (auto* img : fallingImages) delete img;
    }

    void run() {
        while (window.isOpen()) {
            handleEvents();
            update();
            render();
        }
    }

private:
    void initializeBackground() {
        loadAndScaleTexture(fullBackgroundTexture, fullBackground,
            "Assets/Minigame/RhythmBack.png", fullBackgroundSize);
    }

    void loadAndScaleTexture(sf::Texture& texture, sf::Sprite& sprite,
        const std::string& filename, const sf::Vector2f& targetSize)
    {
        if (!texture.loadFromFile(filename)) std::cerr << "Failed to load texture: " << filename << std::endl;
        sprite.setTexture(texture);
        sf::Vector2u ts = texture.getSize();
        sprite.setScale(targetSize.x / ts.x, targetSize.y / ts.y);
    }

    void initializeArrows() {
        keyToDirection = {
            { sf::Keyboard::Left,  "left"  },
            { sf::Keyboard::Down,  "down"  },
            { sf::Keyboard::Up,    "up"    },
            { sf::Keyboard::Right, "right" }
        };
        for (auto& [key, dir] : keyToDirection) {
            sf::Texture tex;
            std::string fn = "Assets/Minigame/Entities/arrow_" + dir + ".png";
            if (!tex.loadFromFile(fn)) std::cerr << "Failed to load arrow: " << fn << std::endl;
            arrowTextures["arrow_" + dir] = tex;
        }
    }

    void initializeUI() {
        robotoFont.loadFromFile("Assets/Font/roboto.ttf");
        fnfFont.loadFromFile("Assets/Font/fnf.ttf");

        scoreText.setFont(robotoFont);
        scoreText.setCharacterSize(36);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10.f, 60.f);

        feedbackText.setFont(fnfFont);
        feedbackText.setCharacterSize(72);
        feedbackText.setFillColor(sf::Color::Red);
        feedbackText.setPosition(1400.f, 100.f);

        gameOverText.setFont(fnfFont);
        gameOverText.setCharacterSize(100);
        gameOverText.setFillColor(sf::Color::Red);
        gameOverText.setString("GAME OVER\nPress F to Exit");
        gameOverText.setPosition(600.f, 400.f);

        detectionZone.setSize(bottomBackgroundSize);
        detectionZone.setFillColor(sf::Color(0, 0, 255, 100));
        detectionZone.setPosition(0.f, 1080.f - bottomBackgroundSize.y);
    }

    void initializeSounds() {
        hitBuffer.loadFromFile("Assets/Sounds/hit_sound.wav");
        hitSound.setBuffer(hitBuffer);
        missBuffer.loadFromFile("Assets/Sounds/miss_sound.wav");
        missSound.setBuffer(missBuffer);
    }

    void initializeHealthBar() {
        healthBarTexture.loadFromFile("Assets/Minigame/Entities/HealthBar.png");
        healthBarSprite.setTexture(healthBarTexture);
        healthBarSprite.setPosition(healthBarPosition);
        auto ts = healthBarTexture.getSize();
        healthBarSprite.setScale(healthBarSize.x / ts.x, healthBarSize.y / ts.y);

        healthFill.setSize(healthBarSize);
        healthFill.setFillColor(sf::Color::Red);
        healthFill.setPosition(healthBarPosition);
    }

    void handleEvents() {
        sf::Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) window.close();
        }
    }

    void update() {
        // On Game Over, await F to close
        if (isGameOver) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) window.close();
            return;
        }
        float dt = clock.restart().asSeconds();
        spawnTimer += dt;
        speedIncreaseTimer += dt;
        feedbackTimer -= dt;

        if (speedIncreaseTimer >= ScalingRate) {
            spawnInterval = std::max(0.3f, spawnInterval - 0.05f);
            speedMultiplier += 0.1f;
            speedIncreaseTimer = 0.f;
        }
        if (spawnTimer >= spawnInterval) {
            spawnFallingImage();
            spawnTimer = 0.f;
        }

        float moveAmt = baseSpeed * speedMultiplier * dt;
        for (auto* img : fallingImages) img->sprite.move(0, moveAmt);

        for (auto it = fallingImages.begin(); it != fallingImages.end();) {
            auto* img = *it;
            if (img->sprite.getPosition().y > window.getSize().y) {
                delete img;
                it = fallingImages.erase(it);
                feedbackText.setString("MISS!");
                feedbackText.setFillColor(sf::Color::Red);
                feedbackTimer = 1.f;
                missSound.play();
                if (--health <= 0) {
                    health = 0;
                    isGameOver = true;
                }
            }
            else ++it;
        }
        for (auto key : keybinds) if (sf::Keyboard::isKeyPressed(key)) checkKeyPress(key);

        // Update health fill width
        float w = (healthBarSize.x / maxHealth) * health;
        healthFill.setSize({ w, healthBarSize.y });
    }

    void render() {
        window.clear();
        window.draw(fullBackground);
        for (auto* img : fallingImages) window.draw(img->sprite);
        window.draw(detectionZone);

        window.draw(healthFill);
        window.draw(healthBarSprite);

        scoreText.setString("Score: " + std::to_string(score));
        window.draw(scoreText);
        if (feedbackTimer > 0) window.draw(feedbackText);

        if (isGameOver) window.draw(gameOverText);

        window.display();
    }

    void spawnFallingImage() {
        auto* img = new FallingImage;
        int idx = std::rand() % 4;
        img->keybind = keybinds[idx];
        std::string dir = keyToDirection[img->keybind];
        auto& tex = arrowTextures["arrow_" + dir];
        img->sprite.setTexture(tex);
        auto ts = tex.getSize();
        img->sprite.setScale(arrowSize.x / ts.x, arrowSize.y / ts.y);
        img->sprite.setOrigin(ts.x / 2.f, ts.y / 2.f);
        img->sprite.setPosition(spawnPositions[idx], -arrowSize.y);
        fallingImages.push_back(img);
    }

    void checkKeyPress(sf::Keyboard::Key key) {
        for (auto it = fallingImages.begin(); it != fallingImages.end();) {
            auto* img = *it;
            if (img->keybind == key && img->sprite.getGlobalBounds().intersects(detectionZone.getGlobalBounds())) {
                delete img;
                it = fallingImages.erase(it);
                score++;
                feedbackText.setString("HIT!");
                feedbackText.setFillColor(sf::Color::Green);
                feedbackTimer = 1.f;
                hitSound.play();
            }
            else ++it;
        }
    }
};
