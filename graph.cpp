#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <string>

struct Node {
    sf::Vector2f position;
    float radius = 0.f;
    bool IsGrowing = true;
    sf::CircleShape shape;
    sf::Text label;

    Node(const sf::Vector2f& p, int index, const sf::Font& font) : position(p) {
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

    void update() {
        if (IsGrowing && radius < 15.f) {
            radius += 0.5f;
            shape.setRadius(radius);
            shape.setOrigin(radius, radius);
        } else {
            IsGrowing = false;
        }
        shape.setPosition(position);

        sf::FloatRect textBounds = label.getLocalBounds();
        label.setOrigin(textBounds.width / 2, textBounds.height / 2);
        label.setPosition(position);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
        window.draw(label);
    }
};

struct Edge {
    int firstNode, secondNode;
    float weight;
    bool isSelected;

    Edge(int firstNode, int secondNode, float weight, bool isSelected = false): firstNode(firstNode), secondNode(secondNode), weight(weight), isSelected(isSelected) {}
};

class Graph {
public:
    std::vector<Node> nodes;
    std::vector<Edge> edges;

    float repelStrength = 0.1f;
    float springStrength = 0.02f;

    void addNode(const sf::Vector2f& position, const sf::Font& font) {
        nodes.emplace_back(position, nodes.size(), font);
    }

    void addEdge(int firstNode, int secondNode) {
        if (hasEdge(firstNode, secondNode)) return;
        auto dx = nodes[secondNode].position.x - nodes[firstNode].position.x;
        auto dy = nodes[secondNode].position.y - nodes[firstNode].position.y;
        auto dist = std::sqrt(dx*dx + dy*dy);
        edges.emplace_back(firstNode, secondNode, dist);
    }

    bool hasEdge(int firstNode, int secondNode) {
        for (auto& edge : edges) {
            if ((edge.firstNode == firstNode && edge.secondNode == secondNode) || (edge.firstNode == secondNode && edge.secondNode == firstNode)) {
                return true;
            }
        }
        return false;
    }

    void updatePhysics(int draggedNodeId) {
        for (size_t i = 0; i < nodes.size(); i++) {
            for (size_t j = i + 1; j < nodes.size(); j++) {
                sf::Vector2f delta = nodes[j].position - nodes[i].position;
                auto dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);
                auto minDist = nodes[i].radius + nodes[j].radius + 2;

                if (dist < minDist && dist > 0.01f) {
                    sf::Vector2f push = (delta / dist) * (minDist - dist) * repelStrength;
                    if ((int)i != draggedNodeId) nodes[i].position -= push;
                    if ((int)j != draggedNodeId) nodes[j].position += push;
                }
            }
        }

        for (auto& edge : edges) {
            sf::Vector2f delta = nodes[edge.secondNode].position - nodes[edge.firstNode].position;
            float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);
            if (dist > 0.01f) {
                sf::Vector2f dir = delta / dist;
                float force = (dist - edge.weight) * springStrength;
                if (edge.firstNode != draggedNodeId) nodes[edge.firstNode].position += dir * force;
                if (edge.secondNode != draggedNodeId) nodes[edge.secondNode].position -= dir * force;
            }
        }
    }

    void updateNodes() {
        for (auto& node : nodes) node.update();
    }

    void draw(sf::RenderWindow& window, const sf::Font& font, int editingEdge, const std::string& weightInput) {
        for (int i = 0; i < (int)edges.size(); i++) {
            auto& edge = edges[i];
            auto color = edge.isSelected ? sf::Color::Red : sf::Color::White;

            sf::Vertex line[] = {
                sf::Vertex(nodes[edge.firstNode].position, color),
                sf::Vertex(nodes[edge.secondNode].position, color)
            };
            window.draw(line, 2, sf::Lines);

            // подпись веса
            auto mid = (nodes[edge.firstNode].position + nodes[edge.secondNode].position) / 2.f;
            sf::Text weightText;
            weightText.setFont(font);
            if (i == editingEdge && !weightInput.empty()) {
                weightText.setString(weightInput);
                weightText.setFillColor(sf::Color::Yellow);
            } else {
                weightText.setString(std::to_string((int)edges[i].weight));
                weightText.setFillColor(sf::Color::Green);
            }
            weightText.setCharacterSize(18);
            auto bounds = weightText.getLocalBounds();
            weightText.setOrigin(bounds.width/2, bounds.height/2);
            weightText.setPosition(mid);
            window.draw(weightText);
        }
        for (auto& node : nodes) node.draw(window);
    }

    void clear() {
        nodes.clear();
        edges.clear();
    }
};

bool isPointNearLine(sf::Vector2f p, sf::Vector2f firstNode, sf::Vector2f secondNode, float tolerance = 10.f) {
    auto ab = secondNode - firstNode;
    auto ap = p - firstNode;
    auto abLen2 = ab.x*ab.x + ab.y*ab.y;
    if (abLen2 < 1e-6f) return false;
    auto proj = (ap.x*ab.x + ap.y*ab.y) / abLen2;
    proj = std::max(0.f, std::min(1.f, proj));
    auto closest = firstNode + proj * ab;
    auto dx = p.x - closest.x, dy = p.y - closest.y;
    return std::sqrt(dx*dx + dy*dy) < tolerance;
}

int main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(3840, 2160), "Graph", sf::Style::Default, settings);
    window.setFramerateLimit(90);

    sf::Font font;
    if (!font.loadFromFile("/System/Library/Fonts/Supplemental/Arial.ttf")) return -1;

    Graph graph;
    auto draggedNodeId = -1;
    auto selectedNodeId = -1;
    auto selectedEdgeId = -1;
    bool typingWeight = false;
    std::string weightInput;

    sf::RectangleShape clearBtn(sf::Vector2f(100, 40));
    clearBtn.setFillColor(sf::Color(200, 50, 50));
    clearBtn.setPosition(10, 10);

    sf::Text btnText("Clear", font, 18);
    btnText.setFillColor(sf::Color::White);
    btnText.setPosition(25, 18);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left) {

                sf::Vector2f click(event.mouseButton.x, event.mouseButton.y);

                // кнопка "Clear"
                if (clearBtn.getGlobalBounds().contains(click)) {
                    graph.clear();
                    draggedNodeId = -1;
                    selectedNodeId = -1;
                    selectedEdgeId = -1;
                    typingWeight = false;
                    weightInput.clear();
                    continue;
                }

                bool clickedNode = false;

                // клик по вершине?
                for (int i = 0; i < (int)graph.nodes.size(); i++) {
                    auto dx = click.x - graph.nodes[i].position.x;
                    auto dy = click.y - graph.nodes[i].position.y;
                    auto dist = std::sqrt(dx*dx + dy*dy);
                    if (dist <= graph.nodes[i].radius) {
                        typingWeight = false;
                        selectedEdgeId = -1;
                        weightInput.clear();

                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
                            if (selectedNodeId == -1) {
                                selectedNodeId = i;
                                graph.nodes[i].shape.setFillColor(sf::Color::Yellow);
                                for (auto& edge : graph.edges) edge.isSelected = (edge.firstNode == i || edge.secondNode == i);
                            } else {
                                graph.addEdge(selectedNodeId, i);
                                graph.nodes[selectedNodeId].shape.setFillColor(sf::Color(100, 150, 250));
                                selectedNodeId = -1;
                                for (auto& edge : graph.edges) edge.isSelected = false;
                            }
                        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                            float angle = (float)rand() / RAND_MAX * 2 * M_PI;
                            graph.addNode(graph.nodes[i].position + sf::Vector2f(60 * cos(angle), 60 * sin(angle)), font);
                            graph.addEdge(i, (int)graph.nodes.size() - 1);
                        } else {
                            draggedNodeId = i;
                            for (auto& edge : graph.edges) edge.isSelected = (edge.firstNode == i || edge.secondNode == i);
                        }

                        clickedNode = true;
                        break;
                    }
                }

                if (!clickedNode) {
                    bool edgeClicked = false;
                    for (int i = 0; i < (int)graph.edges.size(); i++) {
                        auto& edge = graph.edges[i];
                        if (isPointNearLine(click, graph.nodes[edge.firstNode].position, graph.nodes[edge.secondNode].position)) {
                            for (auto& ee : graph.edges) ee.isSelected = false;
                            selectedNodeId = -1;
                            draggedNodeId = -1;

                            selectedEdgeId = i;
                            edge.isSelected = true;
                            typingWeight = true;
                            weightInput.clear();
                            edgeClicked = true;
                            break;
                        }
                    }
                    if (!edgeClicked) {
                        typingWeight = false;
                        selectedEdgeId = -1;
                        weightInput.clear();
                        graph.addNode(click, font);
                        for (auto& edge : graph.edges) edge.isSelected = false;
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                draggedNodeId = -1;
            }

            if (typingWeight && selectedEdgeId != -1 && event.type == sf::Event::TextEntered) {
                const auto ch = static_cast<char>(event.text.unicode);
                if (ch >= '0' && ch <= '9') {
                    weightInput += ch;
                } else if ((ch == '.' || ch == ',') && weightInput.find('.') == std::string::npos) {
                    weightInput += '.';
                }
            }

            if (typingWeight && selectedEdgeId != -1 && event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Return) {
                    if (!weightInput.empty()) {
                        graph.edges[selectedEdgeId].weight = std::stof(weightInput);
                    }
                    typingWeight = false;
                    graph.edges[selectedEdgeId].isSelected = false; selectedEdgeId = -1;
                } else if (event.key.code == sf::Keyboard::Escape) {
                    typingWeight = false;
                    weightInput.clear();
                    graph.edges[selectedEdgeId].isSelected = false; selectedEdgeId = -1;
                } else if (event.key.code == sf::Keyboard::BackSpace) {
                    if (!weightInput.empty()) weightInput.pop_back();
                }
            }
        }

        if (draggedNodeId != -1) {
            graph.nodes[draggedNodeId].position = (sf::Vector2f)sf::Mouse::getPosition(window);
        }

        graph.updatePhysics(draggedNodeId);
        graph.updateNodes();

        window.clear(sf::Color::Black);
        graph.draw(window, font, typingWeight ? selectedEdgeId : -1, weightInput);
        window.draw(clearBtn);
        window.draw(btnText);
        window.display();
    }
}
