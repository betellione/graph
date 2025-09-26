#include "utils.hpp"
#include <cmath>

float distance(sf::Vector2f a, sf::Vector2f b) {
  auto dx = a.x - b.x, dy = a.y - b.y;
  return std::sqrt(dx * dx + dy * dy);
}

bool isPointNearLine(sf::Vector2f p, sf::Vector2f a, sf::Vector2f b,
                     float tol) {
  auto ab = b - a;
  auto ap = p - a;
  float abLen2 = ab.x * ab.x + ab.y * ab.y;
  if (abLen2 < 1e-6f)
    return false;
  float proj = (ap.x * ab.x + ap.y * ab.y) / abLen2;
  proj = std::max(0.f, std::min(1.f, proj));
  auto closest = a + proj * ab;
  return distance(p, closest) < tol;
}
