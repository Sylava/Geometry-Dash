#pragma once
#include <SFML/Graphics.hpp>

class VictoryTrigger
{
public:
    VictoryTrigger(const sf::Vector2f& pos) : position(pos) {}

    sf::FloatRect getBounds() const {
        return sf::FloatRect(
            sf::Vector2f(position.x - 25.f, position.y - 25.f),
            sf::Vector2f(50.f, 50.f)
        );
    }

    void draw(sf::RenderWindow& window) {
        sf::RectangleShape rect({ 50.f, 50.f });
        rect.setPosition(position - sf::Vector2f(25.f, 25.f));
        rect.setFillColor(sf::Color::Green); // juste pour debug
        window.draw(rect);
    }

private:
    sf::Vector2f position;
};