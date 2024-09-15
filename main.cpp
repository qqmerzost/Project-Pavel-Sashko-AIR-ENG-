#include<SFML/Graphics.hpp>
#include<time.h>
#include<vector>
#include<memory> 
#include<sstream>  

using namespace sf;

// Base class for all game objects 
class GameObject {
public:
    virtual void draw(RenderWindow& window) = 0;  
    virtual void update() = 0;  
    virtual ~GameObject() {}  
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

    Player() {
        texture.loadFromFile("C:/Users/sosko/Downloads/doodle.png");
        sprite.setTexture(texture);
        x = 100;
        y = 400;  
        dx = 0;
        dy = 0;
        highestY = y;  
    }

    void update() override {
        // Handle player movement
        if (Keyboard::isKeyPressed(Keyboard::Right)) x += 3;
        if (Keyboard::isKeyPressed(Keyboard::Left)) x -= 3;

        // Apply gravity
        dy += 0.2f;  
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

    void bounce() {
        dy = -10;  
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

    RenderWindow app(VideoMode(400, 533), "Doodle Jump with Points System");
    app.setFramerateLimit(60);

    // Load background texture
    Texture backgroundTexture;
    backgroundTexture.loadFromFile("C:/Users/sosko/Downloads/background.png");

    // Create two background sprites for seamless scrolling
    Sprite backgroundSprite1(backgroundTexture);
    Sprite backgroundSprite2(backgroundTexture);
    backgroundSprite2.setPosition(0, -533);  // Position the second background above the first

    // Load font for displaying the score
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

    // Create game over text to display final score
    Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(24);  // Reduced size so the full message fits
    gameOverText.setFillColor(Color::Red);

    int points = 0;  // Track the player's points (based on how high they go)
    bool gameOver = false;  // Flag to track if the game is over

    // Container for all game objects (player and platforms)
    std::vector<std::unique_ptr<GameObject>> gameObjects;

    // Add player to the game object container
    Player* player = new Player();
    gameObjects.push_back(std::unique_ptr<GameObject>(player));

    // Add platforms to the game object container
    std::vector<Platform*> platforms;  // Keep track of platforms for collision detection
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

            // Simulate camera movement and platform respawn
            if (player->y < 200) {
                // Shift the platforms down as the player goes up, simulating the camera following the player
                float shiftAmount = 200 - player->y;
                for (auto& platform : platforms) {
                    platform->y += shiftAmount;  // Move platforms down when player moves up
                    if (platform->y > 533) {
                        platform->respawn();  // Respawn platform if it moves off the bottom
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

                // Update points based on the player's highest Y position
                points = abs(400 - player->highestY);  // Calculate score based on how high player moves
            }

            // Check if player has fallen off the screen
            if (player->y > player->windowHeight) {
                gameOver = true;  // Set game over flag to true
                std::stringstream gameOverStream;
                gameOverStream << "Game Over! Final Score: " << points;
                gameOverText.setString(gameOverStream.str());

                // Center the game over text
                FloatRect textRect = gameOverText.getLocalBounds();
                gameOverText.setOrigin(textRect.width / 2, textRect.height / 2);
                gameOverText.setPosition(app.getSize().x / 2, app.getSize().y / 2);
            }

            // Update the score text
            std::stringstream ss;
            ss << "Score: " << points;
            scoreText.setString(ss.str());
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

            // Draw the score
            app.draw(scoreText);
        }
        else {
            // Draw the final score after game over
            app.draw(gameOverText);
        }

        app.display();
    }

    return 0;
}
