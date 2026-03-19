#include <cmath>
#include "Player.h"
#include "Engine/AssetManager.h"

Player::Player()
{
    shape.setSize({ 40,40 });
    spriteShape.setSize({ 40,40 });
    shape.setOrigin({ 20,40 });
    spriteShape.setOrigin({ 20,40 });
    shape.setFillColor(sf::Color::Transparent);
    shape.setOutlineColor(sf::Color::Green);
    shape.setOutlineThickness(1);

    mage = sf::IntRect({ 64, 64 }, { 64, 64 });
    robot = sf::IntRect({ 127, 64 }, { 62, 64 });
    ballF = sf::IntRect({ 8, 50 }, { 44, 47 });
    ballSF = sf::IntRect({ 141, 47 }, { 56, 60 });
    shipSF = sf::IntRect({ 3, 7 }, { 53, 29 });
    shipF = sf::IntRect({ 133, 6 }, { 83, 38 });
    waveF = sf::IntRect({ 27, 81 }, { 60, 39 });
    waveSF = sf::IntRect({ 31, 20 }, { 43, 37 });
    sprite.emplace(AssetManager::getTexture("player"));
    waveSprite.emplace(AssetManager::getTexture("waves"));
    transformSprite.emplace(AssetManager::getTexture("tranformations"));
    sprite->setOrigin({ 32, 32 });
    sprite->setTextureRect(mage);
    sprite->setPosition(getPosition());

    velocityY = 0;
    onGround = true;
    atCeiling = false;
    speed = 500.f;
}

void Player::transformBall()
{
    mode = Type::BALL;
    if (theme)
    {
        transformSprite->setTextureRect(ballSF);
        transformSprite->setOrigin({ 27, 27 });
    }
    else
    {
        transformSprite->setTextureRect(ballF);
        transformSprite->setOrigin({ 22, 23 });
    }
}

void Player::transformShip()
{
    canJump = false;
    gravity = 1900.f;   // chute normale
    thrust = 3500.f;    // pousse vers le haut
    mode = Type::SHIP;
    if (theme)
    {
        transformSprite->setTextureRect(shipSF);
        transformSprite->setOrigin({ 26, 15 });
    }
    else
    {
        transformSprite->setTextureRect(shipF);
        transformSprite->setOrigin({ 40, 15 });
    }
}

void Player::transformCube()
{
    gravity = 4700.f;          // gravité normale
    gravityInverted = false;   // on annule l'inversion de la ball
    mode = Type::CUBE;
    if (theme)
        sprite->setTextureRect(robot);
    else
        sprite->setTextureRect(mage);
}

void Player::transformWave()
{
    mode = Type::WAVE;
    gravity = 0.f;
    canJump = false;
    if (theme)
    {
        waveSprite->setTextureRect(waveSF);
        waveSprite->setOrigin({21, 18});
    }
    else
    {
        waveSprite->setTextureRect(waveF);
        waveSprite->setOrigin({ 30, 20 });

    }
}

void Player::changeTheme()
{
    theme = !theme;
    switch (mode)
    {
    case Type::CUBE:
        transformCube();
        break;

    case Type::SHIP:
        transformShip();
        break;

    case Type::BALL:
        transformBall();
        break;

    case Type::WAVE:
        transformWave();
        break;
    }
}

void Player::update(float dt, bool jumpPressed)
{
    velocityY += gravity * dt;
    if (onGround || atCeiling)  {
        canJump = true;
    }

    switch (mode)
    {
    case Type::CUBE:
        if (jumpPressed && onGround)
            velocityY = -jumpForce;

        if (!onGround)
        {
            sprite->rotate(sf::degrees(8));
        }
        else
        {
            float angle = sprite->getRotation().asDegrees();
            float angles[] = { 0, 90, 180, 270 };
            float plusProche = 0;
            int minDiff = angle;
            for (int i = 1; i < 4; ++i)
            {
                int diff = std::abs(angle - angles[i]);
                if (diff < minDiff)
                {
                    minDiff = diff;
                    plusProche = angles[i];
                }
            }
            sprite->setRotation(sf::degrees(plusProche));
        }
        break;

    case Type::SHIP:
        if (jumpPressed)
            velocityY -= thrust * dt;
        else if ((shape.getPosition().y < 500 && gravity > 0) || (shape.getPosition().y > 0 && gravity < 0))
            velocityY += gravity * dt;            // applique gravité seulement si pas sol/plafond

        if (velocityY >= 1600.f)
            velocityY = 1600.f;
        if (jumpPressed)
        {
            transformSprite->rotate(sf::degrees(-2.6f));
            if (transformSprite->getRotation() < sf::degrees(310) && transformSprite->getRotation() > sf::degrees(50))
                transformSprite->setRotation(sf::degrees(310));
        }
        else
        {
            transformSprite->rotate(sf::degrees(2.6));
            if (transformSprite->getRotation() < sf::degrees(310) && transformSprite->getRotation() > sf::degrees(50))
                transformSprite->setRotation(sf::degrees(50));
        }
        if (onGround || atCeiling)
            transformSprite->setRotation(sf::degrees(0));
        break;

    case Type::BALL:

        if (jumpPressed && !wasJumpPressed && (onGround || atCeiling)) // appui unique
        {
            gravity = -gravity; // on inverse la gravité une seule fois
            gravityInverted = !gravityInverted; // optionnel : savoir si inversé
        }

        break;
    case Type::WAVE:
        waveSprite->setPosition({ shape.getPosition().x, shape.getPosition().y - 20 });
        if (jumpPressed)
        {
            velocityY = -waveSpeed;
            waveSprite->setRotation(sf::degrees(-45));
        }
        else
        {
            velocityY = waveSpeed;
            waveSprite->setRotation(sf::degrees(45));
        }

        break;
    }
    wasJumpPressed = jumpPressed; // on mémorise l’état actuel
    sprite->setPosition(spriteShape.getPosition());
    transformSprite->setPosition(spriteShape.getPosition());
}

void Player::jump()
{
    if (canJump && mode == Type::CUBE && onGround)
    {
        velocityY = -1050.f;
        canJump = false;
        onGround = false;
    }
    else if (canDoubleJump) // si power-up activé
    {
        velocityY = -1050.f;
        canDoubleJump = false; // consommation du double jump
    }
}
void Player::reset()
{
    canJump = false;
    shape.setPosition(m_spawnPoint); // On utilise le point enregistré
    spriteShape.setPosition({ m_spawnPoint.x, m_spawnPoint.y - 20.f});
    velocityY = 0;
    onGround = true;
    mode = Type::CUBE;
    gravity = 4700.f;
    if (theme)
        changeTheme();

    //if (onGround) canJump = true;
    gravityInverted = false;      // remettre gravité normale
}
void Player::setMode(Type newMode)
{
    mode = newMode;
}
sf::Vector2f Player::getNextMove(float dt)
{
    float gravity = 4700.f;
    velocityY += gravity * dt;

    // On calcule le mouvement théorique (Vitesse * Temps)
    // Horizontal : speed * dt
    // Vertical : velocityY * dt
    return sf::Vector2f(speed * dt, velocityY * dt);
}

void Player::draw(sf::RenderWindow& window,bool showhitbox)
{
    if(mode == Type::CUBE)
        window.draw(*sprite);
    else if(mode == Type::WAVE)
        window.draw(*waveSprite);
    else
        window.draw(*transformSprite);
    if (showhitbox)
        window.draw(shape);
}

sf::FloatRect Player::getBounds() const
{
    return shape.getGlobalBounds();
}
sf::Vector2f Player::getPosition() const
{
    return shape.getPosition();
}
void Player::setPosition(sf::Vector2f pos)
{
    shape.setPosition(pos);
    spriteShape.setPosition({ pos.x, pos.y - 20.f });
}
void Player::move(sf::Vector2f pos)
{
    shape.move(pos);
    spriteShape.move(pos);
}
sf::Vector2f Player::getSize() const
{
    return shape.getSize();
}