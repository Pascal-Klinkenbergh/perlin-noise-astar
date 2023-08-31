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
          win(sf::VideoMode(model.getWidth() * pixelSize, model.getHeight() * pixelSize), "Path", sf::Style::Titlebar | sf::Style::Close) {
        // set fps
        win.setFramerateLimit(60);

        // drawing setup
        img.create(model.getWidth(), model.getHeight());
        texture.create(model.getWidth(), model.getHeight());
        sprite.setTexture(texture);
        sprite.setScale(pixelSize, pixelSize);
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
        win.clear(sf::Color::White);

        // update pixels on img with model
        for (int y = 0; y < model.getHeight(); ++y) {
            for (int x = 0; x < model.getWidth(); ++x) {
                // get point at (x, y)
                Model::Point* point = model.getPoint(x, y);

                // set different color for start and end
                if (point == model.getStart())
                    img.setPixel(x, y, sf::Color::Green);
                else if (point == model.getEnd())
                    img.setPixel(x, y, sf::Color::Red);
                else {
                    uchar colVal = (uchar)(point->height * 255);
                    sf::Color col(colVal, colVal, colVal);

                    // draw visited points in a greener shade
                    if (point->visited) {
                        col.r *= 0.8;
                        col.b *= 0.9;
                    }

                    img.setPixel(x, y, col);
                }
            }
        }

        Model::Point* p = model.getBest();
        while (p != nullptr) {
            if (p != model.getEnd() || p != model.getStart())
                img.setPixel(p->x, p->y, sf::Color::Blue);
            p = p->prev;
        }

        // update texture
        texture.update(img);

        // draw sprite that has that texture
        win.draw(sprite);

        // final drawing to screen
        win.display();
    }

    uint getPixelSize() {
        return pixelSize;
    }

private:
    Model& model;
    uint pixelSize;
    sf::RenderWindow win;

    sf::Image img;
    sf::Texture texture;
    sf::Sprite sprite;
};
