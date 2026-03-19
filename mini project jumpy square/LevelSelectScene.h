#pragma once
#include "Engine/Scene.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

// Mode de la scène de sélection :
// PLAY → lance GameScene(filename)
// EDIT → lance EditorScene(filename), avec option "Nouveau niveau"
enum class LevelSelectMode { PLAY, EDIT };

class LevelSelectScene : public Scene {
public:
    explicit LevelSelectScene(LevelSelectMode mode);

    void onEnter()  override;
    void onExit()   override;
    void handleEvent(const sf::Event& e) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window)  override;

private:
    LevelSelectMode m_mode;

    // Liste des fichiers .txt trouvés dans Maps/
    std::vector<std::string> m_levelFiles;   // chemins complets  ("Maps/level_1.txt")
    std::vector<std::string> m_levelNames;   // noms affichés     ("level_1")

    int m_selectedIndex = 0;

    std::vector<sf::Text>    m_entries;      // textes affichés
    sf::Text                 m_title;
    sf::Text                 m_hint;

    sf::RectangleShape       m_highlight;    // curseur de sélection

    void scanLevels();          // remplit m_levelFiles / m_levelNames
    void rebuildEntries();      // reconstruit m_entries depuis les noms
    void updateSelection();     // met à jour les couleurs
    void confirmSelection();    // lance la scène correspondante
    void createNewLevel();      // (mode EDIT) crée un fichier vide et l'ouvre
};