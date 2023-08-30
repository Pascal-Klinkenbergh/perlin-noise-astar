#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "model.hpp"

typedef unsigned int uint;
using std::vector;

class Window {
public:
    Window(Model& model, uint pixelSize)
        : model(model),
          pixelSize(pixelSize),
          win(sf::VideoMode(model.getWidth() * pixelSize, model.getHeight() * pixelSize), "Path", sf::Style::Titlebar | sf::Style::Close),
          pixels(model.getHeight(), vector<sf::RectangleShape>(model.getWidth(), sf::RectangleShape(sf::Vector2f(pixelSize, pixelSize)))) {
        // set position of pixels
        for (int y = 0; y < model.getHeight(); ++y)
            for (int x = 0; x < model.getWidth(); ++x)
                pixels[y][x].setPosition(sf::Vector2f(x * pixelSize, y * pixelSize));

        // set fps
        win.setFramerateLimit(60);
    }

    bool isOpen() {
        return win.isOpen();
    }

    bool pollEvent(sf::Event& event) {
        return win.pollEvent(event);
    }

    void close() {
        win.close();
    }

    void render() {
        // set new color of pixels and draw
        for (int y = 0; y < model.getHeight(); ++y) {
            for (int x = 0; x < model.getWidth(); ++x) {
                Model::Point* point = model.getPoint(x, y);
                uchar height = (uchar)(point->height * 255);
                pixels[y][x].setFillColor(sf::Color(height, height, height));
                win.draw(pixels[y][x]);
            }
        }

        win.display();
    }

    uint getPixelSize() {
        return pixelSize;
    }

private:
    Model& model;
    uint pixelSize;
    sf::RenderWindow win;
    vector<vector<sf::RectangleShape>> pixels;
};
