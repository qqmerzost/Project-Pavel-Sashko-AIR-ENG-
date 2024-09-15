#ifndef COIN_H
#define COIN_H

#include "GameObject.h"
#include <SFML/Graphics.hpp>

class Coin : public GameObject {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    int x, y;

    Coin();
    void update() override;
    void draw(sf::RenderWindow& window) override;
    void respawnAtPlatform(int platformX, int platformY);

    sf::FloatRect getBounds() const;
};

#endif
