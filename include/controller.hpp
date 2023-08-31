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
        uint x = event.mouseButton.x / window.getPixelSize();
        uint y = event.mouseButton.y / window.getPixelSize();

        // if theres already a path, clear first
        if (model.getBest())
            model.clearPathState();

        if (event.mouseButton.button == sf::Mouse::Left) {  // set start
            model.setStart(model.getPoint(x, y));
        } else if (event.mouseButton.button == sf::Mouse::Right) {  // set end
            model.setEnd(model.getPoint(x, y));
        }
    }

    void handleKeyPressEvent(sf::Event& event) {
        switch (event.key.code) {
            case sf::Keyboard::P:
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

private:
    Window& window;
    Model& model;
};
