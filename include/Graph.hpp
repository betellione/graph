#pragma once
#include "Edge.hpp"
#include "Node.hpp"
#include "utils.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class Graph {
public:
  std::vector<Node> nodes;
  std::vector<Edge> edges;

  void addNode(const sf::Vector2f &position, const sf::Font &font);
  void addEdge(int firstNodeId, int secondNodeId);
  
  bool hasEdge(int firstNodeId, int secondNodeId);

  void updatePhysics(int draggedId);
  void updateNodes();

  void draw(sf::RenderWindow &window, const sf::Font &font,
            int editingEdge = -1, const std::string &weightInput = "");

  void clear();
};
