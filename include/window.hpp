#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "model.hpp"

using std::vector;

class Window {
public:
    Window(Model& model, int pixelSize)
        : pixelSize(pixelSize),
          model(model),
          win(sf::VideoMode(model.getWidth() * pixelSize, model.getHeight() * pixelSize), "Path", sf::Style::Titlebar | sf::Style::Close),
          view(win.getDefaultView()) {
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
        win.clear(sf::Color::Black);

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
                        col.r *= 0.7;
                    }

                    img.setPixel(x, y, col);
                }
            }
        }

        // draw best path
        Model::Point* p = model.getBest();
        while (p) {
            if (p != model.getEnd() && p != model.getStart())
                img.setPixel(p->x, p->y, sf::Color::Magenta);
            p = p->prev;
        }

        // update texture
        texture.update(img);

        // draw sprite to screen buffer
        win.draw(sprite);

        // final drawing to screen
        win.display();
    }

    void moveView(sf::Vector2f delta) {
        view.move(delta);
        win.setView(view);
    }

    sf::Vector2f mapPixelToCoords(const sf::Vector2i& point) {
        return win.mapPixelToCoords(point);
    }

    void adjustZoom(float delta) {
        view.zoom(powf(1.1, -delta));
        win.setView(view);
    }

    int getPixelSize() {
        return pixelSize;
    }

private:
    const int pixelSize;

    Model& model;
    sf::RenderWindow win;
    sf::View view;

    sf::Image img;
    sf::Texture texture;
    sf::Sprite sprite;
};
