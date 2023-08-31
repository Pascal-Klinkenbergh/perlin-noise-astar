#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <utility>

#include "model.hpp"
#include "window.hpp"

class Controller {
public:
    Controller(Window& window, Model& model) : window(window), model(model) {}

    enum State {
        drawStart,
        drawEnd,
        pathfinding,
    };

    void handleEvent(sf::Event& event) {
        // distribute events to specialized handler functions
        switch (event.type) {
            case sf::Event::Closed:  // window close button
                window.close();
                break;

            case sf::Event::KeyPressed:  // key pressed
                handleKeyPressEvent(event);
                break;

            case sf::Event::MouseButtonPressed:  // mouse button pressed
                handleMousePressEvent(event);
                break;

            default:
                break;
        }
    }

    void handleMousePressEvent(sf::Event& event) {
        if (event.mouseButton.button == sf::Mouse::Left) {  // left click

            uint x = event.mouseButton.x / window.getPixelSize();
            uint y = event.mouseButton.y / window.getPixelSize();

            if (currentState == State::drawStart) {
                // set start point
                model.setStart(model.getPoint(x, y));

            } else if (currentState == State::drawEnd) {
                // set end point
                model.setEnd(model.getPoint(x, y));
            }
        }
    }

    void displayMode(std::string mode) {
        std::cout << "Mode: " << mode << std::endl;
    }

    void handleKeyPressEvent(sf::Event& event) {
        switch (event.key.code) {
            case sf::Keyboard::S:
                currentState = State::drawStart;
                displayMode("start drawing");
                break;

            case sf::Keyboard::E:
                currentState = State::drawEnd;
                displayMode("end drawing");
                break;

            case sf::Keyboard::P:
                if (model.getEnd() && model.getStart()) {
                    currentState = State::pathfinding;
                    displayMode("pathfinding");
                } else {
                    std::cout << "Please select a start and end point first" << std::endl;
                }
                break;

            case sf::Keyboard::R:
                model.regenerateTerrain();
                displayMode("start drawing");
                currentState = State::drawStart;
                break;

            case sf::Keyboard::Q:
                window.close();
                break;

            default:
                break;
        }
    }

private:
    Window& window;
    Model& model;
    State currentState = State::drawStart;
};
