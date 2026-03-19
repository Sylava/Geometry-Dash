#pragma once
#include <SFML/Graphics.hpp>

class Options
{
public:

    int volume = 50;
    sf::Keyboard::Key k_jump = sf::Keyboard::Key::Space;
    sf::Keyboard::Key k_jump_alt = sf::Keyboard::Key::Up;
    sf::Keyboard::Key k_switch = sf::Keyboard::Key::Z;
    sf::Keyboard::Key k_switch_alt = sf::Keyboard::Key::Down;
    sf::Keyboard::Key k_option = sf::Keyboard::Key::Escape;
    bool show_hitbox = false;
    bool mouse_ctrl = false;
    bool practice = false;
    bool no_collision = false;
};

