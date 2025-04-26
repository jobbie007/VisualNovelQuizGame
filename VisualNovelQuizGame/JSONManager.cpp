#include "JSONManager.h"
#include <fstream>
#include <iostream>
#include <algorithm>

JSONManager::JSONManager(sf::RenderWindow& win, const std::string& path)
    : window(win)
{
    // Load dialogue box UI
    dialoguePathCurrent = "Assets/Other/scroll.png";
    if (dialogueTexture.loadFromFile(dialoguePathCurrent)) {
        dialogueSprite.setTexture(dialogueTexture, true);
    }
    else {
        std::cerr << "Error loading dialogue box: " << dialoguePathCurrent << "\n";
        dialoguePathCurrent.clear();
    }

    // Load name box UI
    namePathCurrent = "Assets/Other/name_box.png";
    if (nameBoxTexture.loadFromFile(namePathCurrent)) {
        nameSprite.setTexture(nameBoxTexture, true);
    }
    else {
        std::cerr << "Error loading name box:    " << namePathCurrent << "\n";
        namePathCurrent.clear();
    }

    // Load font for text
    if (!font.loadFromFile("Assets/Fonts/arial.ttf")) {
        std::cerr << "Error loading font: Assets/Fonts/arial.ttf\n";
    }

    // Parse JSON script
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Cannot open JSON: " << path << "\n";
        return;
    }
    jsonData = json::parse(file, nullptr, false);
    if (jsonData.is_discarded()) {
        std::cerr << "JSON parse error:    " << path << "\n";
    }
}

void JSONManager::run() {
    processEvents();
}

void JSONManager::drawCurrentFrame() {
    window.clear();
    renderScene(window);
    renderDialogue(window);
    window.display();
}

std::optional<std::string> JSONManager::getNextEventKey(
    const json& events,
    const std::string& currentKey)
{
    auto it = events.find(currentKey);
    if (it == events.end() || ++it == events.end())
        return std::nullopt;
    return it.key();
}

void JSONManager::updateAssets(
    const std::string& bgPath,
    const std::string& leftPath,
    const std::string& rightPath,
    const std::string& audioPath,
    bool loop)
{
    // Background
    if (bgPath != bgPathCurrent) {
        if (bgPath.empty() || !backgroundTexture.loadFromFile(bgPath)) {
            std::cerr << "Failed to load background:   " << bgPath << "\n";
            bgPathCurrent.clear();
        }
        else {
            backgroundSprite.setTexture(backgroundTexture, true);
            bgPathCurrent = bgPath;
        }
    }
    // Left sprite
    if (leftPath != leftPathCurrent) {
        if (leftPath.empty() || !leftTexture.loadFromFile(leftPath)) {
            std::cerr << "Failed to load left sprite:   " << leftPath << "\n";
            leftPathCurrent.clear();
        }
        else {
            leftSprite.setTexture(leftTexture, true);
            leftPathCurrent = leftPath;
        }
    }
    // Right sprite
    if (rightPath != rightPathCurrent) {
        if (rightPath.empty() || !rightTexture.loadFromFile(rightPath)) {
            std::cerr << "Failed to load right sprite:  " << rightPath << "\n";
            rightPathCurrent.clear();
        }
        else {
            rightSprite.setTexture(rightTexture, true);
            rightPathCurrent = rightPath;
        }
    }
    // Audio
    if (audioPath != audioPathCurrent || loop != audioLoopCurrent) {
        if (music.getStatus() != sf::Music::Stopped)
            music.stop();

        if (audioPath.empty() || !music.openFromFile(audioPath)) {
            std::cerr << "Failed to load audio:        " << audioPath << "\n";
            audioPathCurrent.clear();
            audioLoopCurrent = false;
        }
        else {
            music.setLoop(loop);
            music.play();
            audioPathCurrent = audioPath;
            audioLoopCurrent = loop;
        }
    }
}

void JSONManager::processEvents() {
    if (jsonData.is_null() || !jsonData.contains("events"))
        return;

    const auto& events = jsonData["events"];
    std::string key = events.begin().key();

    while (!key.empty() && window.isOpen()) {
        const auto& ev = events[key];
        std::string type = ev.value("type", "");

        if (type == "dialogue") {
            for (const auto& item : ev["data"]) {
                currentSpeaker = item.value("speaker", "");
                currentLine = item.value("line", "");
                auto bg = item.value("background_sprite", "");
                auto left = item.value("left_sprite", "");
                auto right = item.value("right_sprite", "");
                auto audio = item["audio"].value("path", "");
                bool loop = item["audio"].value("loop", false);

                updateAssets(bg, left, right, audio, loop);

                window.clear();
                renderScene(window);
                renderDialogue(window);
                window.display();

                sf::Event evt;
                while (window.waitEvent(evt)) {
                    if (evt.type == sf::Event::KeyPressed && evt.key.code == sf::Keyboard::Space)
                        break;
                    if (evt.type == sf::Event::Closed) {
                        window.close();
                        return;
                    }
                }
            }
            // Advance
            if (ev.contains("next"))           key = ev["next"].get<std::string>();
            else if (auto nxt = getNextEventKey(events, key)) key = *nxt;
            else                                 break;

        }
        else if (type == "choice") {
            auto bg = ev.value("background_sprite", "");
            auto left = ev.value("left_sprite", "");
            auto right = ev.value("right_sprite", "");
            auto audio = ev["audio"].value("path", "");
            bool loop = ev["audio"].value("loop", false);

            updateAssets(bg, left, right, audio, loop);
            window.clear();
            renderScene(window);
            renderButtons(window);
            window.display();
            break;

        }
        else {
            break;
        }
    }

    if (music.getStatus() != sf::Music::Stopped)
        music.stop();
}

void JSONManager::renderScene(sf::RenderTarget& target) {
    auto size = target.getSize();
    float w = float(size.x), h = float(size.y);

    if (auto tex = backgroundSprite.getTexture()) {
        auto ts = tex->getSize();
        backgroundSprite.setScale(w / ts.x, h / ts.y);
        backgroundSprite.setPosition(0, 0);
        target.draw(backgroundSprite);
    }
    if (auto tex = leftSprite.getTexture()) {
        auto ts = tex->getSize();
        float dw = std::min(150.f, w / 4.f);
        leftSprite.setScale(dw / ts.x, h / ts.y);
        leftSprite.setPosition(0, 0);
        target.draw(leftSprite);
    }
    if (auto tex = rightSprite.getTexture()) {
        auto ts = tex->getSize();
        float dw = std::min(150.f, w / 4.f);
        rightSprite.setScale(dw / ts.x, h / ts.y);
        rightSprite.setPosition(w - dw, 0);
        target.draw(rightSprite);
    }
}

void JSONManager::renderDialogue(sf::RenderTarget& target) {
    auto size = target.getSize();
    float w = float(size.x), h = float(size.y), boxH = h / 4.f;

    if (auto tex = dialogueSprite.getTexture()) {
        auto ts = tex->getSize();
        dialogueSprite.setScale(w / ts.x, boxH / ts.y);
        dialogueSprite.setPosition(0, h - boxH);
        target.draw(dialogueSprite);
    }
    if (auto tex = nameSprite.getTexture()) {
        auto ts = tex->getSize();
        float nameH = boxH / 2.f;
        nameSprite.setScale(ts.x > 0 ? (250.f / ts.x) : 1.f, nameH / ts.y);
        nameSprite.setPosition(10.f, h - boxH - nameH - 5.f);
        target.draw(nameSprite);
    }
    if (!currentSpeaker.empty()) {
        sf::Text txt(currentSpeaker, font, 24);
        txt.setFillColor(sf::Color::White);
        txt.setPosition(nameSprite.getPosition().x + 10.f,
            nameSprite.getPosition().y + 5.f);
        target.draw(txt);
    }
    if (!currentLine.empty()) {
        sf::Text txt(currentLine, font, 20);
        txt.setFillColor(sf::Color::White);
        txt.setPosition(dialogueSprite.getPosition().x + 10.f,
            dialogueSprite.getPosition().y + 10.f);
        target.draw(txt);
    }
}

void JSONManager::renderButtons(sf::RenderTarget& target) {
    // TODO: draw choice options here
}
