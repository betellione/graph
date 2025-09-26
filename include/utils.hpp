#pragma once
#include <SFML/Graphics.hpp>

float distance(sf::Vector2f a, sf::Vector2f b);
bool isPointNearLine(sf::Vector2f p, sf::Vector2f a, sf::Vector2f b,
                     float tol = 10.f);
