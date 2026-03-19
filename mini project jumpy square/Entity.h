#pragma once
#include <SFML/graphics.hpp>

class Entity {
private:

	bool alive;
	bool shown;

public:
	sf::Sprite square;

	bool isAlive() { return alive; }
	bool isShown() { return shown; }

	sf::Vector2f getPosition() { return square.getPosition(); }
	sf::Vector2f getScale() { return square.getScale(); }
	sf::Vector2u getSize() { return square.getTexture().getSize(); }
	sf::Vector2f getOrigin() { return square.getOrigin(); }
	sf::Vector2f getCenter() { return { square.getPosition().x+(square.getTexture().getSize().x/2* square.getScale().x),square.getPosition().y + (square.getTexture().getSize().y/2* square.getScale().y) }; }


	void setAlive(bool aliv) { alive = aliv; }
	void setShown(bool show) { shown = show; }

	void setPosition(sf::Vector2f position) { square.setPosition(position); }
	void setScale(sf::Vector2f scale) { square.setScale(scale); }
	void setOrigin(sf::Vector2f origin) { square.setOrigin(origin); }

	void show(sf::RenderWindow& window) { window.draw(square); }

	Entity(sf::Texture text, sf::Vector2f position, bool show);

};