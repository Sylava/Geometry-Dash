#include "AssetManager.h"
#include <stdexcept>
#include <iostream>

// Static storage
std::unordered_map<std::string, sf::Texture> AssetManager::textures;
std::unordered_map<std::string, sf::SoundBuffer> AssetManager::sounds;
std::unordered_map<std::string, sf::Font> AssetManager::fonts;

// ---------- TEXTURES ----------

void AssetManager::loadTexture(const std::string& id, const std::string& path) {
    sf::Texture tex;
    if (!tex.loadFromFile(path)) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return;
    }
    textures.emplace(id, std::move(tex));
}

sf::Texture& AssetManager::getTexture(const std::string& id) {
    return textures.at(id);
}

// ---------- SOUNDS ----------

void AssetManager::loadSound(const std::string& id, const std::string& path) {
    sf::SoundBuffer buf;
    if (!buf.loadFromFile(path))
        throw std::runtime_error("Failed to load sound: " + path);

    sounds.emplace(id, std::move(buf));
}

sf::SoundBuffer& AssetManager::getSound(const std::string& id) {
    return sounds.at(id);
}

// ---------- FONTS ----------

void AssetManager::loadFont(const std::string& id, const std::string& path) {
    sf::Font font;
    if (!font.openFromFile(path))
        throw std::runtime_error("Failed to load font: " + path);
    fonts.emplace(id, std::move(font));
    
}

sf::Font& AssetManager::getFont(const std::string& id) {
    return fonts.at(id);
}