#include "Coin.h"

Coin::Coin() {
    texture.loadFromFile("C:/Users/sosko/Downloads/coin.png");
    sprite.setTexture(texture);
    sprite.setScale(0.12f, 0.12f);  
    x = rand() % 400;
    y = rand() % 533;
}

void Coin::update() {
    
}

void Coin::draw(sf::RenderWindow& window) {
    sprite.setPosition(x, y);
    window.draw(sprite);
}

void Coin::respawnAtPlatform(int platformX, int platformY) {
    x = platformX + rand() % 50 - 25;
    y = platformY - 30;  
}

sf::FloatRect Coin::getBounds() const {
    return sprite.getGlobalBounds();
}
