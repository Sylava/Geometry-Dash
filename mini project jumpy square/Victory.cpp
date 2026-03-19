#include "Victory.h"

Victory::Victory(sf::Font& font, const sf::Vector2f& size, int attempts) : title(AssetManager::getFont("main_font")),          // déjà ok
attemptsText(AssetManager::getFont("main_font")), menu(AssetManager::getFont("main_font"))
{
    // Fond sombre transparent
    m_background.setSize(size);
    m_background.setFillColor(sf::Color(0, 0, 0, 180));
    m_background.setPosition({ 0.f, 0.f });

    // TEXTE VICTOIRE
    title.setFont(font);
    title.setString("VICTOIRE !");
    title.setCharacterSize(100);
    title.setFillColor(sf::Color::Yellow);

    // Attempts ICI
    attemptsText.setFont(font);
    attemptsText.setString("Attempts: " + std::to_string(attempts));
    attemptsText.setCharacterSize(50);
    attemptsText.setFillColor(sf::Color::White);
    //menu
    menu.setFont(font);
    menu.setString("press ECHAP to go back to the menu");
    menu.setCharacterSize(20);
    menu.setFillColor(sf::Color::White);
    // Centrer le texte
    sf::FloatRect bounds = title.getLocalBounds();/*
    title.setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f });
    title.setPosition({ size.x / 2.f + 400, size.y / 2.f + 100 });*/
    sf::FloatRect bounds2 = attemptsText.getLocalBounds();/*
    attemptsText.setOrigin({ bounds2.size.x / 2.f, bounds2.size.y / 2.f });
    attemptsText.setPosition({ size.x / 2.f + 400, size.y / 2.f + 300 });*/
    sf::FloatRect bounds3 = menu.getLocalBounds();/*
    menu.setOrigin({ bounds3.size.x / 2.f, bounds3.size.y / 2.f });
    menu.setPosition({ size.x / 2.f + 400, size.y / 2.f + 800 });*/
}

void Victory::update(float dt)
{
    // logique pour l'écran de victoire
}
void Victory::handleEvent(const sf::Event& e)
{
    if (const auto* key = e.getIf<sf::Event::KeyPressed>())
    {
        if (key->code == sf::Keyboard::Key::Enter)
            m_continue = true;

        if (key->code == sf::Keyboard::Key::Escape)
            m_menu = true;
    }
}
void Victory::updatePosition(const sf::View& view)
{
    sf::Vector2f center = view.getCenter();

    m_background.setPosition(center - m_background.getSize() / 2.f);
    title.setPosition(center + sf::Vector2f(-400.f, -300.f));
    attemptsText.setPosition(center + sf::Vector2f(-400.f, -100.f));
    menu.setPosition(center + sf::Vector2f(-400.f, 200.f));
}

void Victory::draw(sf::RenderWindow& window)
{
    window.draw(m_background);
    window.draw(title);
    window.draw(attemptsText);
    window.draw(menu);
}