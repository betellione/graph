#include "Edge.hpp"

Edge::Edge(int firstNodeId, int secondNodeId, float weight)
    : firstNodeId(firstNodeId), secondNodeId(secondNodeId), weight(weight),
      IsSelected(false) {}
