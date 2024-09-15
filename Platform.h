#ifndef PLATFORM_H
#define PLATFORM_H

#include "GameObject.h"
#include <SFML/Graphics.hpp>

class Platform : public GameObject {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    int x, y;

    Platform();
    void update() override;
    void draw(sf::RenderWindow& window) override;
    void respawn();

    sf::FloatRect getBounds() const;
};

#endif
