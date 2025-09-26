#pragma once
#include <SFML/Graphics.hpp>

float distance(sf::Vector2f firstNode, sf::Vector2f secondNode);
bool isPointNearLine(sf::Vector2f pointer, sf::Vector2f firstNode, sf::Vector2f secondNode,
                     float tolerance = 10.f);
