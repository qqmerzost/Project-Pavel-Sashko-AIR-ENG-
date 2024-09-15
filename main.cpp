#include<SFML/Graphics.hpp>
#include<time.h>
#include<vector>
#include<memory> // For smart pointers

using namespace sf;

// Base class for all game objects (using polymorphism)
class GameObject {
public:
    virtual void draw(RenderWindow& window) = 0;  // Pure virtual function for drawing
    virtual void update() = 0;  // Pure virtual function for updating
    virtual ~GameObject() {}  // Virtual destructor for proper cleanup of derived classes
};

// Derived class for the player
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

        // Apply gravity
        dy += 0.2f;  // Gravity makes the player fall
        y += dy;

        // Wrap around screen horizontally
        if (x > windowWidth) x = 0;
        if (x < 0) x = windowWidth;

        // Reset if player falls below screen
        if (y > windowHeight) {
            y = 100;  // Reset player
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

        // Only check collision when the player is falling downwards (dy > 0)
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

// Derived class for the platform
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

    // Create two background sprites to create the endless scrolling effect
    Sprite backgroundSprite1(backgroundTexture);
    Sprite backgroundSprite2(backgroundTexture);
    backgroundSprite2.setPosition(0, -533);  // Position the second background above the first

    
    std::vector<std::unique_ptr<GameObject>> gameObjects;

    // Add player to the game object container
    Player* player = new Player();
    gameObjects.push_back(std::unique_ptr<GameObject>(player));

    // Add platforms to the game object container
    std::vector<Platform*> platforms;  
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

        // Update all game objects
        for (auto& obj : gameObjects) {
            obj->update();  // Polymorphic call to update method
        }

        // Check for player-platform collisions and make the player bounce
        for (auto& platform : platforms) {
            if (player->isOnPlatform(platform->getBounds())) {
                player->bounce();  // Bounce the player if they are on a platform
            }
        }

        // Simulate camera movement and platform respawn
        if (player->y < 200) {
            // Shift the platforms down as the player goes up, simulating the camera following the player
            float shiftAmount = 200 - player->y;
            for (auto& platform : platforms) {
                platform->y += shiftAmount;  
                if (platform->y > 533) {
                    platform->respawn();  
                }
            }

            // Move the backgrounds down to simulate endless scrolling
            backgroundSprite1.move(0, shiftAmount);
            backgroundSprite2.move(0, shiftAmount);

            // If a background moves completely off-screen, reposition it above the other
            if (backgroundSprite1.getPosition().y >= 533) {
                backgroundSprite1.setPosition(0, backgroundSprite2.getPosition().y - 533);
            }
            if (backgroundSprite2.getPosition().y >= 533) {
                backgroundSprite2.setPosition(0, backgroundSprite1.getPosition().y - 533);
            }

            player->y = 200;  // Keep player at fixed camera height while the world moves down
        }

        // Draw everything
        app.clear();
        // Draw both background sprites to create the looping effect
        app.draw(backgroundSprite1);
        app.draw(backgroundSprite2);

        for (auto& obj : gameObjects) {
            obj->draw(app);  // Polymorphic call to draw method
        }
        app.display();
    }

    return 0;
}
