#pragma once
#include <SFML/Graphics.hpp>
#include "SceneManager.h"

class Engine {
public:
    Engine(unsigned w, unsigned h, const char* title);

    void run();

    SceneManager& scenes() { return sceneManager; }


private:
    void handleEvents();

    sf::RenderWindow window;
    sf::Clock clock;
    SceneManager sceneManager;
};
