#include "Gate.h"
#include "Engine/AssetManager.h"

Gate::Gate(Type inType, sf::Vector2f position, int planeIndex, bool isFlipX, bool isFlipY) : plane(planeIndex),
fx(isFlipX),
fy(isFlipY)
{
	type = inType;
    shape.setSize({ 40.f, 150.f });
    shape.setOrigin({ 20.f, 75.f });
    shape.setFillColor(sf::Color::Transparent);
    shape.setOutlineColor(sf::Color::Green);
    shape.setOutlineThickness(2.f);
    shape.setPosition(position);
    sprite.emplace(AssetManager::getTexture("gates"));
    gateF = sf::IntRect({ 40, 22 }, { 74, 116 });
    gateSF = sf::IntRect({ 150, 22 }, { 74, 116 });
    sprite->setOrigin({ 37, 58 });
    sprite->setPosition(shape.getPosition());
    if (planeIndex == 1)
    {
        theme = false;
        sprite->setTextureRect(gateSF);
    }
    else
    {
        theme = true;
        sprite->setTextureRect(gateF);

    }
}

sf::FloatRect Gate::getBounds() const
{
    return shape.getGlobalBounds();
}

void Gate::setColor(sf::Color color) {
    shape.setFillColor(color);
}

void Gate::draw(sf::RenderWindow& window, bool showhitbox)
{
    if (showhitbox)
        window.draw(shape);
    window.draw(*sprite);
}