#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>

using namespace std;
using namespace sf;

// Define constants
const float playerSpeed = 6.0f;
const float bulletSpeed = 8.0f;
const float enemySpeed = 3.0f;
const int maxBullets = 5;
const int windowWidth = 800;
const int windowHeight = 600;

// Function to load the high score from a file
int loadHighScore() {
    ifstream file("score.txt");
    int highScore = 0;
    if (file.is_open()) {
        file >> highScore;
        file.close();
    }
    return highScore;
}

// Function to save the high score to a file
void saveHighScore(int highScore) {
    ofstream file("score.txt");
    if (file.is_open()) {
        file << highScore;
        file.close();
    }
}

int main() {
    // Create the window for the start screen
    RenderWindow startWindow(VideoMode(windowWidth, windowHeight), "Space Shooter - Start");
    startWindow.setVerticalSyncEnabled(true);

    // Load a cute background texture
    Texture startBackgroundTexture;
    if (!startBackgroundTexture.loadFromFile("start.jpg")) {
        return 0;
    }

    // Create a sprite for the cute background
    Sprite startBackground(startBackgroundTexture);

    // Create font for start screen text
    Font startFont;
    startFont.loadFromFile("arial.ttf");

    // Create start screen text
    Text startText;
    startText.setFont(startFont);
    startText.setCharacterSize(24);
    startText.setFillColor(Color::White);
    startText.setString("Welcome to Space Shooter!");
    startText.setPosition((windowWidth / 2) - 160, (windowHeight / 2) - 50);

    Text keyText;
    keyText.setFont(startFont);
    keyText.setCharacterSize(24);
    keyText.setFillColor(Color::White);
    keyText.setString("Press any key to start");
    keyText.setPosition((windowWidth / 2) - 125, (windowHeight / 2) - 25);

    Text devText;
    devText.setFont(startFont);
    devText.setCharacterSize(24);
    devText.setFillColor(Color::White);
    devText.setString("Developed by Anoosha and Ansa");
    devText.setPosition((windowWidth / 2) - 180, (windowHeight / 2));

    // Display the start window
    while (startWindow.isOpen()) {
        Event startEvent;
        while (startWindow.pollEvent(startEvent)) {
            if (startEvent.type == Event::Closed)
                startWindow.close();
            else if (startEvent.type == Event::KeyPressed) {
                // Close the start window when any key is pressed
                startWindow.close();
            }
        }

        // Clear and draw the start window things
        startWindow.clear();
        startWindow.draw(startBackground);
        startWindow.draw(startText);
        startWindow.draw(keyText);
        startWindow.draw(devText);
        startWindow.display();
    }

    int highScore = loadHighScore(); // Load the high score

    // Create the high score window
    RenderWindow highScoreWindow(VideoMode(windowWidth, windowHeight), "High Score");
    highScoreWindow.setVerticalSyncEnabled(true);

    // Create font for high score text
    Font highScoreFont;
    highScoreFont.loadFromFile("arial.ttf");

    // Create high score text
    Text highScoreText;
    highScoreText.setFont(highScoreFont);
    highScoreText.setCharacterSize(36);
    highScoreText.setFillColor(Color::White);
    highScoreText.setString("High Score: " + to_string(highScore));
    highScoreText.setPosition((windowWidth / 2) - 110, (windowHeight / 2) - 50);

    while (highScoreWindow.isOpen()) {
        Event highScoreEvent;
        while (highScoreWindow.pollEvent(highScoreEvent)) {
            if (highScoreEvent.type == Event::Closed) {
                highScoreWindow.close();
            }
            else if (highScoreEvent.type == Event::KeyPressed) {
                highScoreWindow.close(); // Close the high score window on any key press
            }
        }

        highScoreWindow.clear();
        highScoreWindow.draw(highScoreText);
        highScoreWindow.display();
    }

    // Moving on to the main game window

    //create the main game window
    RenderWindow window(VideoMode(windowWidth, windowHeight), "Space Shooter");
    window.setVerticalSyncEnabled(true);

    // Load textures
    Texture backgroundTexture, playerTexture, bulletTexture, enemyTexture;
    if (!backgroundTexture.loadFromFile("background.jpg") ||
        !playerTexture.loadFromFile("player.png") ||
        !bulletTexture.loadFromFile("bullet.png") ||
        !enemyTexture.loadFromFile("enemy.png")) {
        return 0;
    }

    // Create sprites using the loaded textures
    Sprite background(backgroundTexture);
    Sprite player(playerTexture);
    Sprite bullet(bulletTexture);
    Sprite enemy(enemyTexture);

    label:
    // Create bullets
    vector<Sprite> bullets;
    bullets.resize(maxBullets);
    for (auto& bullet : bullets) {
        bullet.setTexture(bulletTexture);
        bullet.setPosition(-50, -50); // set initial position off-screen
    }

    // Set initial positions
    player.setPosition(windowWidth / 2 - 30, windowHeight - 70);
    enemy.setPosition(rand() % (windowWidth - 50), 0);

    // Create font for text display
    Font font;
    font.loadFromFile("arial.ttf");

    // create score text
    Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(10, 10);

    // create lives text
    Text livesText;
    livesText.setFont(font);
    livesText.setCharacterSize(24);
    livesText.setFillColor(Color::White);
    livesText.setPosition(700, 10);

    // Load sound buffers for each sound effect
    SoundBuffer bulletHitBuffer, playerHitBuffer, gameOverBuffer;

    if (!bulletHitBuffer.loadFromFile("bullethit.wav") ||
        !playerHitBuffer.loadFromFile("playerhit.wav") ||
        !gameOverBuffer.loadFromFile("gameoversound.wav")) {
        return 0;
    }

    Sound bulletHit(bulletHitBuffer);
    Sound playerHit(playerHitBuffer);
    Sound gameOverSound(gameOverBuffer);

    int score = 0;
    int lives = 3;
    string str = to_string(score);
    string livesstr = to_string(lives);
    // actual game
    while (window.isOpen()) {
        scoreText.setString("Score: " + str);
        livesText.setString("Lives: " + livesstr);

        // handle events
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
        }

        // Handle player input
        if (Keyboard::isKeyPressed(Keyboard::Left) && player.getPosition().x > 0)
            player.move(-playerSpeed, 0);
        if (Keyboard::isKeyPressed(Keyboard::Right) && player.getPosition().x < windowWidth - 70)
            player.move(playerSpeed, 0);

        // move bullets and all other bullet functionalities
        for (auto& bullet : bullets) {
            if (bullet.getPosition().y > 0) {
                bullet.move(0, -bulletSpeed);
                // Check for collision with enemy
                if (bullet.getGlobalBounds().intersects(enemy.getGlobalBounds())) {
                    enemy.setPosition(rand() % (windowWidth - 50), 0);
                    bullet.setPosition(-50, -50); // Move the bullet out of the screen

                    // Increase score and update score text
                    score++;
                    str = to_string(score);
                    scoreText.setString("Score: " + str);

                    // Update high score if needed
                    if (score > highScore) {
                        highScore = score;
                        saveHighScore(highScore);
                    }
                    bulletHit.play();
                }
            }
        }

        // handle enemy position
        enemy.move(0, enemySpeed);
        if (enemy.getPosition().y > windowHeight) {
            enemy.setPosition(rand() % (windowWidth - 50), 0);
        }

        // handling enemy and player collision
        if (enemy.getGlobalBounds().intersects(player.getGlobalBounds())) {
            lives--;
            livesstr = to_string(lives);
            livesText.setString("Lives: " + livesstr);
            playerHit.play();

            if (lives == 0) {
                // Display Game Over window
                RenderWindow gameOverWindow(VideoMode(windowWidth, windowHeight), "Game Over");
                gameOverWindow.setVerticalSyncEnabled(true);

                Texture gameOverTexture;
                if (!gameOverTexture.loadFromFile("gameover.png")) {
                    return 0;
                }
                Sprite gameOver(gameOverTexture);

                gameOverSound.play();

                while (gameOverWindow.isOpen()) {
                    Event gameOverEvent;
                    while (gameOverWindow.pollEvent(gameOverEvent)) {
                        if (gameOverEvent.type == Event::Closed) {
                            gameOverWindow.close();
                            window.close(); // Close the main game window as well
                        }
                        else if (Keyboard::isKeyPressed(Keyboard::R)) {
                                gameOverWindow.close(); // Close the game over window
                                // Restart the game
                                goto label;
                        }
                    }

                    gameOverWindow.clear();
                    gameOverWindow.draw(gameOver);
                    gameOverWindow.display();
                }
            }
            else {
                // Player still has lives, reposition enemy and player
                enemy.setPosition(rand() % (windowWidth - 50), 0);
                player.setPosition(windowWidth / 2 - 30, windowHeight - 70);
            }
        }

        // fire bullets
        for (auto& bullet : bullets) {
            if (Keyboard::isKeyPressed(Keyboard::Space) && bullet.getPosition().y <= 0) {
                bullet.setPosition(player.getPosition().x, player.getPosition().y);
            }
        }

        // Clear the window, draw all things, etc
        window.clear();
        window.draw(background);
        window.draw(player);
        window.draw(enemy);
        for (auto& bullet : bullets) {
            window.draw(bullet);
        }
        window.draw(scoreText);
        window.draw(livesText);
        window.display();

    }
    return 0;
}