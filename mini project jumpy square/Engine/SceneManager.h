#pragma once
#include <memory>
#include "Scene.h"
#include "Options.h"

class SceneManager {
public:
    void setScene(std::unique_ptr<Scene> newScene);

    void handleEvent(const sf::Event& e);
    void update(float dt);
    void draw(sf::RenderWindow& window);

    Scene* get();

    Options option;
private:
    std::unique_ptr<Scene> current;
};
