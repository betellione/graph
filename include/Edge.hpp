#pragma once
#include <SFML/Graphics.hpp>

struct Edge
{
    int firstNodeId, secondNodeId;
    float weight;
    bool IsSelected;

    Edge(int firstNodeId, int secondNodeId, float weight);
};
