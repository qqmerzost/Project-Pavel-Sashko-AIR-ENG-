// Game.cpp
#include "Game.h"
#include <sstream>

Game::Game() : app(sf::VideoMode(400, 533), "Doodle Jump with Points and Coins") {
    app.setFramerateLimit(60);

    // Load background texture
    backgroundTexture.loadFromFile("C:/Users/sosko/Downloads/background.png");
    backgroundSprite1.setTexture(backgroundTexture);
    backgroundSprite2.setTexture(backgroundTexture);
    backgroundSprite2.setPosition(0, -533);

    // Load font
    font.loadFromFile("C:/Windows/Fonts/Arial.ttf");

    // Initialize texts
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::Black);
    scoreText.setPosition(10, 10);

    coinText.setFont(font);
    coinText.setCharacterSize(24);
    coinText.setFillColor(sf::Color::Black);
    coinText.setPosition(10, 40);

    gameOverText.setFont(font);
    gameOverText.setCharacterSize(24);
    gameOverText.setFillColor(sf::Color::Red);

    // Initialize player and coin
    player = new Player(400);
    gameObjects.push_back(std::unique_ptr<GameObject>(player));

    coin = new Coin();
    gameObjects.push_back(std::unique_ptr<GameObject>(coin));

    // Initialize platforms
    Platform* firstPlatform = new Platform();
    firstPlatform->y = player->y + 50;  // Spawn first platform just below the player
    platforms.push_back(firstPlatform);
    gameObjects.push_back(std::unique_ptr<GameObject>(firstPlatform));

    for (int i = 1; i < 10; i++) {
        Platform* platform = new Platform();
        platforms.push_back(platform);
        gameObjects.push_back(std::unique_ptr<GameObject>(platform));
    }
}

void Game::run() {
    while (app.isOpen()) {
        handleEvents();
        if (!gameOver) {
            update();
        }
        render();
    }
}

void Game::handleEvents() {
    sf::Event e;
    while (app.pollEvent(e)) {
        if (e.type == sf::Event::Closed) {
            app.close();
        }
    }
}

void Game::update() {
    for (auto& obj : gameObjects) {
        obj->update();
    }

    for (auto& platform : platforms) {
        if (player->isOnPlatform(platform->getBounds())) {
            player->bounce();
        }
    }

    if (player->isCollectingCoin(coin->getBounds())) {
        coinsCollected++;
        coin->respawnAtPlatform(rand() % 400, rand() % 533);
    }

    if (player->y < 200) {
        float shiftAmount = 200 - player->y;
        cumulativeShift += shiftAmount;

        for (auto& platform : platforms) {
            platform->y += shiftAmount;
            if (platform->y > 533) {
                platformCounter++;
                platform->respawn();
                if (platformCounter % 20 == 0) {
                    coin->respawnAtPlatform(platform->x, platform->y);
                }
            }
        }

        coin->y += shiftAmount;
        if (coin->y > 533) {
            coin->respawnAtPlatform(rand() % 400, rand() % 533);
        }

        backgroundSprite1.move(0, shiftAmount);
        backgroundSprite2.move(0, shiftAmount);

        if (backgroundSprite1.getPosition().y >= 533) {
            backgroundSprite1.setPosition(0, backgroundSprite2.getPosition().y - 533);
        }
        if (backgroundSprite2.getPosition().y >= 533) {
            backgroundSprite2.setPosition(0, backgroundSprite1.getPosition().y - 533);
        }

        player->y = 200;
        points = static_cast<int>(cumulativeShift);
    }

    if (player->y > player->windowHeight) {
        gameOver = true;
        std::stringstream gameOverStream;
        gameOverStream << "Game Over!\nFinal Score: " << points << "\nCoins Collected: " << coinsCollected;
        gameOverText.setString(gameOverStream.str());
        sf::FloatRect textRect = gameOverText.getLocalBounds();
        gameOverText.setOrigin(textRect.width / 2, textRect.height / 2);
        gameOverText.setPosition(app.getSize().x / 2, app.getSize().y / 2);
    }

    std::stringstream ss;
    ss << "Score: " << points;
    scoreText.setString(ss.str());

    std::stringstream coinStream;
    coinStream << "Coins: " << coinsCollected;
    coinText.setString(coinStream.str());
}

void Game::render() {
    app.clear();

    if (!gameOver) {
        app.draw(backgroundSprite1);
        app.draw(backgroundSprite2);

        for (auto& obj : gameObjects) {
            obj->draw(app);
        }

        app.draw(scoreText);
        app.draw(coinText);
    }
    else {
        app.draw(gameOverText);
    }

    app.display();
}
