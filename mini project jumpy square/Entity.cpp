#include "Entity.h"

Entity::Entity(sf::Texture text, sf::Vector2f position, bool show): square(text) {
	shown = show;
	alive = true;
	square.setPosition(position);
}; 