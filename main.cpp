#include<SFML/Graphics.hpp>
#include<time.h>
#include<iostream>

using namespace sf;

// Base class for game objects
class GameObject {
public:
    virtual void draw(RenderWindow& window) = 0; 
    virtual void update() = 0; 
};

// Class for a platform
class Platform : public GameObject {
public:
    Sprite sprite;
    Texture texture;
    int x, y;

    Platform() {
        texture.loadFromFile("C:/Users/sosko/Downloads/platform.png");
        sprite.setTexture(texture);
        x = rand() % 400;
        y = rand() % 533;
    }

    void draw(RenderWindow& window) override {
        sprite.setPosition(x, y);
        window.draw(sprite);
    }

    void update() override {
        
    }
};

// Class for the player
class Player : public GameObject {
public:
    Sprite sprite;
    Texture texture;
    int x, y;
    float dx, dy;

    Player() {
        texture.loadFromFile("C:/Users/sosko/Downloads/doodle.png");
        sprite.setTexture(texture);
        x = 100;
        y = 100;
        dx = 0;
        dy = 0;
    }

    void draw(RenderWindow& window) override {
        sprite.setPosition(x, y);
        window.draw(sprite);
    }

    void update() override {
        dy += 0.2;
        y += dy;
        if (y > 500) dy = -10;
    }
};

int main() {
    srand(time(0));

    RenderWindow app(VideoMode(400, 533), "Start Doodle Game");
    app.setFramerateLimit(60);

    Texture t1;
    t1.loadFromFile("C:/Users/sosko/Downloads/background.png");

    Sprite sBackground(t1);

    Platform platforms[10];
    Player player;

    while (app.isOpen()) {
        Event e;
        while (app.pollEvent(e)) {
            if (e.type == Event::Closed)
                app.close();
        }

        if (Keyboard::isKeyPressed(Keyboard::Right)) player.x += 3;
        if (Keyboard::isKeyPressed(Keyboard::Left)) player.x -= 3;

        player.update();

        for (int i = 0; i < 10; i++) {
            platforms[i].update();
        }

        app.draw(sBackground);
        player.draw(app);

        for (int i = 0; i < 10; i++) {
            platforms[i].draw(app);
        }

        app.display();
    }

    return 0;
}
