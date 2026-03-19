#pragma once
#include <SFML/Graphics.hpp>
#include "Engine/AssetManager.h"
#include <memory>

class Victory
{
public:
    Victory(sf::Font& font, const sf::Vector2f& windowSize, int attempts);
    sf::Text title;
    sf::Text attemptsText;
    sf::Text menu;
    // Afficher l'écran
    void draw(sf::RenderWindow& window);

    // Gérer les événements clavier / souris
    void handleEvent(const sf::Event& e);

    // Mettre à jour l'écran (optionnel pour animations)
    void update(float dt);
    void updatePosition(const sf::View& view);
    // Vérifier si le joueur a choisi de continuer ou retourner au menu
    bool continueGame() const { return m_continue; }
    bool goToMenu() const { return m_menu; }

private:
    /*sf::Text m_titleText;
    sf::Text m_continueText;
    sf::Text m_menuText;*/

    sf::RectangleShape m_background;

    bool m_continue = false;
    bool m_menu = false;

    int m_selectedIndex = 0; // 0 = continue, 1 = menu
    void updateSelection();
};