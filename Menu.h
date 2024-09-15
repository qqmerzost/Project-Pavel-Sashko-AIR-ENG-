#pragma once
#include <SFML/Graphics.hpp>

class Menu {
public:
    Menu();
    void draw(sf::RenderWindow& window);
    bool isStartClicked(sf::Event event);
private:
    sf::Font font;
    sf::Text startText;
    sf::RectangleShape startButton;
};
