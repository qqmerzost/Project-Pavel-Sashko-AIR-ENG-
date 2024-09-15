#include<SFML/Graphics.hpp>
#include<time.h>
#include<vector>
#include<memory> 

using namespace sf;

// Base class for all game objects 
class GameObject {
public:
    virtual void draw(RenderWindow& window) = 0;  
    virtual void update() = 0;  
    virtual ~GameObject() {}  
};

// Class for the player
class Player : public GameObject {
public:
    Sprite sprite;
    Texture texture;
    int x, y;
    float dx, dy;
    const int windowHeight = 533;
    const int windowWidth = 400;

    Player() {
        texture.loadFromFile("C:/Users/sosko/Downloads/doodle.png");
        sprite.setTexture(texture);
        x = 100;
        y = 100;
        dx = 0;
        dy = 0;
    }

    void update() override {
        // Handle player movement
        if (Keyboard::isKeyPressed(Keyboard::Right)) x += 3;
        if (Keyboard::isKeyPressed(Keyboard::Left)) x -= 3;

        // Gravity
        dy += 0.2f;  
        y += dy;

        // Wrap around screen horizontally
        if (x > windowWidth) x = 0;
        if (x < 0) x = windowWidth;

        // Reset if player falls below screen
        if (y > windowHeight) {
            y = 100;  
            dy = 0;
        }
    }

    void draw(RenderWindow& window) override {
        sprite.setPosition(x, y);
        window.draw(sprite);
    }

    // Check collision with platforms only when falling (dy > 0)
    bool isOnPlatform(FloatRect platformBounds) {
        FloatRect playerBounds = sprite.getGlobalBounds();

        
        return (dy > 0 &&
            playerBounds.left + playerBounds.width > platformBounds.left &&
            playerBounds.left < platformBounds.left + platformBounds.width &&
            playerBounds.top + playerBounds.height > platformBounds.top &&
            playerBounds.top + playerBounds.height < platformBounds.top + 14);
    }

    void bounce() {
        dy = -10;  // Bounce upwards
    }
};

// Class for the platform
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

    void update() override {
        // Platforms don't move by themselves, but can be shifted with the camera
    }

    void draw(RenderWindow& window) override {
        sprite.setPosition(x, y);
        window.draw(sprite);
    }

    // Respawn platform at the top of the screen when it falls below the bottom
    void respawn() {
        x = rand() % 400;
        y = 0;
    }

    FloatRect getBounds() const {
        return sprite.getGlobalBounds();
    }
};

int main()
{
    srand(time(0));

    RenderWindow app(VideoMode(400, 533), "Doodle Jump with Polymorphism");
    app.setFramerateLimit(60);

    // Load background texture
    Texture backgroundTexture;
    backgroundTexture.loadFromFile("C:/Users/sosko/Downloads/background.png");
    Sprite backgroundSprite(backgroundTexture);

    // Container for all game objects (player and platforms)
    std::vector<std::unique_ptr<GameObject>> gameObjects;

    // Add player to the game object container
    Player* player = new Player();
    gameObjects.push_back(std::unique_ptr<GameObject>(player));

    // Add platforms to the game object container
    std::vector<Platform*> platforms;  /
    for (int i = 0; i < 10; i++) {
        Platform* platform = new Platform();
        gameObjects.push_back(std::unique_ptr<GameObject>(platform));
        platforms.push_back(platform);
    }

    while (app.isOpen())
    {
        Event e;
        while (app.pollEvent(e)) {
            if (e.type == Event::Closed)
                app.close();
        }

        
        for (auto& obj : gameObjects) {
            obj->update();  
        }

        // Check for player-platform collisions and make the player bounce
        for (auto& platform : platforms) {
            if (player->isOnPlatform(platform->getBounds())) {
                player->bounce();  // Bounce the player if they are on a platform
            }
        }

        // Camera movement and platform respawn
        if (player->y < 200) {
            // Shift the platforms down as the player goes up, simulating the camera following the player
            float shiftAmount = 200 - player->y;
            for (auto& platform : platforms) {
                platform->y += shiftAmount;  
                if (platform->y > 533) {
                    platform->respawn();  
                }
            }
            backgroundSprite.setPosition(0, backgroundSprite.getPosition().y + shiftAmount);  // Move background down
            player->y = 200;  // Keep player at fixed camera height while the world moves down
        }

        // Draw everything
        app.clear();
        app.draw(backgroundSprite);  // Draw background
        for (auto& obj : gameObjects) {
            obj->draw(app);  // Polymorphic call to draw method
        }
        app.display();
    }

    return 0;
}
