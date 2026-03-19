#pragma once
#include <SFML/Graphics.hpp>
#include "Gate.h"

class Player
{
private:
    sf::RectangleShape shape;
    sf::RectangleShape spriteShape;
    sf::Vector2f m_spawnPoint;
    std::optional<sf::Sprite> sprite;
    std::optional<sf::Sprite> waveSprite;
    sf::IntRect mage;
    sf::IntRect robot;
    bool theme = false; // false -> SF, true -> fantasy
    float velocityY;
    bool onGround;
    bool atCeiling;
    float speed;
    float waveSpeed = 550.f;

public:
    Player();
    void changeTheme();
    void update(float dt, bool jumpPressed);
    void jump();
    void draw(sf::RenderWindow& window, bool showhitbox);
    void reset();
    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;
    void setPosition(sf::Vector2f pos);
    void move(sf::Vector2f pos);
    void setMode(Type newMode);
    void setVelocityY(float v) { velocityY = v; }
    float getVelocityY() const { return velocityY; }
    void setOnGround(bool ground) { onGround = ground; }
    bool isOnGround() const { return onGround; }
    void setAtCeiling(bool ceiling) { atCeiling = ceiling; }
    bool isAtCeiling() const { return atCeiling; }
    float getSpeed() const { return speed; }
    sf::Vector2f getSize() const;
    void setSpawnPoint(sf::Vector2f pos) { m_spawnPoint = pos; }
    sf::Vector2f getNextMove(float dt);
    void transformBall();
    void transformShip();
    void transformCube();
    void transformWave();

    std::optional<sf::Sprite> transformSprite;
    sf::IntRect shipSF;
    sf::IntRect ballSF;
    sf::IntRect waveSF;
    sf::IntRect shipF;
    sf::IntRect ballF;
    sf::IntRect waveF;
    Type mode = Type::CUBE;
    bool canJump = true; // vrai si le joueur peut sauter
    bool canDoubleJump = false; // false par défaut
    bool wasJumpPressed = false;
    bool gravityInverted = false; // false = normal, true = inversé
    float gravity = 4700.f;
    float ceilingY = 0.f;
    float jumpForce = 1050.f;
    float thrust = 2000.f;
};