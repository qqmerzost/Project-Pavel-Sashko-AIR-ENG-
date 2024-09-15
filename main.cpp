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

int main()
{
    srand(time(0));

    RenderWindow app(VideoMode(400, 533), "Doodle Jump with Points and Coins");
    app.setFramerateLimit(60);

    // Load background texture
    Texture backgroundTexture;
    backgroundTexture.loadFromFile("C:/Users/sosko/Downloads/background.png");

    // Create two background sprites for seamless scrolling
    Sprite backgroundSprite1(backgroundTexture);
    Sprite backgroundSprite2(backgroundTexture);
    backgroundSprite2.setPosition(0, -533);  // Position the second background above the first

    // Load font for displaying the score and coin count
    Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/Arial.ttf")) {
        return -1;  // Handle font loading error
    }

    // Create a text object for displaying the score
    Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(Color::Black);  // Set text color to black
    scoreText.setPosition(10, 10);  // Position the score text at the top-left corner

    // Create a text object for displaying the coin count
    Text coinText;
    coinText.setFont(font);
    coinText.setCharacterSize(24);
    coinText.setFillColor(Color::Black);  // Set text color to black
    coinText.setPosition(10, 40);  // Position the coin count text below the score

    // Create game over text to display final score and coins
    Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(24);  // Reduced size so the full message fits
    gameOverText.setFillColor(Color::Red);

    int points = 0;  // Track the player's points (based on how high they go)
    int coinsCollected = 0;  // Track the number of collected coins
    bool gameOver = false;  // Flag to track if the game is over

    float cumulativeShift = 0;  // Track how much the camera (world) has shifted
    int platformCounter = 0;  // Counter to track platforms and spawn coins every 20th platform

    // Container for all game objects (player, platforms, coins)
    std::vector<std::unique_ptr<GameObject>> gameObjects;

    // Start player at a fixed Y position
    int playerStartY = 400;
    Player* player = new Player(playerStartY);  // Spawn player at a fixed height
    gameObjects.push_back(std::unique_ptr<GameObject>(player));

    // Add platforms to the game object container
    std::vector<Platform*> platforms;  // Keep track of platforms for collision detection

    // Place the first platform just below the player
    Platform* firstPlatform = new Platform();
    firstPlatform->y = playerStartY + 50;  // 50 pixels below the player
    platforms.push_back(firstPlatform);
    gameObjects.push_back(std::unique_ptr<GameObject>(firstPlatform));

    // Add the rest of the platforms randomly
    for (int i = 1; i < 10; i++) {
        Platform* platform = new Platform();
        gameObjects.push_back(std::unique_ptr<GameObject>(platform));
        platforms.push_back(platform);
    }

    // Add a coin to the game
    Coin* coin = new Coin();
    gameObjects.push_back(std::unique_ptr<GameObject>(coin));

    while (app.isOpen())
    {
        Event e;
        while (app.pollEvent(e)) {
            if (e.type == Event::Closed)
                app.close();
        }

        if (!gameOver) {
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

            // Check if player collects the coin
            if (player->isCollectingCoin(coin->getBounds())) {
                coinsCollected++;  // Increment the coin count
                coin->respawnAtPlatform(rand() % 400, rand() % 533);  // Respawn the coin after collection
            }

            // Simulate camera movement and platform respawn
            if (player->y < 200) {
                // Shift the platforms and coins down as the player goes up
                float shiftAmount = 200 - player->y;
                cumulativeShift += shiftAmount;  // Track the total camera shift

                for (auto& platform : platforms) {
                    platform->y += shiftAmount;  // Move platforms down when player moves up
                    if (platform->y > 533) {
                        platformCounter++;  // Increment the platform counter
                        platform->respawn();  // Respawn platform if it moves off the bottom

                        // Every 20th platform spawns a coin
                        if (platformCounter % 20 == 0) {
                            coin->respawnAtPlatform(platform->x, platform->y);
                        }
                    }
                }

                // Move the coin as well
                coin->y += shiftAmount;
                if (coin->y > 533) {
                    coin->respawnAtPlatform(rand() % 400, rand() % 533);  // Respawn the coin if it moves off the bottom
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

                // Update points based on the cumulative shift
                points = static_cast<int>(cumulativeShift);  // Set score based on cumulative shift
            }

            // Check if player has fallen off the screen
            if (player->y > player->windowHeight) {
                gameOver = true;  // Set game over flag to true

                // Display final score and coins collected in the game-over text
                std::stringstream gameOverStream;
                gameOverStream << "Game Over!\nFinal Score: " << points << "\nCoins Collected: " << coinsCollected;
                gameOverText.setString(gameOverStream.str());

                // Center the game over text
                FloatRect textRect = gameOverText.getLocalBounds();
                gameOverText.setOrigin(textRect.width / 2, textRect.height / 2);
                gameOverText.setPosition(app.getSize().x / 2, app.getSize().y / 2);
            }

            // Update the score and coin count text
            std::stringstream ss;
            ss << "Score: " << points;
            scoreText.setString(ss.str());

            std::stringstream coinStream;
            coinStream << "Coins: " << coinsCollected;
            coinText.setString(coinStream.str());
        }

        // Draw everything
        app.clear();
        if (!gameOver) {
            // Draw both background sprites to create the looping effect
            app.draw(backgroundSprite1);
            app.draw(backgroundSprite2);

            for (auto& obj : gameObjects) {
                obj->draw(app);  // Polymorphic call to draw method
            }

            // Draw the score and coin count
            app.draw(scoreText);
            app.draw(coinText);
        }
        else {
            // Draw the final score and coins collected after game over
            app.draw(gameOverText);
        }

        app.display();
    }

    return 0;
}
