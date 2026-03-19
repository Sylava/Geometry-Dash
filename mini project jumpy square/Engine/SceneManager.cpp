#include "SceneManager.h"

void SceneManager::setScene(std::unique_ptr<Scene> newScene) {
    if (current) current->onExit();
    current = std::move(newScene);

    if (current) {
        current->setManager(this); // La scène sait maintenant qui est son manager
        current->onEnter();
    }
}


void SceneManager::handleEvent(const sf::Event& e) {
    if (current) current->handleEvent(e);
}

void SceneManager::update(float dt) {
    if (current) current->update(dt);
}

void SceneManager::draw(sf::RenderWindow& window) {
    if (current) current->draw(window);
}

Scene* SceneManager::get() {
    return current.get();
}
