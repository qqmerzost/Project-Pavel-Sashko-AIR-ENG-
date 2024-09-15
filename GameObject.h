// GameObject.h
#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <SFML/Graphics.hpp>

class GameObject {
public:
    virtual void draw(sf::RenderWindow& window) = 0;  // Pure virtual function for drawing
    virtual void update() = 0;  // Pure virtual function for updating
    virtual ~GameObject() {}  // Virtual destructor for proper cleanup of derived classes
};

#endif
