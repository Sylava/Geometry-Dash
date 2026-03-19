#include "OptionScene.h"


#include "MenuScene.h"
#include "Engine/AssetManager.h"
//#include "GameScene.h"
#include "Engine/Engine.h"

OptionScene::OptionScene() {
}

void OptionScene::onEnter() {


    sf::Font& font = AssetManager::getFont("main_font");
    // Création des textes
    m_options.emplace_back(font, "Retour", 30);
    m_options.emplace_back(font, "Options", 40);
    m_options.emplace_back(font, "volume: " + std::to_string(m_manager->option.volume), 25);
    m_options.emplace_back(font, "show hitbox: " + boolinstr(m_manager->option.show_hitbox), 25);
    m_options.emplace_back(font, "practice mode: " + boolinstr(m_manager->option.practice), 25); //removed since there's more things to do elsewhere
    m_options.emplace_back(font, "no collision mode: " + boolinstr(m_manager->option.no_collision), 25);
    m_options.emplace_back(font, "Controles", 40);
    m_options.emplace_back(font, "mouse controle: " + boolinstr(m_manager->option.mouse_ctrl), 25);
    m_options.emplace_back(font, "jump: " + keylist.key_list[m_manager->option.k_jump], 25);
    m_options.emplace_back(font, "jump (alt): " + keylist.key_list[m_manager->option.k_jump_alt], 25);
    m_options.emplace_back(font, "switch: " + keylist.key_list[m_manager->option.k_switch], 25);
    m_options.emplace_back(font, "switch (alt): " + keylist.key_list[m_manager->option.k_switch_alt], 25);
    m_options.emplace_back(font, "menue: " + keylist.key_list[m_manager->option.k_option], 25);

     

    // Positionnement simple
    for (size_t i = 0; i < m_options.size(); ++i) {
        m_options[i].setPosition({ 100.f, 100.f + i * 60.f });
    }
    updateSelection();

    // Lancement de la musique du menu
    if (m_optionMusic.openFromFile("Asset_game/music/time_for_adventure.mp3")) {


        m_optionMusic.setVolume(m_manager->option.volume);
        m_optionMusic.play();
    }
}

void OptionScene::onExit() {
    m_optionMusic.stop(); // On coupe la musique en changeant de scène
}


void OptionScene::updateSelection() {

    for (int i = 0; i < m_options.size(); ++i) {
        m_options[i].setFillColor(i == m_selectedIndex ? sf::Color::Yellow : sf::Color::White);

        if (i == 2) { //mettre a jour l'option au cas où elle est modifié
            m_options[i].setString("volume: " + std::to_string(m_manager->option.volume));
        }
        else if (i == 3) {
            m_options[i].setString("show hitbox: " + boolinstr(m_manager->option.show_hitbox));
        }
        else if (i == 4) { //mettre a jour l'option au cas où elle est modifié
            //m_options[i].setString("practice mode: " + boolinstr(m_manager->option.practice));
            m_options[i].setFillColor(i == m_selectedIndex ? sf::Color::Transparent : sf::Color::Transparent);
        }
        else if (i == 5) { //mettre a jour l'option au cas où elle est modifié
            m_options[i].setString("no collision mode: " + boolinstr(m_manager->option.no_collision));
        }
        else if (i == 7) { //mettre a jour l'option au cas où elle est modifié
            m_options[i].setString("mouse controle : " + boolinstr(m_manager->option.mouse_ctrl));
        }
        else if (i == 8) { //mettre a jour l'option au cas où elle est modifié
            m_options[i].setString("jump: " + keylist.printkey(m_manager->option.k_jump));
        }
        else if (i == 9) { //mettre a jour l'option au cas où elle est modifié
            m_options[i].setString("jump (alt): " + keylist.printkey(m_manager->option.k_jump_alt));
        }
        else if (i == 10) { //mettre a jour l'option au cas où elle est modifié
            m_options[i].setString("switch: " + keylist.printkey(m_manager->option.k_switch));
        }
        else if (i == 11) { //mettre a jour l'option au cas où elle est modifié
            m_options[i].setString("switch (alt): " + keylist.printkey(m_manager->option.k_switch_alt));
        }
        else if (i == 12) { //mettre a jour l'option au cas où elle est modifié
            m_options[i].setString("menue: " + keylist.printkey(m_manager->option.k_option));
        }
    }

}

void OptionScene::handleEvent(const sf::Event& e) {

    if (const auto* key = e.getIf<sf::Event::KeyPressed>()) {

        if (m_manager->option.k_jump == sf::Keyboard::Key::Unknown || m_manager->option.k_jump_alt == sf::Keyboard::Key::Unknown || m_manager->option.k_switch == sf::Keyboard::Key::Unknown || m_manager->option.k_switch_alt == sf::Keyboard::Key::Unknown || m_manager->option.k_option == sf::Keyboard::Key::Unknown) {
            switch (m_selectedIndex) {
            case 8:
                m_manager->option.k_jump = key->code;
                updateSelection();
                return;
            case 9:
                m_manager->option.k_jump_alt = key->code;
                updateSelection();
                return;
            case 10:
                m_manager->option.k_switch = key->code;
                updateSelection();
                return;
            case 11:
                m_manager->option.k_switch_alt = key->code;
                updateSelection();
                return;
            case 12:
                m_manager->option.k_option = key->code;
                updateSelection();
                return;
            default:
                return;
            }
        }

        if (key->code == sf::Keyboard::Key::Up) {
            m_selectedIndex = (m_selectedIndex - 1 + m_options.size()) % m_options.size();
            if (m_selectedIndex == 1 || m_selectedIndex == 6 || m_selectedIndex == 4) { // "category names"
                m_selectedIndex = (m_selectedIndex - 1 + m_options.size()) % m_options.size();
            }
            updateSelection();
        }
        if (key->code == sf::Keyboard::Key::Down) {
            m_selectedIndex = (m_selectedIndex + 1) % m_options.size();
            if (m_selectedIndex == 1 || m_selectedIndex == 6 || m_selectedIndex == 4) { // "category names"
                m_selectedIndex = (m_selectedIndex + 1) % m_options.size();
            }
            updateSelection();
        }
        if (key->code == sf::Keyboard::Key::Right) {
            if (m_selectedIndex == 2 && m_manager->option.volume<100) { // "category names"
                m_manager->option.volume += 1;
                m_optionMusic.setVolume(m_manager->option.volume);
            }
            updateSelection();
        }
        if (key->code == sf::Keyboard::Key::Left) {
            if (m_selectedIndex == 2 && m_manager->option.volume > 0) { // "category names"
                m_manager->option.volume -= 1;
                m_optionMusic.setVolume(m_manager->option.volume);
            }
            updateSelection();
        }

        // Action !
        if (key->code == sf::Keyboard::Key::Enter) {
            if (m_selectedIndex == 0) { // "Retour"
                m_manager->setScene(std::make_unique<MenuScene>());
                return;
            }
            else if (m_selectedIndex == 3) {
                m_manager->option.show_hitbox = !m_manager->option.show_hitbox;
                updateSelection();
                return;
            }
            else if (m_selectedIndex == 4) {
                m_manager->option.practice = !m_manager->option.practice;
                updateSelection();
                return;
            }
            else if (m_selectedIndex == 5) {
                m_manager->option.no_collision = !m_manager->option.no_collision;
                updateSelection();
                return;
            }
            else if (m_selectedIndex == 7) {
                m_manager->option.mouse_ctrl = !m_manager->option.mouse_ctrl;
                updateSelection();
                return;
            }
            else if (m_selectedIndex == 8) {
                m_manager->option.k_jump = sf::Keyboard::Key::Unknown;
                updateSelection();
                return;
            }
            else if (m_selectedIndex == 9) {
                m_manager->option.k_jump_alt = sf::Keyboard::Key::Unknown;
                updateSelection();
                return;
            }
            else if (m_selectedIndex == 10) {
                m_manager->option.k_switch = sf::Keyboard::Key::Unknown;
                updateSelection();
                return;
            }
            else if (m_selectedIndex == 11) {
                m_manager->option.k_switch_alt = sf::Keyboard::Key::Unknown;
                updateSelection();
                return;
            }
            else if (m_selectedIndex == 12) {
                m_manager->option.k_option = sf::Keyboard::Key::Unknown;
                updateSelection();
                return;
            }
            
        }
        if (key->code == sf::Keyboard::Key::Escape) {
            m_manager->setScene(std::make_unique<MenuScene>());
            return;
        }
    }
}

void OptionScene::update(float dt) {}

void OptionScene::draw(sf::RenderWindow& window) {
    for (auto& text : m_options) window.draw(text);
}

std::string OptionScene::boolinstr(bool var) {
    if (var) {
        return "Enabled";
    }
    else {
        return "Disabled";
    }
}