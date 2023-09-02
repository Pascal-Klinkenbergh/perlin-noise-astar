#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <utility>

#include "model.hpp"
#include "window.hpp"

class Controller {
public:
    Controller(Window& window, Model& model) : window(window), model(model) {}

    void handleEvent(sf::Event& event) {
        switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;

            case sf::Event::KeyPressed:
                handleKeyPressEvent(event);
                break;

            case sf::Event::MouseButtonPressed:
                handleMousePressEvent(event);
                break;

            case sf::Event::MouseButtonReleased:
                handleMouseReleaseEvent(event);
                break;

            case sf::Event::MouseWheelScrolled:
                handleMouseScrollEvent(event);
                break;

            case sf::Event::EventType::MouseMoved:
                handleMouseMoveEvent(event);
                break;

            default:
                break;
        }
    }

    void handleKeyPressEvent(sf::Event& event) {
        switch (event.key.code) {
            case sf::Keyboard::Space:
                if (model.getEnd() && model.getStart()) {
                    model.setupPathfinding();
                    while (!model.iteratePathfinding())
                        ;
                } else
                    std::cout << "Please select a start and end point first" << std::endl;
                break;

            case sf::Keyboard::R:
                model.regenerateTerrain();
                break;

            case sf::Keyboard::Q:
                window.close();
                break;

            default:
                break;
        }
    }

    void handleMousePressEvent(sf::Event& event) {
        if (event.mouseButton.button == sf::Mouse::Left) {  // left button pressed
            leftIsPressed = true;
            oldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
        }

        if (event.mouseButton.button == sf::Mouse::Right) {
            rightIsPressed = true;
        }
    }

    void handleMouseReleaseEvent(sf::Event& event) {
        if (event.mouseButton.button == sf::Mouse::Left) {  // left button released
            if (!leftWasMoved) mouseLeftClick(event);
            leftIsPressed = false;
            leftWasMoved = false;
        } else if (event.mouseButton.button == sf::Mouse::Right) {  // right button released
            if (!rightWasMoved) mouseRightClick(event);
            rightIsPressed = false;
            rightWasMoved = false;
        }
    }

    void mouseLeftClick(sf::Event& event) {
        auto pos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
        int x = int(pos.x) / window.getPixelSize();
        int y = int(pos.y) / window.getPixelSize();

        if (x < 0 || x > model.getWidth() - 1 || y < 0 || y > model.getHeight() - 1)
            return;

        model.setStart(model.getPoint(x, y));
    }

    void mouseRightClick(sf::Event& event) {
        auto pos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
        int x = int(pos.x) / window.getPixelSize();
        int y = int(pos.y) / window.getPixelSize();

        if (x < 0 || x > model.getWidth() - 1 || y < 0 || y > model.getHeight() - 1)
            return;

        model.setEnd(model.getPoint(x, y));
    }

    void handleMouseMoveEvent(sf::Event& event) {
        if (leftIsPressed) {
            leftWasMoved = true;

            // get new position
            const sf::Vector2f newPos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));

            // get the difference of the view positions
            const sf::Vector2f deltaPos = oldPos - newPos;

            // move view to adjust for new position
            window.moveView(deltaPos);

            // update oldPos with current position
            oldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
        }

        if (rightIsPressed)
            rightWasMoved = true;
    }

    void handleMouseScrollEvent(sf::Event& event) {
        window.adjustZoom(event.mouseWheelScroll.delta);
    }

private:
    Window& window;
    Model& model;

    bool leftIsPressed = false;
    bool leftWasMoved = false;

    bool rightIsPressed = false;
    bool rightWasMoved = false;

    sf::Vector2f oldPos;
};
