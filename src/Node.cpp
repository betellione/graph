#include "Node.hpp"

constexpr float NODE_RADIUS_MAX = 15.f;
constexpr float NODE_GROWTH_SPEED = 0.5f;

Node::Node(const sf::Vector2f& p, int index, const sf::Font& font)
    : position(p), radius(0.f), IsGrowing(true)
{
    shape.setRadius(radius);
    shape.setFillColor(sf::Color(100, 150, 250));
    shape.setOrigin(radius, radius);
    shape.setPosition(position);

    label.setFont(font);
    label.setString(std::to_string(index));
    label.setCharacterSize(14);
    label.setFillColor(sf::Color::White);
    label.setPosition(position);
}

void Node::update()
{
    if (IsGrowing && radius < NODE_RADIUS_MAX)
    {
        radius += NODE_GROWTH_SPEED;
        shape.setRadius(radius);
        shape.setOrigin(radius, radius);
    }
    else
    {
        IsGrowing = false;
    }
    shape.setPosition(position);
    auto bounds = label.getLocalBounds();
    label.setOrigin(bounds.width / 2, bounds.height / 2);
    label.setPosition(position);
}

void Node::draw(sf::RenderWindow& window)
{
    window.draw(shape);
    window.draw(label);
}
