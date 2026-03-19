#include "LevelSelectScene.h"
#include "Engine/AssetManager.h"
#include "Engine/Engine.h"
#include "GameScene.h"
#include "Engine/EditorScene.h"
#include "MenuScene.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <numeric>

namespace fs = std::filesystem;

// =============================================================================
//  Constructeur
// =============================================================================

LevelSelectScene::LevelSelectScene(LevelSelectMode mode)
    : m_mode(mode),
    m_title(AssetManager::getFont("main_font"),
        mode == LevelSelectMode::PLAY ? "Choisir un niveau" : "Editer un niveau",
        60),
    m_hint(AssetManager::getFont("main_font"), "", 28)
{
    m_title.setFillColor(sf::Color::White);
    m_title.setOutlineColor(sf::Color::Black);
    m_title.setOutlineThickness(2.f);

    m_hint.setFillColor(sf::Color(200, 200, 200));

    m_highlight.setFillColor(sf::Color(255, 200, 0, 60));
    m_highlight.setOutlineColor(sf::Color(255, 200, 0, 180));
    m_highlight.setOutlineThickness(2.f);
}

// =============================================================================
//  onEnter / onExit
// =============================================================================

void LevelSelectScene::onEnter()
{
    scanLevels();
    rebuildEntries();
    updateSelection();
}

void LevelSelectScene::onExit() {}

// =============================================================================
//  scanLevels — parcourt Maps/ et collecte les .txt
// =============================================================================

void LevelSelectScene::scanLevels()
{
    m_levelFiles.clear();
    m_levelNames.clear();

    const std::string mapsDir = "Maps";

    // Crée le dossier s'il n'existe pas encore
    if (!fs::exists(mapsDir))
        fs::create_directory(mapsDir);

    for (const auto& entry : fs::directory_iterator(mapsDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            m_levelFiles.push_back(entry.path().string());
            m_levelNames.push_back(entry.path().stem().string()); // nom sans extension
        }
    }

    // Tri alphabétique pour un ordre cohérent
    // On trie les deux vecteurs ensemble via un index
    std::vector<int> idx(m_levelFiles.size());
    std::iota(idx.begin(), idx.end(), 0);
    std::sort(idx.begin(), idx.end(), [&](int a, int b) {
        return m_levelNames[a] < m_levelNames[b];
        });
    std::vector<std::string> sortedFiles, sortedNames;
    for (int i : idx) {
        sortedFiles.push_back(m_levelFiles[i]);
        sortedNames.push_back(m_levelNames[i]);
    }
    m_levelFiles = sortedFiles;
    m_levelNames = sortedNames;

    // En mode EDIT : ajouter l'entrée "[ + Nouveau niveau ]" à la fin
    if (m_mode == LevelSelectMode::EDIT) {
        m_levelFiles.push_back("");          // chemin vide = nouveau
        m_levelNames.push_back("[ + Nouveau niveau ]");
    }

    m_selectedIndex = 0;
}

// =============================================================================
//  rebuildEntries — reconstruit les sf::Text d'affichage
// =============================================================================

void LevelSelectScene::rebuildEntries()
{
    m_entries.clear();
    sf::Font& font = AssetManager::getFont("main_font");

    for (const auto& name : m_levelNames) {
        sf::Text t(font, name, 44);
        t.setFillColor(sf::Color::White);
        m_entries.push_back(std::move(t));
    }

    // Hint en bas
    if (m_mode == LevelSelectMode::PLAY)
        m_hint.setString("Entree : jouer    Echap : retour");
    else
        m_hint.setString("Entree : ouvrir    Echap : retour");
}

// =============================================================================
//  updateSelection
// =============================================================================

void LevelSelectScene::updateSelection()
{
    for (int i = 0; i < (int)m_entries.size(); ++i)
        m_entries[i].setFillColor(i == m_selectedIndex
            ? sf::Color::Yellow
            : sf::Color::White);
}

// =============================================================================
//  handleEvent
// =============================================================================

void LevelSelectScene::handleEvent(const sf::Event& e)
{
    if (const auto* key = e.getIf<sf::Event::KeyPressed>()) {

        if (key->code == sf::Keyboard::Key::Up) {
            m_selectedIndex = (m_selectedIndex - 1 + (int)m_entries.size())
                % (int)m_entries.size();
            updateSelection();
        }
        else if (key->code == sf::Keyboard::Key::Down) {
            m_selectedIndex = (m_selectedIndex + 1) % (int)m_entries.size();
            updateSelection();
        }
        else if (key->code == sf::Keyboard::Key::Enter) {
            if (!m_entries.empty())
                confirmSelection();
        }
        else if (key->code == sf::Keyboard::Key::Escape) {
            m_manager->setScene(std::make_unique<MenuScene>());
        }
    }
}

// =============================================================================
//  confirmSelection
// =============================================================================

void LevelSelectScene::confirmSelection()
{
    const std::string& path = m_levelFiles[m_selectedIndex];

    // Entrée "Nouveau niveau" (path vide, mode EDIT seulement)
    if (path.empty()) {
        createNewLevel();
        return;
    }

    if (m_mode == LevelSelectMode::PLAY)
        m_manager->setScene(std::make_unique<GameScene>(path));
    else
        m_manager->setScene(std::make_unique<EditorScene>(path));
}

// =============================================================================
//  createNewLevel — crée un fichier vide et ouvre l'éditeur dessus
// =============================================================================

void LevelSelectScene::createNewLevel()
{
    // Génère un nom unique : level_N.txt où N est le premier entier libre
    int n = 1;
    std::string newPath;
    do {
        newPath = "Maps/level_" + std::to_string(n++) + ".txt";
    } while (fs::exists(newPath));

    // Écrit un fichier vide valide (300×30, 4 layers vides)
    std::ofstream file(newPath);
    if (!file.is_open()) {
        std::cerr << "Impossible de créer : " << newPath << std::endl;
        return;
    }

    const unsigned int W = 300, H = 30;
    file << "width " << W << " height " << H << "\n";

    for (int l = 0; l < 4; ++l) {
        file << "layer " << l << "\n";
        for (unsigned int row = 0; row < H; ++row) {
            for (unsigned int col = 0; col < W; ++col) {
                if (l == 1 || l == 3)
                    file << "0 0 0 "; // logique vide : symbole flipX flipY
                else
                    file << "-1 0 0 0 "; // visuel vide : id flipX flipY tilesetIdx
            }
            file << "\n";
        }
    }
    file.close();

    std::cout << "Nouveau niveau cree : " << newPath << std::endl;
    m_manager->setScene(std::make_unique<EditorScene>(newPath));
}

// =============================================================================
//  update
// =============================================================================

void LevelSelectScene::update(float /*dt*/) {}

// =============================================================================
//  draw
// =============================================================================

void LevelSelectScene::draw(sf::RenderWindow& window)
{
    // Vue fixe centrée sur la fenêtre
    sf::View view;
    view.setCenter({ 925.f, 490.f });
    view.setSize({ 1850.f, 980.f });
    window.setView(view);

    const float startX = 300.f;
    const float startY = 150.f;
    const float lineH = 60.f;

    // Titre
    m_title.setPosition({ startX, 60.f });
    window.draw(m_title);

    // Entrées
    for (int i = 0; i < (int)m_entries.size(); ++i) {
        float y = startY + i * lineH;
        m_entries[i].setPosition({ startX, y });

        // Highlight de la sélection courante
        if (i == m_selectedIndex) {
            m_highlight.setSize({ 900.f, lineH - 4.f });
            m_highlight.setPosition({ startX - 10.f, y + 2.f });
            window.draw(m_highlight);
        }

        window.draw(m_entries[i]);
    }

    // Hint bas de page
    m_hint.setPosition({ startX, 900.f });
    window.draw(m_hint);

    // Message si aucun niveau disponible (hors entrée "Nouveau")
    if (m_levelFiles.size() == (m_mode == LevelSelectMode::EDIT ? 1u : 0u)) {
        sf::Text empty(AssetManager::getFont("main_font"),
            "Aucun niveau trouve dans Maps/", 36);
        empty.setFillColor(sf::Color(180, 180, 180));
        empty.setPosition({ startX, startY });
        window.draw(empty);
    }
}