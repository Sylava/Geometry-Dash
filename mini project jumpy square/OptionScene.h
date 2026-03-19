#pragma once
#include "Engine/Scene.h"
#include "keylist.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <map>

class OptionScene: public Scene
{
public:
    OptionScene();
    void onEnter() override;
    void onExit() override;
    void handleEvent(const sf::Event& e) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;




private:
    std::vector<sf::Text> m_options;
    int m_selectedIndex = 0;
    sf::Music m_optionMusic;
    Keylist keylist;

    void updateSelection();

    std::string boolinstr(bool var);
};

