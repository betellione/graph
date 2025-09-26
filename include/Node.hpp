#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Node
{
   public:
    sf::Vector2f position;
    float radius;
    bool IsGrowing;
    sf::CircleShape shape;
    sf::Text label;

    Node(const sf::Vector2f& position, int index, const sf::Font& font);
    void update();
    void draw(sf::RenderWindow& window);
};
