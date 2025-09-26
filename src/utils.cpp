#include "utils.hpp"
#include <cmath>
#include <algorithm>

constexpr float REALLY_SMALL_NUMBER = 1e-6F;

auto distance(sf::Vector2f a, sf::Vector2f b) -> float
{
    auto dx = a.x - b.x, dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

auto isPointNearLine(sf::Vector2f p, sf::Vector2f a, sf::Vector2f b, float tol) -> bool
{
    auto ab = b - a;
    auto ap = p - a;
    float abLen2 = (ab.x * ab.x) + (ab.y * ab.y);
    if (abLen2 < REALLY_SMALL_NUMBER)
    {
        return false;
    }
    float proj = (ap.x * ab.x + ap.y * ab.y) / abLen2;
    proj = std::max(0.F, std::min(1.F, proj));
    auto closest = a + (proj * ab);
    return distance(p, closest) < tol;
}
