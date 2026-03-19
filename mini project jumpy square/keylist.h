#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>

class Keylist {
public:

	std::map<sf::Keyboard::Key, std::string> key_list;

	Keylist();
	bool inlist(sf::Keyboard::Key test_key);
	std::string printkey(sf::Keyboard::Key);
};
