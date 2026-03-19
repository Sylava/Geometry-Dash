#pragma once
#include <SFML/Graphics.hpp>

class SceneManager;

class Scene {
public:
    virtual ~Scene() = default;

    // Pointeur vers le manager
    void setManager(SceneManager* sm) { m_manager = sm; }

    virtual void onEnter() {}
    virtual void onExit() {}

    virtual void handleEvent(const sf::Event& e) = 0;
    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;

protected:
    SceneManager* m_manager = nullptr;
};
