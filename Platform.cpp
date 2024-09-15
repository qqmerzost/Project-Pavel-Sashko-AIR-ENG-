#include "Platform.h"

Platform::Platform() {
    texture.loadFromFile("C:/Users/sosko/Downloads/platform.png");
    sprite.setTexture(texture);
    x = rand() % 400;
    y = rand() % 533;
}

void Platform::update() {
    // Platforms don't move by themselves, but can be shifted with the camera
}

void Platform::draw(sf::RenderWindow& window) {
    sprite.setPosition(x, y);
    window.draw(sprite);
}

void Platform::respawn() {
    x = rand() % 400;
    y = 0;
}

sf::FloatRect Platform::getBounds() const {
    return sprite.getGlobalBounds();
}
