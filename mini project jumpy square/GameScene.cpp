#include <SFML/Graphics.hpp>
#include <random>
#include <fstream>
#include <iostream>
#include <string>
#include "GameScene.h"
#include "MenuScene.h"
#include "Player.h"
#include "Gate.h"
#include "Engine/Engine.h"

GameScene::GameScene(const std::string& levelFile)
    : attemptsText(AssetManager::getFont("essais_font"), "Attempt 1", 40),
    m_levelFile(levelFile)
{
    attemptsText.setFillColor(sf::Color::White);
    attemptsText.setPosition({ 50.f, 900.f });
    collision = new CollisionManager(&player, &spike, &gates, &jumpBalls, &currentPlane);
}
void GameScene::onEnter()
{
    m_view.setSize({ 1850, 980 });
    m_view.setCenter({ 500.f, 500.f });

    // --- Backgrounds par plan ---
    // Plan 0 (arriere / fantasy) : 2 layers a des vitesses differentes pour l'effet parallaxe
    // Plan 1 (avant / scifi)    : 1 layer seul
    struct BgDef { const char* id; float speed; int plane; };
    BgDef bgDefs[] = {
        { "Fantasy_bg_base",  0.15f, 0 }, // base : lente (decor lointain)
        { "Fantasy_bg_nuage", 0.30f, 0 }, // nuages : plus rapides (plus proches)
        { "Scifi_bg",         0.25f, 1 }, // scifi : plan avant, seul
    };
    for (auto& def : bgDefs) {
        sf::Texture& tex = AssetManager::getTexture(def.id);
        tex.setRepeated(true);
        ParallaxLayer layer(tex, def.speed, def.plane);
        // Scale pour couvrir la hauteur de la fenetre (980px)
        float scaleY = 980.f / tex.getSize().y;
        layer.sprite.setScale({ scaleY, scaleY });
        layer.sprite.setTextureRect(sf::IntRect({ 0, 0 }, { 10000, (int)tex.getSize().y }));
        m_backgroundLayers.push_back(layer);
    }


    loadMap(m_levelFile);

    updateMap();

    //menue additions
    sf::Font& font = AssetManager::getFont("main_font");
    m_options.emplace_back(font, "Restart", 60);
    m_options.emplace_back(font, "Return to menu", 60);
    updateSelection();
    optionbackground.setFillColor(sf::Color::Color({ 255, 100, 0, 100 }));
    optionbackground.setSize({ 1850, 980 });
    optionbackground.setPosition({ 0.f,0.f });

    m_musicFantasy.openFromFile("Asset_game/music/openmindaudio-cinematic-mandolin-adventure-music-echoes-beyond-the-emerald-pass-502903.mp3");
    m_musicFantasy.setLooping(true);
    m_musicScifi.openFromFile("Asset_game/music/freemusiclab-dark-cyberpunk-i-free-background-music-i-free-music-lab-release-469493.mp3"); 
    m_musicScifi.setLooping(true);

    // Démarrer la musique du plan initial (plan 0 = fantasy)
    m_musicOffset = sf::Time::Zero;
    m_musicFantasy.play();

    // LASER POOL
    for (int i = 0; i < SWITCH_POOL_SIZE; ++i) {
        switchSounds.emplace_back(AssetManager::getSound("switch_sound"));
        switchSounds.back().setVolume(100.f);
    }

    if (!m_inverseShader.loadFromFile("Asset_game/Shader/inverse.frag", sf::Shader::Type::Fragment)) {
        std::cout << "Erreur : Impossible de charger le shader !" << std::endl;
    }
    m_inverseShader.setUniform("texture", sf::Shader::CurrentTexture);

    if (sf::Shader::isAvailable()) {
        m_overlayReady = m_overlayShader.loadFromFile(
            "Asset_game/Shader/gameplay_overlay.frag",
            sf::Shader::Type::Fragment);
        if (m_overlayReady) {
            m_overlayRect.setSize({ 1850.f, 980.f });
            m_overlayRect.setFillColor(sf::Color::White);
            m_overlayShader.setUniform("resolution", sf::Glsl::Vec2(1850.f, 980.f));
        }
    }
}

void GameScene::onExit()
{
    m_musicFantasy.stop();
    m_musicScifi.stop();
    delete collision;
}
void GameScene::handleEvent(const sf::Event& e)
{
    if (const auto* key = e.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Space) {
            player.jump();
        }
        else if (key->code == m_manager->option.k_option) {
            paused = !paused;
        }

        if (paused) {
            if (key->code == sf::Keyboard::Key::Up) {
                m_selectedIndex = (m_selectedIndex - 1 + m_options.size()) % m_options.size();
                updateSelection();
            }
            if (key->code == sf::Keyboard::Key::Down) {
                m_selectedIndex = (m_selectedIndex + 1) % m_options.size();
                updateSelection();
            }
            if (key->code == sf::Keyboard::Key::Enter) {
                if (m_selectedIndex == 0) { // "restart"
                    paused = !paused;
                    die();
                    return;
                }
                else if (m_selectedIndex == 1) { // "Retour"
                    m_manager->setScene(std::make_unique<MenuScene>());
                    return;
                }
            }
        }
        if (victoryScreen) {
            victoryScreen->handleEvent(e);
            if (victoryScreen->goToMenu())
            {
                m_manager->setScene(std::make_unique<MenuScene>());
            }
            return;
        }
    }
}
void GameScene::spawnParticles(sf::Vector2f pos)
{
    for (int i = 0; i < 20; i++)
    {
        Particle p;

        p.shape = sf::CircleShape(3);
        p.shape.setFillColor(sf::Color::Yellow);
        p.shape.setPosition(pos);

        float angle = static_cast<float>(rand() % 360) * 3.14159f / 180.f;
        float speed = static_cast<float>((rand() % 200) + 100);

        p.velocity = { cos(angle) * speed, sin(angle) * speed };

        p.lifetime = 0.5f + static_cast<float>(rand() % 50) / 100.f;

        particles.push_back(p);
    }
}

void GameScene::update(float dt)
{
    if (!paused) {
        bool jumpPressed = (sf::Keyboard::isKeyPressed(m_manager->option.k_jump) || sf::Keyboard::isKeyPressed(m_manager->option.k_jump_alt) || (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && m_manager->option.mouse_ctrl));

        float moveX = player.getSpeed() * dt;
        float moveY = player.getVelocityY() * dt;

        switchCooldown -= dt;
        m_switchFlash = 1.f;  // déclenche le flash
        if (switchCooldown < 0.f) switchCooldown = 0.f;

        if (switchCooldown == 0.f && (sf::Keyboard::isKeyPressed(m_manager->option.k_switch)|| sf::Keyboard::isKeyPressed(m_manager->option.k_switch_alt) || (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) && m_manager->option.mouse_ctrl)) && !m_planeKeyWasPressed) {
            currentPlane = (currentPlane == 1) ? 0 : 1;
            player.changeTheme();
            // jouer le son
            switchSounds[currentswitchSound].play();
            currentswitchSound = (currentswitchSound + 1) % SWITCH_POOL_SIZE;
            m_planeKeyWasPressed = true; // Pour éviter de switch 60 fois par seconde

            switchCooldown = 0.1f;

            // Swap musique — à l'intérieur du if
            sf::Music& current = (currentPlane == 0) ? m_musicScifi : m_musicFantasy;
            sf::Music& next = (currentPlane == 0) ? m_musicFantasy : m_musicScifi;
            m_musicOffset = current.getPlayingOffset();
            current.stop();
            next.setPlayingOffset(m_musicOffset);
            next.play();
        }

        // Si l'écran de victoire n'est pas encore actif
        for (auto& v : victoryTiles) {
            if (v.getBounds().findIntersection(player.getBounds())) {
                if (!victoryScreen) {
                    victoryScreen = std::make_unique<Victory>(
                        AssetManager::getFont("main_font"),
                        sf::Vector2f(1850, 980),
                        attempts
                    );
                }
                break;
            }
        }

        // Mettre à jour l'écran de victoire si actif
        if (victoryScreen) {
            /*std::cout << "your a WINAR " << std::endl;*/
            victoryScreen->update(dt);

            //if (victoryScreen->continueGame()) {
            //    victoryScreen.reset();
            //    // TODO: charger prochain niveau ici
            //}

            //if (victoryScreen->goToMenu()) {
            //    m_manager->setScene(std::make_unique<MenuScene>());
            //}

            return;
        }
        if (!(sf::Keyboard::isKeyPressed(m_manager->option.k_switch) || sf::Keyboard::isKeyPressed(m_manager->option.k_switch_alt) || (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) && m_manager->option.mouse_ctrl)))
            m_planeKeyWasPressed = false;

        checkGateCollision();
        if (collision->ballCollision())
            player.canDoubleJump = true;
        else
            player.canDoubleJump = false;

        if (collision->spikeCollision() && !m_manager->option.no_collision)
            die();

        horizontalCollision(dt); //collision horizontale
        verticalCollision(dt); //collision verticale
        player.update(dt, jumpPressed);

        parallaxeUpdate(dt); // update du parallaxe
        cameraUpdate(dt);

        m_overlayTime += dt;
        if (m_switchFlash > 0.f)
            m_switchFlash = std::max(0.f, m_switchFlash - dt * 3.5f); // fondu en ~0.3s
    }
}

void GameScene::cameraUpdate(float dt)
{
    sf::Vector2f playerPos = player.getPosition();
    sf::Vector2f currentCenter = m_view.getCenter();

    // 1. Suivi Horizontal (on garde ton décalage de 250 pour anticiper la course)
    float targetX = playerPos.x + 250;

    // 2. Suivi Vertical avec "Amorti" (Lerp)
    // On définit une vitesse de suivi (plus le chiffre est bas, plus c'est fluide/lent)
    float verticalLerpSpeed = 5.0f;

    // On ne suit le Y que si l'écart est significatif (évite les tremblements sur les micro-sauts)
    float targetY = currentCenter.y;
    float verticalThreshold = 50.f; // "Dead zone" de 50 pixels

    if (std::abs(playerPos.y - currentCenter.y) > verticalThreshold) {
        // Formule du Lerp : ValeurActuelle + (Cible - ValeurActuelle) * Vitesse * DeltaTime
        targetY = currentCenter.y + (playerPos.y - currentCenter.y) * verticalLerpSpeed * dt;
    }
    m_view.setCenter({ targetX, targetY });
}

void GameScene::parallaxeUpdate(float dt)
{
    sf::Vector2f viewCenter = m_view.getCenter();
    sf::Vector2f viewSize = m_view.getSize();
    sf::Vector2f topLeft = viewCenter - viewSize / 2.f;

    for (auto& layer : m_backgroundLayers) {
        // 1. On calcule le décalage de la texture (vitesse horizontale)
        // On récupère le rectangle actuel
        sf::IntRect textureRect = layer.sprite.getTextureRect();

        // On change la position 'left' pour faire défiler la texture
        // On multiplie par le speedFactor pour l'effet de profondeur
        textureRect.position.x = static_cast<int>(viewCenter.x * layer.speedFactor);

        layer.sprite.setTextureRect(textureRect);

        // 2. On repositionne le sprite pour qu'il soit toujours devant la caméra
        layer.sprite.setPosition(topLeft);
    }
    float gravity = 900.f;

    for (size_t i = 0; i < particles.size(); )
    {
        Particle& p = particles[i];

        p.lifetime -= dt;

        if (p.lifetime <= 0)
        {
            particles.erase(particles.begin() + i);
        }
        else
        {
            p.velocity.y += gravity * dt;
            p.shape.move(p.velocity * dt);
            i++;
        }
    }
}

void GameScene::horizontalCollision(float dt)
{
    float moveX = player.getSpeed() * dt;
    float stepMove = 4.f;
    float moved = 0.f;

    while (moved < moveX)
    {
        float step = std::min(stepMove, moveX - moved);

        sf::Vector2f pos = player.getPosition();

        if (checkCollisionT({ pos.x + 21.f + step, pos.y - 20.f }) && !m_manager->option.no_collision) {
            die();
            return;
        }

        player.move({ step, 0.f });
        moved += step;
    }
}

void GameScene::verticalCollision(float dt)
{
    float moveY = player.getVelocityY() * dt;
    sf::Vector2f pos = player.getPosition();
    float step = m_tileSize.x * m_scale;

    // Bas et tête du cube
    sf::Vector2f bottom = { pos.x, pos.y + moveY };
    float headY = pos.y - player.getSize().y;
    sf::Vector2f top = { pos.x, headY + moveY };

    if (player.mode == Type::WAVE)
    {
        player.move({ 0.f, player.getVelocityY() * dt });
        return;
    }
    if (moveY > 0 && checkCollisionT(bottom)) // SOL
    {
        player.setVelocityY(0.f);
        player.setOnGround(true);
        player.setAtCeiling(false);

        int gy = (int)(bottom.y / step); // tuile touchée
        player.setPosition({ pos.x, gy * step }); // bas sur le sol
    }
    else if (moveY < 0 && checkCollisionT(top)) // PLAFOND
    {
        player.setVelocityY(0.f);
        player.setAtCeiling(true);
        player.setOnGround(false);

        int gy = (int)(top.y / step); // tuile plafond touchée
        player.setPosition({ pos.x, (gy + 1) * step + player.getSize().y }); // bas juste sous plafond
    }
    else // Pas de collision verticale
    {
        player.move({ 0.f, moveY });
        player.setOnGround(false);
        player.setAtCeiling(false);
    }
}

void GameScene::checkGateCollision()
{
    for (auto& gate : gates)
    {
        if (gate.getPlane() != currentPlane)
            continue;
        if (player.getBounds().findIntersection(gate.getBounds()).has_value() && gate.type != player.mode)
        {
            switch (gate.type)
            {
            case Type::CUBE:
                player.transformCube();
                break;

            case Type::SHIP:
                player.transformShip();
                player.transformSprite->setRotation(sf::degrees(0));
                break;

            case Type::BALL:
                player.transformBall();
                player.transformSprite->setRotation(sf::degrees(0));
                break;
            case Type::WAVE:
                player.transformWave();
                break;
            default:
                break;
            }
        }
    }
}

void GameScene::loadMap(const std::string& filename) {
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
            // On redimensionne nos vecteurs dès qu'on connaît la taille
            for (int i = 0; i < 4; ++i) {
                m_layers[i].assign(m_width * m_height, { -1, false, false, 0 });
            }
        }
        else if (token == "layer") {
            file >> currentLayer;
            tileIdx = 0; // On recommence au début de la grille pour chaque nouvelle couche
        }
        else if (currentLayer != -1 && tileIdx < (int)(m_width * m_height)) {
            int objectPlane = (currentLayer < 2) ? 0 : 1;
            if (currentLayer == 1 || currentLayer == 3) {

                float step = m_tileSize.x * m_scale;
                int x = tileIdx % m_width;
                int y = tileIdx / m_width;
                sf::Vector2f worldPos(x * step + step / 2.f, y * step + step / 2.f);

                // --- TRADUCTION DES SYMBOLES ---
                int logicId = TileType::NONE;

                if (token == "#") { logicId = TileType::COLLISION; }
                else if (token == "&") { logicId = TileType::SPAWN_PLAYER; }
                else if (token == "@") { logicId = TileType::SPIKE; }
                else if (token == "^") { logicId = TileType::JUMPB; }
                else if (token == "=") { logicId = TileType::GATE_CUBE; }
                else if (token == ",") { logicId = TileType::GATE_SHIP; }
                else if (token == "O") { logicId = TileType::GATE_BALL; }
                else if (token == "ù") { logicId = TileType::GATE_UFO; }
                else if (token == "~") { logicId = TileType::GATE_WAVE; }
                else if (token == "!") { logicId = TileType::GATE_GRAVITY; }
                else if (token == "$") { logicId = TileType::VICTORY; }
                // "0" ou numerique = case vide, logicId reste NONE

                // Lire flipX flipY (presents dans le nouveau format)
                int fx = 0, fy = 0;
                file >> fx >> fy;
                bool flipX = (bool)fx;
                bool flipY = (bool)fy;

                m_layers[currentLayer][tileIdx] = { logicId, flipX, flipY, 0 };

                // Spawn des objets gameplay avec le flip
                if (logicId == TileType::SPAWN_PLAYER) {
                    player.setSpawnPoint(worldPos);
                    player.setPosition(worldPos);
                }
                else if (logicId == TileType::SPIKE) { spike.push_back(Spike(worldPos, objectPlane, flipX, flipY)); }
                else if (logicId == TileType::JUMPB) { jumpBalls.push_back(JumpBall(worldPos, objectPlane)); }
                else if (logicId == TileType::GATE_CUBE) { gates.push_back(Gate(Type::CUBE, worldPos, objectPlane, flipX, flipY)); }
                else if (logicId == TileType::GATE_SHIP) { gates.push_back(Gate(Type::SHIP, worldPos, objectPlane, flipX, flipY)); }
                else if (logicId == TileType::GATE_BALL) { gates.push_back(Gate(Type::BALL, worldPos, objectPlane, flipX, flipY)); }
                else if (logicId == TileType::GATE_UFO) { gates.push_back(Gate(Type::UFO, worldPos, objectPlane, flipX, flipY)); }
                else if (logicId == TileType::GATE_WAVE) { gates.push_back(Gate(Type::WAVE, worldPos, objectPlane, flipX, flipY)); }
                else if (logicId == TileType::GATE_GRAVITY) { gates.push_back(Gate(Type::GRAVITY, worldPos, objectPlane, flipX, flipY)); }
                else if (logicId == TileType::VICTORY) { victoryTiles.push_back(VictoryTrigger(worldPos)); }
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

    std::cout << "Carte chargee : " << m_width << "x" << m_height << std::endl;
}

void GameScene::updateMap()
{
    const int numTilesets = static_cast<int>(m_tilesetIds.size());

    for (int layer = 0; layer < 4; ++layer) {
        // Les layers logiques (1 et 3) n'ont pas de representation visuelle
        if (layer == 1 || layer == 3) continue;

        for (int ts = 0; ts < numTilesets; ++ts) {
            // Vue filtree : seules les tuiles de ce tileset sur ce layer
            std::vector<TileData> filtered(m_layers[layer].size(), { -1, false, false, ts });
            for (size_t i = 0; i < m_layers[layer].size(); ++i) {
                const TileData& td = m_layers[layer][i];
                if (TileType::isVisual(td.id) && td.tilesetIdx == ts)
                    filtered[i] = td;
            }
            m_tileMaps[layer][ts].load(
                AssetManager::getTexture(m_tilesetIds[ts]),
                m_tileSize, filtered, m_width, m_height, m_scale
            );
        }
    }
}

bool GameScene::checkCollisionT(sf::Vector2f pos)
{
    if (m_width == 0) return false;

    // Si le joueur est au plan 1 (avant), on check le layer 3. 
    // Si c'est le plan 0 (arrière), on check le layer 1.
    int logicLayerIndex = (currentPlane == 0) ? 1 : 3;

    float step = m_tileSize.x * m_scale;
    int gx = (int)(pos.x / step);
    int gy = (int)(pos.y / step);

    if (gx < 0 || gx >= (int)m_width || gy < 0 || gy >= (int)m_height) return false;

    return m_layers[logicLayerIndex][gx + gy * m_width].id == TileType::COLLISION;
}

void GameScene::die() {
    // Effet de particules à la position du joueur
    spawnParticles(player.getPosition());

    // Augmenter le compteur d'essais
    attempts++;
    attemptsText.setString("Attempt " + std::to_string(attempts));

    // Réinitialiser le joueur
    currentPlane = 0;

    m_musicScifi.stop();
    m_musicOffset = sf::Time::Zero;
    m_musicFantasy.setPlayingOffset(m_musicOffset);
    m_musicFantasy.play();

    player.reset();
    sf::Vector2f pos = player.getPosition();
    m_view.setCenter(pos);
   
}

void GameScene::draw(sf::RenderWindow& window)
{
    if (victoryScreen) {

        window.setView(m_view);

        victoryScreen->updatePosition(m_view);
        victoryScreen->draw(window);
        return;
    }
    sf::Shader* shaderPtr = &m_inverseShader;
    // caméra
    window.setView(m_view);
    // 1. On dessine le fond (Parallaxe)
    for (auto& layer : m_backgroundLayers) {
        if (layer.plane != currentPlane) continue;
        if (currentPlane == 0)
            window.draw(layer.sprite);
        else
            window.draw(layer.sprite);
    }

    const sf::Color activeColor(255, 255, 255, 255);
    const sf::Color inactiveColor(100, 100, 100, 200);

    // Plan actif = celui du joueur. Plan inactif = l'autre.
    // On dessine toujours : inactif en premier (fond), actif par-dessus.
    const int activePlan = currentPlane;
    const int inactivePlan = 1 - activePlan;
    const int inactiveBase = inactivePlan * 2;
    const int activeBase = activePlan * 2;
    const int numTilesets = static_cast<int>(m_tilesetIds.size());

    auto drawTileMap = [&](int layerIdx, sf::Color color) {
        for (int ts = 0; ts < numTilesets; ++ts) {
            m_tileMaps[layerIdx][ts].setColor(color);
            if (activePlan == 0)
                window.draw(m_tileMaps[layerIdx][ts]);
            else
                window.draw(m_tileMaps[layerIdx][ts], shaderPtr);
        }
        };
    auto drawObjects = [&](int plane, sf::Color color) {
        for (auto& pique : spike) {
            if (pique.getPlane() != plane) continue;
            pique.setColor(color);
            pique.draw(window, m_manager->option.show_hitbox);
        }
        for (auto& ball : jumpBalls) {
            if (ball.getPlane() != plane) continue;
            ball.setColor(color);
            ball.draw(window);
        }
        for (auto& gate : gates) {
            if (gate.getPlane() != plane) continue;
            gate.setColor(color);
            gate.draw(window, m_manager->option.show_hitbox);
        }
        };

    // ---- 1. Plan INACTIF (dessine en premier, reste en fond) ----
    drawTileMap(inactiveBase, inactiveColor);
    drawTileMap(inactiveBase + 1, inactiveColor);
    drawObjects(inactivePlan, inactiveColor);

    // ---- 2. Plan ACTIF (dessine par-dessus, jamais cache) ----
    drawTileMap(activeBase, activeColor);
    drawTileMap(activeBase + 1, activeColor);
    drawObjects(activePlan, activeColor);

    // ---- 3. Joueur et effets (toujours au premier plan) ----
    player.draw(window, m_manager->option.show_hitbox);
    for (auto& p : particles) window.draw(p.shape);
    window.draw(attemptsText);

    if (m_overlayReady) {
        window.setView(window.getDefaultView()); // vue fixe pour le quad plein écran
        m_overlayShader.setUniform("time", m_overlayTime);
        m_overlayShader.setUniform("currentPlane", currentPlane);
        m_overlayShader.setUniform("switchFlash", m_switchFlash);
        m_overlayRect.setPosition({ 0.f, 0.f });
        window.draw(m_overlayRect, &m_overlayShader);
        window.setView(m_view); // remettre la vue monde pour le menu pause
    }

    //option menue
    if (paused) {
        optionbackground.setPosition({ player.getPosition().x - 675,window.getView().getCenter().y - 490 });
        window.draw(optionbackground);
        int i = 0;
        for (auto& text : m_options) {
            text.setPosition({ player.getPosition().x,window.getView().getCenter().y + 80 * i });
            window.draw(text);
            i++;
        }
    }
}


void GameScene::updateSelection() {

    for (int i = 0; i < m_options.size(); ++i) {
        m_options[i].setFillColor(i == m_selectedIndex ? sf::Color::Yellow : sf::Color::White);
    }
}