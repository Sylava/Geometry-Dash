#pragma once
#include <string>
#include "Engine/Scene.h"
#include "Engine/TileMap.h"
#include "Engine/AssetManager.h"
#include <SFML/Audio.hpp>
#include "Player.h"
#include "JumpBall.h"
#include "CollisionManager.h"
#include "Spike.h"
#include <vector>
#include <memory>
#include "VictoryTrigger.h"
#include "Victory.h"

struct ParallaxLayer {
    sf::Sprite sprite;
    float speedFactor; // 0.0f = immobile, 1.0f = suit la caméra parfaitement
    int plane = 0;
    // Ajoute un constructeur vide par défaut
    ParallaxLayer() = default;

    // Un constructeur pratique pour gagner du temps
    ParallaxLayer(const sf::Texture& tex, float speed, int inPlane) :
        sprite(tex),
        speedFactor(speed),
        plane(inPlane)
    {
        
    }
};

 struct Particle
    {
        sf::CircleShape shape;
        sf::Vector2f velocity;
        float lifetime;
    };

class GameScene : public Scene {
public:
    explicit GameScene(const std::string& levelFile = "Maps/level_direct.txt");
    void onEnter() override;
    void onExit() override;
    void handleEvent(const sf::Event& e) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    std::vector<Particle> particles;
    int attempts = 1;
    sf::Text attemptsText;
    bool m_planeKeyWasPressed = false;

private:
    std::vector<ParallaxLayer> m_backgroundLayers;
    std::vector<TileData> m_layers[4];
    static constexpr int MAX_TILESETS = 8;
    TileMap m_tileMaps[4][MAX_TILESETS];;             // Autant de maps visuelles que de layers
    unsigned int m_width = 15, m_height = 15;
    sf::Vector2u m_tileSize = { 16, 16 };
    float m_scale = 3.0f;
    bool paused = false;
    int currentPlane = 0; // 0 = Arrière, 1 = Avant (par défaut)
    Player player;
    CollisionManager* collision;
    std::vector<Gate> gates;
    std::vector<JumpBall> jumpBalls;
    std::vector<Spike> spike;

    sf::Shader m_overlayShader;
    sf::RectangleShape m_overlayRect;
    float m_overlayTime = 0.f;
    float m_switchFlash = 0.f;  // part à 1 au switch, fond vers 0
    bool m_overlayReady = false;

    sf::Shader m_inverseShader;
    bool m_isEffectActive = false; 
    sf::View m_view;
    sf::Music m_musicFantasy;
    sf::Music m_musicScifi;
    sf::Time  m_musicOffset; // position partagée entre les deux

    std::vector<std::string> m_tilesetIds = { "Main_Terrain", "tileset_fantasy", "tileset_scifi" };
    int m_currentTilesetIdx = 0;
    const std::string& currentTilesetId() const { return m_tilesetIds[m_currentTilesetIdx]; }
    std::vector<VictoryTrigger> victoryTiles;      // toutes les tiles VICTORY
    std::unique_ptr<Victory> victoryScreen;

    std::vector<sf::Sound> switchSounds;
    int currentswitchSound = 0;
    static constexpr int SWITCH_POOL_SIZE = 8;
    float switchCooldown = 0.f;

    //menue
    std::vector<sf::Text> m_options;
    sf::RectangleShape optionbackground;
    int m_selectedIndex = 0;

    std::string m_levelFile;

    void die();
    void spawnParticles(sf::Vector2f pos);
    bool checkCollisionT(sf::Vector2f pos);
    void checkGateCollision();
    void updateSelection();
    void loadMap(const std::string& filename);
    void updateMap();
    void horizontalCollision(float dt);
    void verticalCollision(float dt);
    void parallaxeUpdate(float dt);
    void cameraUpdate(float dt);
};