#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

// Les IDs de tuiles suivent cette convention :
//   id == -1  → case vide (TileType::NONE)
//   id <= -2  → tuile logique (collision, spike, gate…)
//   id >= 0   → tuile visuelle, index direct dans le tileset (sans plafond)
//
// Cette séparation supprime l'ancienne limite arbitraire de 300 tuiles visuelles.

namespace TileType {
    // --- Vide ---
    const int NONE = -1;

    // --- Logiques (tous <= -2) ---
    const int COLLISION = -2;
    const int SPAWN_PLAYER = -3;
    const int SPIKE = -4;
    const int JUMPB = -5;
    const int GATE_SHIP = -6;
    const int GATE_CUBE = -7;
    const int GATE_BALL = -8;
    const int GATE_UFO = -9;
    const int GATE_WAVE = -10;
    const int GATE_GRAVITY = -11;
    const int GATE_GRAVITYLOW = -12;
    const int GATE_GRAVITYHIGH = -13;
    const int VICTORY = -14;

    // --- Helpers sémantiques ---
    inline bool isEmpty(int id) { return id == NONE; }       // case vide
    inline bool isLogical(int id) { return id < NONE; }       // tuile logique (< -1)
    inline bool isVisual(int id) { return id >= 0; }       // tuile visuelle (≥ 0)
}

struct TileData {
    int id = -1;
    bool flipX = false;
    bool flipY = false;
    int tilesetIdx = 0; // index dans la liste des tilesets chargés (0 = Main_Terrain, etc.)
};

class TileMap : public sf::Drawable, public sf::Transformable {
public:
    bool load(const sf::Texture& tileset, sf::Vector2u sourceSize,
        const std::vector<TileData>& tiles,
        unsigned int width, unsigned int height, float scale);
    void setColor(sf::Color color);

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    sf::VertexArray m_vertices;
    const sf::Texture* m_tileset = nullptr;
};