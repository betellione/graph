#include "Graph.hpp"
#include "utils.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>

int main()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Graph Editor", sf::Style::Default,
                            settings);
    window.setFramerateLimit(90);

    sf::Font font;
    if (!font.loadFromFile("/System/Library/Fonts/Supplemental/Arial.ttf")) return -1;

    Graph graph;
    int draggedNodeId = -1;
    int selectedNodeId = -1;
    int selectedEdgeId = -1;
    bool typingWeight = false;
    std::string weightInput;

    sf::RectangleShape clearBtn({100, 40});
    clearBtn.setFillColor(sf::Color(200, 50, 50));
    clearBtn.setPosition(10, 10);

    sf::Text btnText("Clear", font, 18);
    btnText.setFillColor(sf::Color::White);
    btnText.setPosition(25, 18);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2f click(event.mouseButton.x, event.mouseButton.y);

                // кнопка Clear
                if (clearBtn.getGlobalBounds().contains(click))
                {
                    graph.clear();
                    draggedNodeId = -1;
                    selectedNodeId = -1;
                    selectedEdgeId = -1;
                    typingWeight = false;
                    weightInput.clear();
                    continue;
                }

                bool clickedNode = false;

                // проверяем клик по вершине
                for (int i = 0; i < (int) graph.nodes.size(); i++)
                {
                    float dx = click.x - graph.nodes[i].position.x;
                    float dy = click.y - graph.nodes[i].position.y;
                    float dist = std::sqrt(dx * dx + dy * dy);
                    if (dist <= graph.nodes[i].radius)
                    {
                        typingWeight = false;
                        selectedEdgeId = -1;
                        weightInput.clear();

                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                        {
                            // соединение вершин
                            if (selectedNodeId == -1)
                            {
                                selectedNodeId = i;
                                graph.nodes[i].shape.setFillColor(sf::Color::Yellow);
                                for (auto& edge : graph.edges)
                                    edge.IsSelected =
                                        (edge.firstNodeId == i || edge.secondNodeId == i);
                            }
                            else
                            {
                                graph.addEdge(selectedNodeId, i);
                                graph.nodes[selectedNodeId].shape.setFillColor(
                                    sf::Color(100, 150, 250));
                                selectedNodeId = -1;
                                for (auto& edge : graph.edges) edge.IsSelected = false;
                            }
                        }
                        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                        {
                            // создаём соседнюю вершину
                            float angle = (float) rand() / RAND_MAX * 2 * M_PI;
                            graph.addNode(graph.nodes[i].position +
                                              sf::Vector2f(60 * cos(angle), 60 * sin(angle)),
                                          font);
                            graph.addEdge(i, (int) graph.nodes.size() - 1);
                        }
                        else
                        {
                            // перетаскивание вершины
                            draggedNodeId = i;
                            for (auto& edge : graph.edges)
                                edge.IsSelected = (edge.firstNodeId == i || edge.secondNodeId == i);
                        }

                        clickedNode = true;
                        break;
                    }
                }

                if (!clickedNode)
                {
                    // клик по ребру?
                    bool edgeClicked = false;
                    for (int i = 0; i < (int) graph.edges.size(); i++)
                    {
                        auto& edge = graph.edges[i];
                        if (isPointNearLine(click, graph.nodes[edge.firstNodeId].position,
                                            graph.nodes[edge.secondNodeId].position))
                        {
                            for (auto& ee : graph.edges) ee.IsSelected = false;
                            selectedNodeId = -1;
                            draggedNodeId = -1;

                            selectedEdgeId = i;
                            edge.IsSelected = true;
                            typingWeight = true;
                            weightInput.clear();
                            edgeClicked = true;
                            break;
                        }
                    }

                    if (!edgeClicked)
                    {
                        typingWeight = false;
                        selectedEdgeId = -1;
                        weightInput.clear();
                        graph.addNode(click, font);
                        for (auto& edge : graph.edges) edge.IsSelected = false;
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonReleased &&
                event.mouseButton.button == sf::Mouse::Left)
            {
                draggedNodeId = -1;
            }

            if (typingWeight && selectedEdgeId != -1 && event.type == sf::Event::TextEntered)
            {
                char ch = static_cast<char>(event.text.unicode);
                if (ch >= '0' && ch <= '9')
                {
                    weightInput += ch;
                }
                else if ((ch == '.' || ch == ',') && weightInput.find('.') == std::string::npos)
                {
                    weightInput += '.';
                }
            }

            if (typingWeight && selectedEdgeId != -1 && event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Return)
                {
                    if (!weightInput.empty())
                    {
                        graph.edges[selectedEdgeId].weight = std::stof(weightInput);
                    }
                    typingWeight = false;
                    graph.edges[selectedEdgeId].IsSelected = false;
                    selectedEdgeId = -1;
                }
                else if (event.key.code == sf::Keyboard::Escape)
                {
                    typingWeight = false;
                    weightInput.clear();
                    graph.edges[selectedEdgeId].IsSelected = false;
                    selectedEdgeId = -1;
                }
                else if (event.key.code == sf::Keyboard::BackSpace)
                {
                    if (!weightInput.empty()) weightInput.pop_back();
                }
            }
        }

        if (draggedNodeId != -1)
        {
            graph.nodes[draggedNodeId].position = (sf::Vector2f) sf::Mouse::getPosition(window);
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
