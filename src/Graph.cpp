#include "Graph.hpp"

constexpr float REPULSION_STRENGTH = 0.1f;
constexpr float SPRING_STRENGTH = 0.02f;

void Graph::addNode(const sf::Vector2f &position, const sf::Font &font) {
  nodes.emplace_back(position, nodes.size(), font);
}

void Graph::addEdge(int firstNodeId, int secondNodeId) {
  if (hasEdge(firstNodeId, secondNodeId))
    return;
  edges.emplace_back(
      firstNodeId, secondNodeId,
      distance(nodes[firstNodeId].position, nodes[secondNodeId].position));
}

bool Graph::hasEdge(int firstNodeId, int secondNodeId) {
  for (auto &e : edges)
    if ((e.firstNodeId == firstNodeId && e.secondNodeId == secondNodeId) ||
        (e.firstNodeId == secondNodeId && e.secondNodeId == firstNodeId))
      return true;
  return false;
}

void Graph::updatePhysics(int draggedId) {
  for (size_t i = 0; i < nodes.size(); i++) {
    for (size_t j = i + 1; j < nodes.size(); j++) {
      float dist = distance(nodes[i].position, nodes[j].position);
      float minDist = nodes[i].radius + nodes[j].radius + 2;
      if (dist < minDist && dist > 0.01f) {
        auto delta = (nodes[j].position - nodes[i].position) / dist;
        auto push = delta * (minDist - dist) * REPULSION_STRENGTH;
        if ((int)i != draggedId)
          nodes[i].position -= push;
        if ((int)j != draggedId)
          nodes[j].position += push;
      }
    }
  }

  for (auto &e : edges) {
    float dist =
        distance(nodes[e.firstNodeId].position, nodes[e.secondNodeId].position);
    if (dist > 0.01f) {
      auto dir =
          (nodes[e.secondNodeId].position - nodes[e.firstNodeId].position) /
          dist;
      float force = (dist - e.weight) * SPRING_STRENGTH;
      if (e.firstNodeId != draggedId)
        nodes[e.firstNodeId].position += dir * force;
      if (e.secondNodeId != draggedId)
        nodes[e.secondNodeId].position -= dir * force;
    }
  }
}

void Graph::updateNodes() {
  for (auto &n : nodes)
    n.update();
}

void Graph::draw(sf::RenderWindow &window, const sf::Font &font,
                 int editingEdge, const std::string &weightInput) {
  for (int i = 0; i < (int)edges.size(); i++) {
    auto &e = edges[i];
    auto color = e.IsSelected ? sf::Color::Red : sf::Color::White;

    sf::Vertex line[] = {sf::Vertex(nodes[e.firstNodeId].position, color),
                         sf::Vertex(nodes[e.secondNodeId].position, color)};
    window.draw(line, 2, sf::Lines);

    auto mid =
        (nodes[e.firstNodeId].position + nodes[e.secondNodeId].position) / 2.f;
    sf::Text text;
    text.setFont(font);
    text.setString((i == editingEdge && !weightInput.empty())
                       ? weightInput
                       : std::to_string((int)e.weight));
    text.setFillColor(i == editingEdge ? sf::Color::Yellow : sf::Color::Green);
    text.setCharacterSize(18);
    auto b = text.getLocalBounds();
    text.setOrigin(b.width / 2, b.height / 2);
    text.setPosition(mid + sf::Vector2f(1, 1));
    window.draw(text);
  }
  for (auto &n : nodes)
    n.draw(window);
}

void Graph::clear() {
  nodes.clear();
  edges.clear();
}
