#include "Spike.h"
#include "Engine/AssetManager.h"

Spike::Spike(sf::Vector2f startPos, int planeIndex, bool isFlipX, bool isFlipY) : plane(planeIndex),
fx(isFlipX),
fy(isFlipY)
{
    shape.setPointCount(3);
    if (!fy) {
        shape.setPoint(0, { 0.f, 50.f });
        shape.setPoint(1, { 25.f,  0.f });
        shape.setPoint(2, { 50.f, 50.f });
    }
    else {
        shape.setPoint(0, { 0.f,  0.f });
        shape.setPoint(1, { 25.f, 50.f });
        shape.setPoint(2, { 50.f,  0.f });
    }
    shape.setFillColor(sf::Color::Red);
    shape.setPosition(startPos); 
    auto bounds = shape.getLocalBounds();
    shape.setOrigin({ bounds.size.x / 2, bounds.size.y / 2});
    
    const float hbW = 14.f, hbH = 30.f;
    const float originY = fy ? 8.f : 3.f;

    hitbox.setSize({ hbW, hbH });
    hitbox.setOrigin({ 8.f, originY });
    hitbox.setPosition(shape.getPosition());
    hitbox.setFillColor(sf::Color::Transparent);
    hitbox.setOutlineColor(sf::Color::Blue);
    hitbox.setOutlineThickness(1.f);

    sf::IntRect rect;
    rect = sf::IntRect({ 7, 13 }, { 28, 21 });
    spriteF.emplace(AssetManager::getTexture("spikeF"));
    spriteF->setTextureRect(rect);
    spriteF->setOrigin({ 15, 8 });
    spriteF->setScale({ 1.8, 2 });
    spriteF->setPosition(shape.getPosition());
    spriteSF.emplace(AssetManager::getTexture("spikeSF"));
    spriteSF->setTextureRect(rect);
    spriteSF->setOrigin({ 15, 8 });
    spriteSF->setScale({ 1.8, 2 });
    spriteSF->setPosition(shape.getPosition());
    if (fy)
    {
        hitbox.setPosition({ hitbox.getPosition().x + 2, hitbox.getPosition().y - 18});
        spriteF->setRotation(sf::degrees(180));
        spriteSF->setRotation(sf::degrees(180));
    }
}

void Spike::update(float dt)
{

}

void Spike::setColor(sf::Color color) {
    shape.setFillColor(color);
}

void Spike::draw(sf::RenderWindow& window,bool showhitbox)
{
    if (plane == 1)
        window.draw(*spriteSF);
    else
        window.draw(*spriteF);
    if (showhitbox) {
        window.draw(hitbox);
    }
}

sf::FloatRect Spike::getBounds() const
{
    return hitbox.getGlobalBounds();
}