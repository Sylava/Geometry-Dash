#pragma once
#include "Scene.h"
#include "TileMap.h"
#include "AssetManager.h"
#include "Engine.h"
#include "../MenuScene.h"
#include <SFML/Graphics.hpp>
#include <vector>

class EditorScene : public Scene {
public:
    explicit EditorScene(const std::string& levelFile = "Maps/level_direct.txt");

    void onEnter() override;
    void handleEvent(const sf::Event& e) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;

    void saveToFile(const std::string& filename);
    void loadFromFile(const std::string& filename);

private:
    std::vector<TileData> m_layers[4];
    static constexpr int MAX_TILESETS = 8;
    TileMap m_tileMaps[4][MAX_TILESETS];
    int m_currentLayer = 0;

    unsigned int m_width = 1200;
    unsigned int m_height = 30;
    sf::Vector2u m_tileSize = { 16, 16 };

    // les sprites sont initialis�s avec m_tilesetIds[0] dans le constructeur
    //m_tilesetIds doit �tre d�clar� en premier
    std::vector<std::string> m_tilesetIds = { "Main_Terrain", "tileset_fantasy", "tileset_scifi" };
    int m_currentTilesetIdx = 0;
    const std::string& currentTilesetId() const { return m_tilesetIds[m_currentTilesetIdx]; }
    void switchTileset(int delta); // +1 ou -1 pour cycler

    sf::Sprite m_paletteSprite;
    sf::Sprite m_brushPreview;
    int m_selectedTile = 0;
    float m_scale = 1.5f;

    sf::Vector2f m_palettePos = { 1520.f, 10.f };
    sf::RectangleShape m_paletteBackground;

    // Calcule le FloatRect de la palette en fonction de la texture et du scale.
    sf::FloatRect getPaletteRect() const;

    sf::View m_view;
    float m_cameraSpeed = 500.f;
    sf::Text m_layerIndicator;
    void updateVisuals();

    sf::RenderWindow* m_windowPtr = nullptr;

    bool m_flipX = false;
    bool m_flipY = false;

    std::string m_levelFile;

    bool m_isSaving = false;
    bool m_isLoading = false;

    void updateMap();                          // rebuild complet (chargement/switchTileset)
    void updateMap(int layer, int tilesetIdx); // rebuild partiel (une seule tuile changee)
    void rebuildGrid();                        // appele quand le zoom change
    void rebuildDebugVertices();               // appele quand une tuile logique change

    // Geometrie cachee - reconstruite seulement quand les donnees changent
    sf::VertexArray m_gridVertices;
    float           m_cachedGridStep = 0.f;

    sf::VertexArray m_debugVertices;
    bool            m_debugDirty = true;

    sf::Color getTileDebugColor(int id) const;
};