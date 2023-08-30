#include <SFML/Graphics.hpp>
#include <iostream>
#include <utility>

#include "model.hpp"
#include "window.hpp"

class Controller {
public:
    Controller(Window& window, Model& model) : window(window), model(model) {}

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
            auto coord = screenToModelCoordinates(event.mouseButton.x, event.mouseButton.y);
            uint modelX = coord.first;
            uint modelY = coord.second;

            if (currentState == State::drawStart) {
                // set start point
                model.setStart(model.getPoint(modelX, modelY));

            } else if (currentState == State::drawEnd) {
                // set end point
                model.setEnd(model.getPoint(modelX, modelY));
            }
        }
    }

    void handleKeyPressEvent(sf::Event& event) {
        switch (event.key.code) {
            case sf::Keyboard::S:
                currentState = State::drawStart;
                std::cout << "mode: start drawing" << std::endl;
                break;

            case sf::Keyboard::E:
                currentState = State::drawEnd;
                std::cout << "mode: end drawing" << std::endl;
                break;

            case sf::Keyboard::R:
                model.randomizePerlin();
                model.setStart(nullptr);
                model.setEnd(nullptr);
                break;

            case sf::Keyboard::Q:
                window.close();
                break;

            default:
                break;
        }
    }

    // translate the screen coordinate into model coordinates
    std::pair<uint, uint> screenToModelCoordinates(int x, int y) {
        uint pSize = window.getPixelSize();
        return {(uint)(x / pSize), (uint)(y / pSize)};
    }

    enum State {
        drawStart,
        drawEnd,
    };

private:
    Window& window;
    Model& model;
    State currentState = State::drawStart;
};
