#include "Graph.hpp"

constexpr float REPULSION_STRENGTH = 0.1F;
constexpr float SPRING_STRENGTH = 0.02F;

void Graph::addNode(const sf::Vector2f& position, const sf::Font& font)
{
    nodes.emplace_back(position, nodes.size(), font);
}

void Graph::addEdge(int firstNodeId, int secondNodeId)
{
    if (hasEdge(firstNodeId, secondNodeId))
    {
        return;
    }
    edges.emplace_back(firstNodeId, secondNodeId,
                       distance(nodes[firstNodeId].position, nodes[secondNodeId].position));
}

auto Graph::hasEdge(int firstNodeId, int secondNodeId) -> bool
{
    for (auto& edge : edges)
    {
        if ((edge.firstNodeId == firstNodeId && edge.secondNodeId == secondNodeId) ||
            (edge.firstNodeId == secondNodeId && edge.secondNodeId == firstNodeId))
        {
            return true;
        }
    }

    return false;
}

void Graph::updatePhysics(int draggedId)
{
    for (size_t i = 0; i < nodes.size(); i++)
    {
        for (size_t j = i + 1; j < nodes.size(); j++)
        {
            float dist = distance(nodes[i].position, nodes[j].position);
            float minDist = nodes[i].radius + nodes[j].radius + 2;
            if (dist < minDist && dist > 0.01F)
            {
                auto delta = (nodes[j].position - nodes[i].position) / dist;
                auto push = delta * (minDist - dist) * REPULSION_STRENGTH;
                if ((int) i != draggedId) nodes[i].position -= push;
                if ((int) j != draggedId) nodes[j].position += push;
            }
        }
    }

    for (auto& edge : edges)
    {
        float dist = distance(nodes[edge.firstNodeId].position, nodes[edge.secondNodeId].position);
        if (dist > 0.01f)
        {
            auto dir =
                (nodes[edge.secondNodeId].position - nodes[edge.firstNodeId].position) / dist;
            float force = (dist - edge.weight) * SPRING_STRENGTH;
            if (edge.firstNodeId != draggedId) nodes[edge.firstNodeId].position += dir * force;
            if (edge.secondNodeId != draggedId) nodes[edge.secondNodeId].position -= dir * force;
        }
    }
}

void Graph::updateNodes()
{
    for (auto& n : nodes) n.update();
}

void Graph::draw(sf::RenderWindow& window, const sf::Font& font, int editingEdge,
                 const std::string& weightInput)
{
    for (int i = 0; i < (int) edges.size(); i++)
    {
        auto& edge = edges[i];
        auto color = edge.IsSelected ? sf::Color::Red : sf::Color::White;

        sf::Vertex line[] = {sf::Vertex(nodes[edge.firstNodeId].position, color),
                             sf::Vertex(nodes[edge.secondNodeId].position, color)};
        window.draw(line, 2, sf::Lines);

        auto mid = (nodes[edge.firstNodeId].position + nodes[edge.secondNodeId].position) / 2.f;
        sf::Text text;
        text.setFont(font);
        text.setString((i == editingEdge && !weightInput.empty())
                           ? weightInput
                           : std::to_string((int) edge.weight));
        text.setFillColor(i == editingEdge ? sf::Color::Yellow : sf::Color::Green);
        text.setCharacterSize(18);
        auto b = text.getLocalBounds();
        text.setOrigin(b.width / 2, b.height / 2);
        text.setPosition(mid + sf::Vector2f(1, 1));
        window.draw(text);
    }
    for (auto& n : nodes) n.draw(window);
}

void Graph::clear()
{
    nodes.clear();
    edges.clear();
}
