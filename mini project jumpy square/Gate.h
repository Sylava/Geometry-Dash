#pragma once
#include <SFML/Graphics.hpp>

enum class Type
{
	CUBE,
	SHIP,
	BALL,
	WAVE,
	UFO,
	GRAVITY,
	END
};

class Gate
{
public:
	Gate(Type inType, sf::Vector2f position, int planeIndex, bool isFlipX, bool isFlipY);
	void draw(sf::RenderWindow& window, bool showhitbox);
	sf::FloatRect getBounds() const;
	int getPlane() const { return plane; }
	void setColor(sf::Color color);
	bool theme = false; // false -> SF, true -> fantasy
	Type type;
	bool getFx() const { return fx; }
	bool getFY() const { return fy; }

private:
    std::optional<sf::Sprite> sprite;
	sf::IntRect gateF;
	sf::IntRect gateSF;
	sf::RectangleShape shape;
	int plane;
	bool fx, fy;
};

