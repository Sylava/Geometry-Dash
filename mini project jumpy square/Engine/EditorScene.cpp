#include "EditorScene.h"
#include <iostream>
#include <fstream>

EditorScene::EditorScene(const std::string& levelFile)
    : m_paletteSprite(AssetManager::getTexture(m_tilesetIds[0])),
    m_brushPreview(AssetManager::getTexture(m_tilesetIds[0])),
    m_layerIndicator(AssetManager::getFont("essais_font")),
    m_levelFile(levelFile)
{
    m_layerIndicator.setCharacterSize(30);
    m_layerIndicator.setFillColor(sf::Color::White);
    m_layerIndicator.setOutlineColor(sf::Color::Black);
    m_layerIndicator.setOutlineThickness(2);

    m_brushPreview.setColor(sf::Color(255, 255, 255, 200));

    for (int i = 0; i < 4; ++i)
        m_layers[i].assign(m_width * m_height, { -1, false, false });

    // Fond de la palette — sa taille sera mise à jour dans onEnter / updateVisuals
    m_paletteBackground.setFillColor(sf::Color(30, 30, 30, 200));
    m_paletteBackground.setOutlineColor(sf::Color(200, 200, 200, 150));
    m_paletteBackground.setOutlineThickness(1.f);
}

sf::FloatRect EditorScene::getPaletteRect() const
{
    const sf::Texture& tex = AssetManager::getTexture(currentTilesetId());
    float paletteW = tex.getSize().x * m_scale;
    float paletteH = tex.getSize().y * m_scale;
    return sf::FloatRect(m_palettePos, { paletteW, paletteH });
}

// Couleur de debug associée à chaque type logique.
// Utilisée à la fois pour l'affichage des tuiles placées ET pour le curseur de pinceau.
sf::Color EditorScene::getTileDebugColor(int id) const
{
    if (id == TileType::COLLISION)    return sf::Color(255, 0, 0, 150);
    if (id == TileType::SPAWN_PLAYER) return sf::Color(0, 255, 0, 150);
    if (id == TileType::SPIKE)        return sf::Color(255, 165, 0, 150);
    if (id == TileType::JUMPB)        return sf::Color(0, 150, 255, 150);
    if (id == TileType::GATE_SHIP)    return sf::Color(255, 255, 0, 150);
    if (id == TileType::GATE_CUBE)    return sf::Color(255, 0, 255, 150);
    if (id == TileType::GATE_BALL)    return sf::Color(0, 255, 255, 150);
    if (id == TileType::GATE_UFO)     return sf::Color(100, 100, 100, 150);
    if (id == TileType::GATE_WAVE)    return sf::Color(255, 150, 0, 150);
    if (id == TileType::GATE_GRAVITY) return sf::Color(200, 200, 200, 150);
    if (id == TileType::VICTORY)      return sf::Color(255, 255, 255, 150);
    return sf::Color::Transparent;
}



void EditorScene::onEnter()
{
    m_view.setSize({ 1000.f, 1000.f });
    m_view.setCenter({ 500.f, 500.f });

    // On initialise m_windowPtr ici si on a accès à la fenêtre — sinon il sera
    // capturé au premier draw() comme avant.

    updateMap();
    m_debugDirty = true;
    updateVisuals();
}

// =============================================================================
//  Événements
// =============================================================================

void EditorScene::handleEvent(const sf::Event& e)
{
    if (!m_windowPtr) return;

    const float displaySize = m_tileSize.x * m_scale;
    const sf::FloatRect paletteRect = getPaletteRect(); // ← une seule fois par event

    // ------------------------------------------------------------------
    // A. Position souris
    // ------------------------------------------------------------------
    sf::Vector2i mousePosPixel;
    bool isAction = false;

    if (const auto* mBtn = e.getIf<sf::Event::MouseButtonPressed>()) {
        mousePosPixel = mBtn->position;
        isAction = true;
    }
    else if (const auto* mMove = e.getIf<sf::Event::MouseMoved>()) {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) ||
            sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
        {
            mousePosPixel = mMove->position;
            isAction = true;
        }
    }

    // ------------------------------------------------------------------
    // B. Clic / dessin
    // ------------------------------------------------------------------
    if (isAction)
    {
        sf::Vector2f mouseF(static_cast<float>(mousePosPixel.x),
            static_cast<float>(mousePosPixel.y));

        // --- Zone PALETTE ---
        if (paletteRect.contains(mouseF) &&
            sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            const sf::Texture& tex = AssetManager::getTexture(currentTilesetId());
            const int tilesPerRow = tex.getSize().x / m_tileSize.x;

            // Coordonnées locales à l'intérieur de la palette
            int tx = static_cast<int>(mouseF.x - paletteRect.position.x) / static_cast<int>(displaySize);
            int ty = static_cast<int>(mouseF.y - paletteRect.position.y) / static_cast<int>(displaySize);
            int id = tx + ty * tilesPerRow;

            const int maxTiles = tilesPerRow * (tex.getSize().y / m_tileSize.y);

            if (tx >= 0 && tx < tilesPerRow && id >= 0 && id < maxTiles)
            {
                m_selectedTile = id;
                if (m_currentLayer == 1 || m_currentLayer == 3)
                    m_currentLayer = 0; // repasser sur une couche visuelle
                m_brushPreview.setTextureRect(sf::IntRect(
                    { tx * (int)m_tileSize.x, ty * (int)m_tileSize.y },
                    { (int)m_tileSize.x,      (int)m_tileSize.y }
                ));
            }
        }
        // --- Zone de DESSIN (tout ce qui n'est pas la palette) ---
        else if (!paletteRect.contains(mouseF))
        {
            sf::Vector2f worldPos = m_windowPtr->mapPixelToCoords(mousePosPixel, m_view);

            int gx = static_cast<int>(worldPos.x) / static_cast<int>(displaySize);
            int gy = static_cast<int>(worldPos.y) / static_cast<int>(displaySize);

            if (gx >= 0 && gx < (int)m_width && gy >= 0 && gy < (int)m_height)
            {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                    const TileData newTile = { m_selectedTile, m_flipX, m_flipY, m_currentTilesetIdx };
                    m_layers[m_currentLayer][gx + gy * m_width] = newTile;
                    if (TileType::isVisual(m_selectedTile))
                        updateMap(m_currentLayer, m_currentTilesetIdx); // rebuild 1 sous-tilemap au lieu de 12
                    else
                        m_debugDirty = true;
                }
                else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
                    const int oldTs = m_layers[m_currentLayer][gx + gy * m_width].tilesetIdx;
                    const int oldId = m_layers[m_currentLayer][gx + gy * m_width].id;
                    m_layers[m_currentLayer][gx + gy * m_width] = { -1, false, false, 0 };
                    if (TileType::isVisual(oldId))
                        updateMap(m_currentLayer, oldTs);
                    else
                        m_debugDirty = true;
                }
            }
        }
    }

    // ------------------------------------------------------------------
    // C. Raccourcis clavier
    // ------------------------------------------------------------------
    if (const auto* key = e.getIf<sf::Event::KeyPressed>())
    {
        // Navigation de tileset
        if (key->code == sf::Keyboard::Key::Tab) switchTileset(+1);

        // Sélection de couche
        if (key->code == sf::Keyboard::Key::Num1) m_currentLayer = 0;
        if (key->code == sf::Keyboard::Key::Num2) m_currentLayer = 1;
        if (key->code == sf::Keyboard::Key::Num3) m_currentLayer = 2;
        if (key->code == sf::Keyboard::Key::Num4) m_currentLayer = 3;

        // Miroirs — touches dédiées sans conflit
        if (key->code == sf::Keyboard::Key::X) m_flipX = !m_flipX;
        if (key->code == sf::Keyboard::Key::F) m_flipY = !m_flipY; // était Y, conflit corrigé

        // Tuiles logiques — chaque touche a sa propre lettre
        if (key->code == sf::Keyboard::Key::C) { m_selectedTile = TileType::COLLISION;    m_currentLayer = 1; }
        if (key->code == sf::Keyboard::Key::P) { m_selectedTile = TileType::SPAWN_PLAYER; m_currentLayer = 1; }
        if (key->code == sf::Keyboard::Key::A) { m_selectedTile = TileType::SPIKE;        m_currentLayer = 1; }
        if (key->code == sf::Keyboard::Key::R) { m_selectedTile = TileType::JUMPB;        m_currentLayer = 1; }
        if (key->code == sf::Keyboard::Key::G) { m_selectedTile = TileType::GATE_SHIP;    m_currentLayer = 1; }
        if (key->code == sf::Keyboard::Key::Z) { m_selectedTile = TileType::GATE_CUBE;    m_currentLayer = 1; }
        if (key->code == sf::Keyboard::Key::T) { m_selectedTile = TileType::GATE_BALL;    m_currentLayer = 1; }
        if (key->code == sf::Keyboard::Key::Y) { m_selectedTile = TileType::GATE_UFO;     m_currentLayer = 1; } // Y dédié UFO
        if (key->code == sf::Keyboard::Key::U) { m_selectedTile = TileType::GATE_WAVE;    m_currentLayer = 1; }
        if (key->code == sf::Keyboard::Key::I) { m_selectedTile = TileType::GATE_GRAVITY; m_currentLayer = 1; }
        if (key->code == sf::Keyboard::Key::V) { m_selectedTile = TileType::VICTORY;      m_currentLayer = 1; }
        if (key->code == sf::Keyboard::Key::E) { m_selectedTile = -1; }
        if (key->code == sf::Keyboard::Key::Escape) {
            m_manager->setScene(std::make_unique<MenuScene>());
            return;
        }
    }

    // Zoom molette
    if (const auto* scroll = e.getIf<sf::Event::MouseWheelScrolled>())
    {
        if (scroll->wheel == sf::Mouse::Wheel::Vertical)
        {
            float zoomFactor = (scroll->delta > 0) ? 0.9f : 1.1f;
            m_view.zoom(zoomFactor);
        }
    }

    updateVisuals();
}

// =============================================================================
//  updateVisuals
// =============================================================================

void EditorScene::updateVisuals()
{
    // Texte de la couche active + tileset actif
    std::string layerNames[4] = {
        "0: Background Visual",
        "1: Background Logic",
        "2: Foreground Visual",
        "3: Foreground Logic"
    };
    m_layerIndicator.setString(
        "[TAB] Tileset: " + currentTilesetId() + "\n" +
        "Layer " + layerNames[m_currentLayer] +
        (m_flipX ? "  [flipX]" : "") +
        (m_flipY ? "  [flipY]" : "")
    );

    // Transparence des layers non actifs (tous les sous-tilemaps du layer)
    const int numTilesets = static_cast<int>(m_tilesetIds.size());
    for (int i = 0; i < 4; ++i) {
        sf::Color c = (i == m_currentLayer) ? sf::Color::White : sf::Color(255, 255, 255, 100);
        for (int ts = 0; ts < numTilesets; ++ts)
            m_tileMaps[i][ts].setColor(c);
    }

    // Fond de la palette calqué sur le getPaletteRect courant
    const sf::FloatRect pr = getPaletteRect();
    const float padding = 6.f;
    m_paletteBackground.setPosition({ pr.position.x - padding, pr.position.y - padding });
    m_paletteBackground.setSize({ pr.size.x + padding * 2, pr.size.y + padding * 2 });
}

// =============================================================================
//  switchTileset
// =============================================================================

void EditorScene::switchTileset(int delta)
{
    m_currentTilesetIdx = (m_currentTilesetIdx + delta + (int)m_tilesetIds.size())
        % (int)m_tilesetIds.size();

    const sf::Texture& tex = AssetManager::getTexture(currentTilesetId());

    // On repointe les sprites sur la nouvelle texture
    m_paletteSprite.setTexture(tex);
    m_brushPreview.setTexture(tex);

    // La tuile sélectionnée est remise à 0 pour éviter un ID hors-limites
    m_selectedTile = 0;
    m_brushPreview.setTextureRect(sf::IntRect(
        { 0, 0 },
        { (int)m_tileSize.x, (int)m_tileSize.y }
    ));

    // Les tilemaps visuels doivent être reconstruits avec la nouvelle texture
    updateMap();
    updateVisuals();
}



void EditorScene::update(float dt)
{
    sf::Vector2f movement(0.f, 0.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))    movement.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))  movement.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))  movement.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) movement.x += 1.f;

    if (movement.x != 0 || movement.y != 0)
        m_view.move(movement * m_cameraSpeed * dt);

    // Sauvegarde (garde anti-répétition)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
        if (!m_isSaving) {
            saveToFile(m_levelFile);
            m_isSaving = true;
        }
    }
    else {
        m_isSaving = false;
    }

    // Chargement (garde anti-répétition — même logique que la sauvegarde)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::L)) {
        if (!m_isLoading) {
            loadFromFile(m_levelFile);
            m_isLoading = true;
        }
    }
    else {
        m_isLoading = false;
    }
}

// =============================================================================
//  Draw
// =============================================================================

void EditorScene::draw(sf::RenderWindow& window)
{
    if (!m_windowPtr) m_windowPtr = &window;

    const float step = m_tileSize.x * m_scale;
    const float gridSizeX = m_width * step;
    const float gridSizeY = m_height * step;

    // -----------------------------------------------------------------------
    // 1. PHASE MONDE (éléments qui suivent la caméra)
    // -----------------------------------------------------------------------
    window.setView(m_view);

    // Couches visuelles — on dessine tous les sous-tilemaps (un par tileset)
    // Les sous-tilemaps vides ne coûtent rien (vertex array vide)
    const int numTilesets = static_cast<int>(m_tilesetIds.size());
    for (int ts = 0; ts < numTilesets; ++ts) {
        window.draw(m_tileMaps[0][ts]);
        window.draw(m_tileMaps[2][ts]);
    }

    // Debug rects+fleches -- rebuild seulement si une tuile logique a change
    if (m_debugDirty) rebuildDebugVertices();
    window.draw(m_debugVertices);

    // Grille -- rebuild seulement si le zoom a change
    if (m_cachedGridStep != step) rebuildGrid();
    window.draw(m_gridVertices);

    // Curseur pinceau dans le monde
    sf::Vector2i mPosPixel = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(mPosPixel, m_view);
    sf::Vector2f mouseF(static_cast<float>(mPosPixel.x), static_cast<float>(mPosPixel.y));
    const sf::FloatRect paletteRect = getPaletteRect();

    if (!paletteRect.contains(mouseF) &&
        worldPos.x >= 0 && worldPos.x < gridSizeX &&
        worldPos.y >= 0 && worldPos.y < gridSizeY)
    {
        float snapX = std::floor(worldPos.x / step) * step;
        float snapY = std::floor(worldPos.y / step) * step;

        if (TileType::isLogical(m_selectedTile)) {
            sf::RectangleShape brushLogic({ step, step });
            brushLogic.setPosition({ snapX, snapY });
            sf::Color c = getTileDebugColor(m_selectedTile);
            c.a = 180;
            brushLogic.setFillColor(c);
            window.draw(brushLogic);

            // Triangle directionnel (même logique que les tuiles posées)
            float cx = snapX + step * 0.5f;
            float cy = snapY + step * 0.5f;
            float r = step * 0.28f;
            float tipX = cx + (m_flipX ? r : -r);
            float baseY = cy + (m_flipY ? r : -r);
            sf::ConvexShape arr(3);
            arr.setPoint(0, { tipX, cy });
            arr.setPoint(1, { cx - r * (m_flipX ? -1.f : 1.f) * 0.5f, baseY });
            arr.setPoint(2, { cx + r * (m_flipX ? -1.f : 1.f) * 0.5f, baseY });
            arr.setFillColor(sf::Color(255, 255, 255, 230));
            window.draw(arr);
        }
        else {
            m_brushPreview.setPosition({ snapX, snapY });
            m_brushPreview.setScale({ m_scale, m_scale });
            window.draw(m_brushPreview);
        }
    }

    // -----------------------------------------------------------------------
    // 2. PHASE INTERFACE (éléments fixes — on repasse en vue par défaut)
    // -----------------------------------------------------------------------
    window.setView(window.getDefaultView());

    // Fond de la palette
    window.draw(m_paletteBackground);

    // Sprite du tileset
    m_paletteSprite.setPosition(m_palettePos); // ← une seule ligne, toujours cohérent
    m_paletteSprite.setScale({ m_scale, m_scale });
    window.draw(m_paletteSprite);

    // Surbrillance de la tuile sélectionnée (uniquement pour les tuiles visuelles)
    if (TileType::isVisual(m_selectedTile)) {
        const sf::Texture& tex = AssetManager::getTexture(currentTilesetId());
        const int tilesPerRow = tex.getSize().x / m_tileSize.x;
        int tx = m_selectedTile % tilesPerRow;
        int ty = m_selectedTile / tilesPerRow;
        sf::RectangleShape highlight({ step, step });
        highlight.setPosition({
            m_palettePos.x + tx * step,
            m_palettePos.y + ty * step
            });
        highlight.setFillColor(sf::Color(255, 255, 100, 80));
        highlight.setOutlineColor(sf::Color::Yellow);
        highlight.setOutlineThickness(2.f);
        window.draw(highlight);
    }

    // Indicateur de couche
    m_layerIndicator.setPosition({ 20.f, 20.f });
    window.draw(m_layerIndicator);

    window.setView(m_view); // On remet la vue monde pour le prochain frame
}

// =============================================================================
//  Sauvegarde / Chargement
// =============================================================================

void EditorScene::saveToFile(const std::string& filename)
{
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erreur: Impossible d'ouvrir le fichier : " << filename << std::endl;
        return;
    }

    file << "width " << m_width << " height " << m_height << "\n";

    for (int l = 0; l < 4; ++l) {
        file << "layer " << l << "\n";
        for (unsigned int i = 0; i < m_layers[l].size(); ++i) {
            const auto& tile = m_layers[l][i];
            if (l == 1 || l == 3) {
                // Format : symbole flipX flipY  (même convention que les layers visuels)
                std::string sym = "0";
                if (tile.id == TileType::COLLISION)    sym = "#";
                else if (tile.id == TileType::SPAWN_PLAYER) sym = "&";
                else if (tile.id == TileType::SPIKE)        sym = "@";
                else if (tile.id == TileType::JUMPB)        sym = "^";
                else if (tile.id == TileType::GATE_SHIP)    sym = ",";
                else if (tile.id == TileType::GATE_CUBE)    sym = "=";
                else if (tile.id == TileType::GATE_BALL)    sym = "O";
                else if (tile.id == TileType::GATE_UFO)     sym = "ù";
                else if (tile.id == TileType::GATE_WAVE)    sym = "~";
                else if (tile.id == TileType::GATE_GRAVITY) sym = "!";
                else if (tile.id == TileType::VICTORY)      sym = "$";
                file << sym << " "
                    << (tile.flipX ? 1 : 0) << " "
                    << (tile.flipY ? 1 : 0) << " ";
            }
            else {
                // Format visuel : id flipX flipY tilesetIdx
                file << tile.id << " "
                    << (tile.flipX ? 1 : 0) << " "
                    << (tile.flipY ? 1 : 0) << " "
                    << tile.tilesetIdx << " ";
            }
            if ((i + 1) % m_width == 0) file << "\n";
        }
    }

    file.close();
    std::cout << "Carte sauvegardee : " << filename << std::endl;
}

void EditorScene::loadFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erreur: Impossible d'ouvrir le fichier : " << filename << std::endl;
        return;
    }

    std::string token;
    int currentLayer = -1;
    int tileIdx = 0;

    while (file >> token) {
        if (token == "width") {
            file >> m_width;
        }
        else if (token == "height") {
            file >> m_height;
            for (int i = 0; i < 4; ++i)
                m_layers[i].assign(m_width * m_height, { -1, false, false });
        }
        else if (token == "layer") {
            file >> currentLayer;
            tileIdx = 0;
        }
        else if (currentLayer != -1 && tileIdx < (int)(m_width * m_height)) {
            if (currentLayer == 1 || currentLayer == 3) {
                int tileId = TileType::NONE;
                if (token == "#") tileId = TileType::COLLISION;
                else if (token == "&") tileId = TileType::SPAWN_PLAYER;
                else if (token == "@") tileId = TileType::SPIKE;
                else if (token == "^") tileId = TileType::JUMPB;
                else if (token == ",") tileId = TileType::GATE_SHIP;
                else if (token == "=") tileId = TileType::GATE_CUBE;
                else if (token == "O") tileId = TileType::GATE_BALL;
                else if (token == "ù") tileId = TileType::GATE_UFO;
                else if (token == "~") tileId = TileType::GATE_WAVE;
                else if (token == "!") tileId = TileType::GATE_GRAVITY;
                else if (token == "$") tileId = TileType::VICTORY;
                // "0" ou token numérique = case vide, tileId reste NONE

                // Lire flipX flipY (présents même pour les logiques depuis la nouvelle version)
                int fx = 0, fy = 0;
                file >> fx >> fy;

                m_layers[currentLayer][tileIdx] = { tileId, (bool)fx, (bool)fy };
            }
            else {
                // Format visuel : id flipX flipY tilesetIdx
                int id, fx, fy, ts = 0;
                id = std::stoi(token);
                file >> fx >> fy >> ts;
                m_layers[currentLayer][tileIdx] = { id, (bool)fx, (bool)fy, ts };
            }
            tileIdx++;
        }
    }

    file.close();
    updateMap();
    std::cout << "Carte chargee : " << m_width << "x" << m_height << std::endl;
}

// Rebuild complet : appele au chargement ou au switchTileset
void EditorScene::updateMap()
{
    const int numTilesets = static_cast<int>(m_tilesetIds.size());
    for (int layer = 0; layer < 4; ++layer)
        for (int ts = 0; ts < numTilesets; ++ts)
            updateMap(layer, ts);
    m_debugDirty = true;
}

// Rebuild partiel : reconstruit UN seul sous-tilemap (layer x tileset).
// Appele apres chaque pose/effacement de tuile visuelle.
void EditorScene::updateMap(int layer, int tilesetIdx)
{
    std::vector<TileData> filtered(m_layers[layer].size(), { -1, false, false, tilesetIdx });
    for (size_t i = 0; i < m_layers[layer].size(); ++i) {
        const TileData& td = m_layers[layer][i];
        if (TileType::isVisual(td.id) && td.tilesetIdx == tilesetIdx)
            filtered[i] = td;
    }
    m_tileMaps[layer][tilesetIdx].load(
        AssetManager::getTexture(m_tilesetIds[tilesetIdx]),
        m_tileSize, filtered, m_width, m_height, m_scale
    );
}

// Grille : un seul VertexArray, rebuild uniquement quand le zoom change.
void EditorScene::rebuildGrid()
{
    const float step = m_tileSize.x * m_scale;
    m_cachedGridStep = step;
    const float gridSizeX = m_width * step;
    const float gridSizeY = m_height * step;

    m_gridVertices.clear();
    m_gridVertices.setPrimitiveType(sf::PrimitiveType::Lines);
    const sf::Color gridColor(255, 255, 255, 40);

    for (float x = 0; x <= gridSizeX; x += step) {
        m_gridVertices.append(sf::Vertex({ x, 0 }, gridColor));
        m_gridVertices.append(sf::Vertex({ x, gridSizeY }, gridColor));
    }
    for (float y = 0; y <= gridSizeY; y += step) {
        m_gridVertices.append(sf::Vertex({ 0,         y }, gridColor));
        m_gridVertices.append(sf::Vertex({ gridSizeX, y }, gridColor));
    }
}

// Debug rects + fleches : un seul VertexArray pour toutes les tuiles logiques.
// Appele seulement quand m_debugDirty == true.
void EditorScene::rebuildDebugVertices()
{
    m_debugVertices.clear();
    m_debugVertices.setPrimitiveType(sf::PrimitiveType::Triangles);

    const float step = m_tileSize.x * m_scale;

    for (int a : {1, 3}) {
        for (unsigned int i = 0; i < m_layers[a].size(); ++i) {
            const TileData& td = m_layers[a][i];
            if (!TileType::isLogical(td.id)) continue;
            sf::Color c = getTileDebugColor(td.id);
            if (c == sf::Color::Transparent) continue;

            float px = static_cast<float>(i % m_width) * step;
            float py = static_cast<float>(i / m_width) * step;

            // Rectangle (2 triangles)
            sf::Vector2f p0(px, py);
            sf::Vector2f p1(px + step, py);
            sf::Vector2f p2(px + step, py + step);
            sf::Vector2f p3(px, py + step);
            m_debugVertices.append(sf::Vertex(p0, c));
            m_debugVertices.append(sf::Vertex(p1, c));
            m_debugVertices.append(sf::Vertex(p2, c));
            m_debugVertices.append(sf::Vertex(p0, c));
            m_debugVertices.append(sf::Vertex(p2, c));
            m_debugVertices.append(sf::Vertex(p3, c));

            // Fleche directionnelle
            float cx = px + step * 0.5f;
            float cy = py + step * 0.5f;
            float r = step * 0.28f;
            float tipX = cx + (td.flipX ? r : -r);
            float baseY = cy + (td.flipY ? r : -r);
            float sign = td.flipX ? -1.f : 1.f;
            sf::Color arrowColor(255, 255, 255, 200);
            m_debugVertices.append(sf::Vertex({ tipX,                      cy }, arrowColor));
            m_debugVertices.append(sf::Vertex({ cx - r * sign * 0.5f, baseY }, arrowColor));
            m_debugVertices.append(sf::Vertex({ cx + r * sign * 0.5f, baseY }, arrowColor));
        }
    }
    m_debugDirty = false;
}