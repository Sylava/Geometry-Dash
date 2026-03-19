#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>

class AssetManager {
public:
    // --- Textures ---
    static sf::Texture& getTexture(const std::string& id);

    // --- Sounds ---
    static sf::SoundBuffer& getSound(const std::string& id);
    // --- Fonts ---
    static sf::Font& getFont(const std::string& id);


    // --- Loaders ---
    static void loadTexture(const std::string& id, const std::string& path);
    static void loadSound(const std::string& id, const std::string& path);
    static void loadFont(const std::string& id, const std::string& path);

private:
    static std::unordered_map<std::string, sf::Texture> textures;
    static std::unordered_map<std::string, sf::SoundBuffer> sounds;
    static std::unordered_map<std::string, sf::Font> fonts;
};

