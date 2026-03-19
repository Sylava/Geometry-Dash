#include "MenuScene.h"
#include "Engine/AssetManager.h"
#include "Engine/EditorScene.h"
#include "OptionScene.h"
#include "GameScene.h"
#include "LevelSelectScene.h"
#include "Engine/Engine.h"
#include "CollisionManager.h"
#include <iostream>

MenuScene::MenuScene() {
    sf::Font& font = AssetManager::getFont("main_font");
    sf::Texture& interface = AssetManager::getTexture("UI_MENU");

    const int btnW = 32;
    const int btnH = 32;

    // Définition des 3 zones (Rects) pour tes 3 types de boutons
    sf::IntRect rectNormal({ 0, 0 }, { btnW, btnH });
    sf::IntRect rectHover({ 0, btnH }, { btnW, btnH });
    sf::IntRect rectSelected({ 0, btnH * 2 }, { btnW, btnH });

    // 1. Définition des textes
    std::vector<std::string> labels = { "Jouer", "Editeur", "Options", "Quitter" };

    for (size_t i = 0; i < labels.size(); ++i) {
        // Créer le texte
        m_options.emplace_back(font, labels[i], 50);
        m_options[i].setPosition({ 800.f, 150.f + i * 80.f });

        // 2. Sprite de fond (le bouton)
        sf::Sprite btnSprite(interface);
        btnSprite.setTextureRect(rectNormal); // On applique le IntRect ici

        // On centre le sprite par rapport au texte
        btnSprite.setOrigin({ btnW / 2.f, btnH / 2.f });
        // On place le bouton au centre du texte (en ajustant un peu l'Y si besoin)
        btnSprite.setPosition({ m_options[i].getPosition().x - 35.f, m_options[i].getPosition().y + 20.f });
        btnSprite.setScale({ 3.7f, 2.0f }); // Optionnel : pour ajuster la taille

        m_UI.push_back(btnSprite);
    }

    updateSelection();
}

void MenuScene::onEnter() {
    // Lancement de la musique du menu
    if (m_menuMusic.openFromFile("Asset_game/music/menu.mp3")) {
        m_menuMusic.setLooping(true);

        m_menuMusic.setVolume(m_manager->option.volume);
        m_menuMusic.play();
    }
    // Shaders
    if (sf::Shader::isAvailable()) {
        bool bgOk = m_bgShader.loadFromFile("Asset_game/Shader/menu_bg.frag",
            sf::Shader::Type::Fragment);
        bool glowOk = m_glowShader.loadFromFile("Asset_game/Shader/menu_glow.frag",
            sf::Shader::Type::Fragment);
        m_shadersLoaded = bgOk && glowOk;

        if (m_shadersLoaded) {
            // Quad plein ecran pour le fond (1850x980)
            m_bgRect.setSize({ 1850.f, 980.f });
            m_bgRect.setPosition({ 0.f, 0.f });
            m_bgRect.setFillColor(sf::Color::White); // couleur ignoree par le shader
            m_bgShader.setUniform("resolution", sf::Glsl::Vec2(1850.f, 980.f));
            m_bgShader.setUniform("time", 0.f);

            // Quad pour le glow (meme largeur qu'un bouton, positionne dans draw)
            m_glowRect.setSize({ 420.f, 70.f });
            m_glowRect.setFillColor(sf::Color::White);
            m_glowRect.setTextureRect(sf::IntRect({ 0, 0 }, { 1, 1 })); // coords 0..1
            m_glowShader.setUniform("time", 0.f);
        }
        else {
            std::cout << "Shaders menu non charges" << std::endl;
        }
    }
}

void MenuScene::onExit() {
    m_menuMusic.stop();
}


void MenuScene::updateSelection() {
    int btnH = 134; // La même hauteur que dans le constructeur
    int singleH = btnH / 3;
    for (size_t i = 0; i < m_options.size(); ++i) {
        if (i == m_selectedIndex) {
            m_options[i].setFillColor(sf::Color::Yellow);
            // Change le rectangle de texture du sprite correspondant
            m_UI[i].setTextureRect(sf::IntRect({ 0, singleH * 2 }, { 380, singleH })); // Version "Selectionnée"
        }
        else {
            m_options[i].setFillColor(sf::Color::Black);
            m_UI[i].setTextureRect(sf::IntRect({ 0, 0 }, { 380, btnH / 3 }));    // Version "Normale"
        }
    }
}

void MenuScene::handleEvent(const sf::Event& e) {
    if (const auto* key = e.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Up) {
            m_selectedIndex = (m_selectedIndex - 1 + m_options.size()) % m_options.size();
            updateSelection();
        }
        if (key->code == sf::Keyboard::Key::Down) {
            m_selectedIndex = (m_selectedIndex + 1) % m_options.size();
            updateSelection();
        }
        // Action !
        if (key->code == sf::Keyboard::Key::Enter) {
            if (m_selectedIndex == 0) {
                // Lancer le jeu
                m_manager->setScene(std::make_unique<LevelSelectScene>(LevelSelectMode::PLAY));
                return;
            }
            else if (m_selectedIndex == 1) { // "Editeur"
                m_manager->setScene(std::make_unique<LevelSelectScene>(LevelSelectMode::EDIT));
            }
            else if (m_selectedIndex == 2) { // "Option"
                m_manager->setScene(std::make_unique<OptionScene>());
            }
            else if (m_selectedIndex == 3) { // "Quitter"
                exit(0);
            }
        }
    }
}

void MenuScene::update(float dt) {
    m_time += dt;
    if (m_shadersLoaded) {
        m_bgShader.setUniform("time", m_time);
        m_glowShader.setUniform("time", m_time);
    }
}

void MenuScene::draw(sf::RenderWindow& window) {

    //to reset the view after coming back from game
    sf::View baseview;
    baseview.setCenter({ 925,490 });
    baseview.setSize({ 1850, 980 });
    window.setView(baseview);

    if (m_shadersLoaded) {
        // 1. Fond aurora anime
        window.draw(m_bgRect, &m_bgShader);

        // 2. Glow derriere l'option selectionnee
        sf::Vector2f optPos = m_options[m_selectedIndex].getPosition();
        m_glowRect.setPosition({ optPos.x - 25.f, optPos.y - 5.f });
        window.draw(m_glowRect, &m_glowShader);
    }

    for (auto& UI : m_UI) window.draw(UI);
    for (auto& text : m_options) window.draw(text);
}