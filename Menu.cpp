#include "Menu.h"

Menu::Menu() {
    if (!font.loadFromFile("C:/Users/sosko/Downloads/arial.ttf")) {
        throw std::runtime_error("Failed to load font");
    }

    startText.setFont(font);
    startText.setString("Start");
    startText.setCharacterSize(24);
    startText.setFillColor(sf::Color::White);
    startText.setStyle(sf::Text::Bold);

    startButton.setSize(sf::Vector2f(200, 50));
    startButton.setFillColor(sf::Color(100, 100, 250));
    startButton.setOutlineColor(sf::Color::White);
    startButton.setOutlineThickness(3);
    startButton.setPosition(300, 300); // Center on the screen

    startText.setPosition(
        startButton.getPosition().x + (startButton.getSize().x - startText.getLocalBounds().width) / 2,
        startButton.getPosition().y + (startButton.getSize().y - startText.getLocalBounds().height) / 2 - 5
    );
}

void Menu::draw(sf::RenderWindow& window) {
    window.draw(startButton);
    window.draw(startText);
}

bool Menu::isStartClicked(sf::Event event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        if (startButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
            return true;
        }
    }
    return false;
}
