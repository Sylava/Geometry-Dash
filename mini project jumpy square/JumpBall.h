#pragma once
#include <SFML/Graphics.hpp>

class JumpBall
{
public:
    JumpBall(sf::Vector2f startPos, int planeIndex) : plane(planeIndex)
    {
        shape.setRadius(15.f);
        shape.setFillColor(sf::Color::Yellow);
        shape.setOrigin({ 15.f, 15.f }); // centre du cercle
        shape.setPosition(startPos);
    }

    void draw(sf::RenderWindow& window)
    {
        window.draw(shape);
    }

    sf::FloatRect getBounds() const { return shape.getGlobalBounds(); }

    int getPlane() const { return plane; }
    void setColor(sf::Color color) { shape.setFillColor(color); } // Pour griser le pique s'il est inactif
    
private:
    sf::CircleShape shape;
    int plane; // 0 ou 1

    
};