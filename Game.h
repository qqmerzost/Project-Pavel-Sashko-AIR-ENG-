// Game.h
#ifndef GAME_H
#define GAME_H

#include "Player.h"
#include "Platform.h"
#include "Coin.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class Game {
private:
    sf::RenderWindow app;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite1, backgroundSprite2;
    sf::Font font;
    sf::Text scoreText, coinText, gameOverText;
    int points = 0;
    int coinsCollected = 0;
    bool gameOver = false;
    float cumulativeShift = 0;
    int platformCounter = 0;

    Player* player;
    Coin* coin;
    std::vector<Platform*> platforms;
    std::vector<std::unique_ptr<GameObject>> gameObjects;

public:
    Game();
    void run();

private:
    void handleEvents();
    void update();
    void render();
};

#endif
