#include "Options.h"
#include "Engine.h"


Engine::Engine(unsigned w, unsigned h, const char* title)
    : window(sf::VideoMode({ w, h }), title)
{
    window.setFramerateLimit(60);
}


void Engine::handleEvents() {
    while (const std::optional<sf::Event> e = window.pollEvent()) {
        if (e->is<sf::Event::Closed>())
            window.close();

        if (sceneManager.get()) {
            sceneManager.get()->handleEvent(*e);
        }
    }
}

void Engine::run() {
    while (window.isOpen()) {
        // ON APPELLE UNIQUEMENT CECI POUR LES ÉVÉNEMENTS
        handleEvents();

        float dt = clock.restart().asSeconds();

        if (sceneManager.get())
            sceneManager.get()->update(dt);

        window.clear();

        if (sceneManager.get())
            sceneManager.get()->draw(window);

        window.display();
    }
}