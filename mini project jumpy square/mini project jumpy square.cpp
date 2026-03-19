#include <iostream>
#include <SFML/Graphics.hpp>
#include <memory>
#include "Engine/Engine.h"
#include "Engine/AssetManager.h"
#include "MenuScene.h"

int main()
{
    Engine engine(1850, 980, "Dual Pulse");

    AssetManager::loadTexture("Main_Terrain", "Asset_game/sprites/world_tileset.png");
    AssetManager::loadTexture("tileset_fantasy", "Asset_game/sprites/assett_plateforme_fantasy.png");
    AssetManager::loadTexture("tileset_scifi", "Asset_game/sprites/assett_plateforme_scifi.png");
    AssetManager::loadTexture("UI_MENU", "Asset_game/sprites/Ui_Ux_boutons.png");


    // --- Font ---
    AssetManager::loadFont("main_font", "Asset_game/fonts/PixelOperator8.ttf");
    AssetManager::loadFont("essais_font", "Asset_game/fonts/arial.ttf");
    // Chargement des fonds pour le parallaxe
    AssetManager::loadTexture("Scifi_bg", "Asset_game/background/decorvfuturist.png");
    AssetManager::loadTexture("Fantasy_bg_base", "Asset_game/background/DECOR_V2_base.png");
    AssetManager::loadTexture("Fantasy_bg_nuage", "Asset_game/background/DECOR_V2_nuage.png");

    AssetManager::loadTexture("bg5", "Asset_game/background/5.png");
    AssetManager::loadTexture("bg4", "Asset_game/background/4.png");
    AssetManager::loadTexture("bg3", "Asset_game/background/3.png");
    AssetManager::loadTexture("bg2", "Asset_game/background/2.png");
    AssetManager::loadTexture("bg1", "Asset_game/background/1.png");
    // Son
    AssetManager::loadSound("switch_sound", "Asset_game/sounds/switch_sound.mp3");
    // Texture joueur
    AssetManager::loadTexture("player", "Asset_game/sprites/Players.png");
    AssetManager::loadTexture("tranformations", "Asset_game/sprites/transformations.png");
    AssetManager::loadTexture("waves", "Asset_game/sprites/waves.png");
    // Spikes
    AssetManager::loadTexture("spikeSF", "Asset_game/sprites/spike-sf.png");
    AssetManager::loadTexture("spikeF", "Asset_game/sprites/spike-fantasy.png");
    // Gates
    AssetManager::loadTexture("gates", "Asset_game/sprites/gates.png");
    
    engine.scenes().setScene(std::make_unique<MenuScene>());
    engine.run();

    return 0;
}
 