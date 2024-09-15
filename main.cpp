#include<SFML/Graphics.hpp>
#include<time.h>
#include<vector>
#include<memory> // For smart pointers
#include<sstream>  // For converting score to string

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

    int highestY;  // Track the highest point the player has reached

    Player(int startY) {  // Start with a custom Y position
        texture.loadFromFile("C:/Users/sosko/Downloads/doodle.png");
        sprite.setTexture(texture);
        x = 100;
        y = startY;  // Start player slightly above the first platform
        dx = 0;
        dy = 0;
        highestY = y;  // Initialize highestY to the starting position
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

        // Update the highestY only if the player moves higher
        if (y < highestY) {
            highestY = y;
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

    bool isCollectingCoin(FloatRect coinBounds) {
        FloatRect playerBounds = sprite.getGlobalBounds();
        return playerBounds.intersects(coinBounds);
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

// Derived class for the coin
class Coin : public GameObject {
public:
    Sprite sprite;
    Texture texture;
    int x, y;

    Coin() {
        texture.loadFromFile("C:/Users/sosko/Downloads/coin.png");
        sprite.setTexture(texture);
        sprite.setScale(0.1f, 0.1f);  // Scale down the coin to make it smaller
        x = rand() % 400;
        y = rand() % 533;
    }

    void update() override {
        // Coins don't move by themselves, but can be respawned when collected
    }

    void draw(RenderWindow& window) override {
        sprite.setPosition(x, y);
        window.draw(sprite);
    }

    // Respawn the coin at the given platform's position
    void respawnAtPlatform(int platformX, int platformY) {
        x = platformX + rand() % 50 - 25;  // Randomize slightly around the platform's X position
        y = platformY - 30;  // Spawn above the platform
    }

    FloatRect getBounds() const {
        return sprite.getGlobalBounds();
    }
};

// Main Game Class
class Game {
private:
    RenderWindow app;
    Texture backgroundTexture;
    Sprite backgroundSprite1, backgroundSprite2;
    Font font;
    Text scoreText, coinText, gameOverText;
    int points = 0;
    int coinsCollected = 0;
    bool gameOver = false;
    float cumulativeShift = 0;
    int platformCounter = 0;

    Player* player;
    Coin* coin;
    std::vector<Platform*> platforms;
    std::vector<std::unique_ptr<GameObject>> gameObjects;

public:
    Game() : app(VideoMode(400, 533), "Doodle Jump with Points and Coins") {
        app.setFramerateLimit(60);

        // Load background texture
        backgroundTexture.loadFromFile("C:/Users/sosko/Downloads/background.png");
        backgroundSprite1.setTexture(backgroundTexture);
        backgroundSprite2.setTexture(backgroundTexture);
        backgroundSprite2.setPosition(0, -533);

        // Load font
        font.loadFromFile("C:/Windows/Fonts/Arial.ttf");

        // Initialize texts
        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(Color::Black);
        scoreText.setPosition(10, 10);

        coinText.setFont(font);
        coinText.setCharacterSize(24);
        coinText.setFillColor(Color::Black);
        coinText.setPosition(10, 40);

        gameOverText.setFont(font);
        gameOverText.setCharacterSize(24);
        gameOverText.setFillColor(Color::Red);

        // Initialize player and coin
        player = new Player(400);
        gameObjects.push_back(std::unique_ptr<GameObject>(player));

        coin = new Coin();
        gameObjects.push_back(std::unique_ptr<GameObject>(coin));

        // Initialize platforms
        Platform* firstPlatform = new Platform();
        firstPlatform->y = player->y + 50;  // Spawn first platform just below the player
        platforms.push_back(firstPlatform);
        gameObjects.push_back(std::unique_ptr<GameObject>(firstPlatform));

        for (int i = 1; i < 10; i++) {
            Platform* platform = new Platform();
            platforms.push_back(platform);
            gameObjects.push_back(std::unique_ptr<GameObject>(platform));
        }
    }

    void run() {
        while (app.isOpen()) {
            handleEvents();
            if (!gameOver) {
                update();
            }
            render();
        }
    }

private:
    void handleEvents() {
        Event e;
        while (app.pollEvent(e)) {
            if (e.type == Event::Closed) {
                app.close();
            }
        }
    }

    void update() {
        // Update all game objects
        for (auto& obj : gameObjects) {
            obj->update();
        }

        // Check for player-platform collisions
        for (auto& platform : platforms) {
            if (player->isOnPlatform(platform->getBounds())) {
                player->bounce();
            }
        }

        // Check for coin collection
        if (player->isCollectingCoin(coin->getBounds())) {
            coinsCollected++;
            coin->respawnAtPlatform(rand() % 400, rand() % 533);
        }

        // Simulate camera movement
        if (player->y < 200) {
            float shiftAmount = 200 - player->y;
            cumulativeShift += shiftAmount;

            for (auto& platform : platforms) {
                platform->y += shiftAmount;
                if (platform->y > 533) {
                    platformCounter++;
                    platform->respawn();
                    if (platformCounter % 20 == 0) {
                        coin->respawnAtPlatform(platform->x, platform->y);
                    }
                }
            }

            coin->y += shiftAmount;
            if (coin->y > 533) {
                coin->respawnAtPlatform(rand() % 400, rand() % 533);
            }

            backgroundSprite1.move(0, shiftAmount);
            backgroundSprite2.move(0, shiftAmount);

            if (backgroundSprite1.getPosition().y >= 533) {
                backgroundSprite1.setPosition(0, backgroundSprite2.getPosition().y - 533);
            }
            if (backgroundSprite2.getPosition().y >= 533) {
                backgroundSprite2.setPosition(0, backgroundSprite1.getPosition().y - 533);
            }

            player->y = 200;
            points = static_cast<int>(cumulativeShift);
        }

        // Check for game over
        if (player->y > player->windowHeight) {
            gameOver = true;
            std::stringstream gameOverStream;
            gameOverStream << "Game Over!\nFinal Score: " << points << "\nCoins Collected: " << coinsCollected;
            gameOverText.setString(gameOverStream.str());
            FloatRect textRect = gameOverText.getLocalBounds();
            gameOverText.setOrigin(textRect.width / 2, textRect.height / 2);
            gameOverText.setPosition(app.getSize().x / 2, app.getSize().y / 2);
        }

        // Update score and coin text
        std::stringstream ss;
        ss << "Score: " << points;
        scoreText.setString(ss.str());

        std::stringstream coinStream;
        coinStream << "Coins: " << coinsCollected;
        coinText.setString(coinStream.str());
    }

    void render() {
        app.clear();

        if (!gameOver) {
            app.draw(backgroundSprite1);
            app.draw(backgroundSprite2);

            for (auto& obj : gameObjects) {
                obj->draw(app);
            }

            app.draw(scoreText);
            app.draw(coinText);
        }
        else {
            app.draw(gameOverText);
        }

        app.display();
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}
