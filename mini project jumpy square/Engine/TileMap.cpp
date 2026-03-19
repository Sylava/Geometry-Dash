#include "TileMap.h"

bool TileMap::load(const sf::Texture& tileset, sf::Vector2u sourceSize, const std::vector<TileData>& tiles, unsigned int width, unsigned int height, float scale) {
    m_tileset = &tileset;
    m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
    // Sparse : on n'alloue QUE les tuiles non vides.
    // Pour 300x30 vide ca fait 0 vertices au lieu de 54 000.
    m_vertices.clear();

    float displaySizeX = sourceSize.x * scale;
    float displaySizeY = sourceSize.y * scale;

    for (unsigned int i = 0; i < width; ++i) {
        for (unsigned int j = 0; j < height; ++j) {
            const TileData& tile = tiles[i + j * width];
            if (!TileType::isVisual(tile.id)) continue;

            int tu = tile.id % (m_tileset->getSize().x / sourceSize.x);
            int tv = tile.id / (m_tileset->getSize().x / sourceSize.x);

            sf::Vector2f pos0( i      * displaySizeX,  j      * displaySizeY);
            sf::Vector2f pos1((i + 1) * displaySizeX,  j      * displaySizeY);
            sf::Vector2f pos2((i + 1) * displaySizeX, (j + 1) * displaySizeY);
            sf::Vector2f pos3( i      * displaySizeX, (j + 1) * displaySizeY);

            float left   = (float)(tu * sourceSize.x);
            float right  = left + (float)sourceSize.x;
            float top    = (float)(tv * sourceSize.y);
            float bottom = top  + (float)sourceSize.y;

            if (tile.flipX) std::swap(left, right);
            if (tile.flipY) std::swap(top, bottom);

            m_vertices.append(sf::Vertex(pos0, sf::Color::White, { left,  top    }));
            m_vertices.append(sf::Vertex(pos1, sf::Color::White, { right, top    }));
            m_vertices.append(sf::Vertex(pos2, sf::Color::White, { right, bottom }));
            m_vertices.append(sf::Vertex(pos0, sf::Color::White, { left,  top    }));
            m_vertices.append(sf::Vertex(pos2, sf::Color::White, { right, bottom }));
            m_vertices.append(sf::Vertex(pos3, sf::Color::White, { left,  bottom }));
        }
    }
    return true;
}

void TileMap::setColor(sf::Color color) {
    for (std::size_t i = 0; i < m_vertices.getVertexCount(); ++i) {
        m_vertices[i].color = color;
    }
}

void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    states.texture = m_tileset;
    target.draw(m_vertices, states);
}