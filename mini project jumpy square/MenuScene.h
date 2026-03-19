#pragma once
#include "Engine/Scene.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>

class MenuScene : public Scene {
public:
    MenuScene();
    void onEnter() override;
    void onExit() override;
    void handleEvent(const sf::Event& e) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;

private:
    std::vector<sf::Text> m_options;
    std::vector<sf::Sprite> m_UI;
    int m_selectedIndex = 0;
    sf::Music m_menuMusic;

    // Effets visuels
    float           m_time = 0.f;
    sf::Shader      m_bgShader;       // fond aurora anime
    sf::Shader      m_glowShader;     // halo sur l'option selectionnee
    sf::RectangleShape m_bgRect;      // quad plein ecran pour le fond
    sf::RectangleShape m_glowRect;    // quad derriere l'option active
    bool            m_shadersLoaded = false;

    void updateSelection();
};