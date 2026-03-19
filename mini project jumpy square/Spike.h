#pragma once
#include <SFML/Graphics.hpp>

class Spike
{
private:
    sf::ConvexShape shape;
    sf::RectangleShape hitbox;
    std::optional<sf::Sprite> spriteSF;
    std::optional<sf::Sprite> spriteF;
    int plane; // 0 ou 1

    bool fx, fy;
public:
    Spike(sf::Vector2f startPos, int planeIndex, bool isFlipX, bool isFlipY); // On ajoute planeIndex ici
    int getPlane() const { return plane; }
    bool getFx() const { return fx; }
    bool getFY() const { return fy; }
    void setColor(sf::Color color); // Pour griser le pique s'il est inactif

    void update(float dt);
    void draw(sf::RenderWindow& window, bool showhitbox);
    sf::FloatRect getBounds() const;
};