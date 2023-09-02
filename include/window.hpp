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
                    uint8_t colVal = (uchar)(point->height * 255);
                    sf::Color col(colVal, colVal, colVal);

                    // draw visited points in a greener shade
                    if (point->visited)
                        col.r *= 0.7f;

                    img.setPixel(x, y, col);
                }
            }
        }

        // draw best path
        Model::Point* p = model.getBest();
        while (p) {
            if (p != model.getEnd() && p != model.getStart()) {
                float factor = p->height;
                img.setPixel(p->x, p->y, sf::Color(uint8_t(255 * factor), 90, uint8_t(255 * (1.f - factor))));
            }
            p = p->prev;
        }

        // update texture
        texture.update(img);

        // draw sprite to screen buffer
        win.draw(sprite);

        // draw info
        if (drawInfo)
            displayInfo();

        // final drawing to screen
        win.display();
    }

    void displayInfo() {
        auto pt1 = win.mapPixelToCoords({0, 0});
        auto pt2 = win.mapPixelToCoords({int(model.getWidth()) * pixelSize, int(model.getHeight()) * pixelSize});

        int xLeft = pt1.x / pixelSize;
        int xRight = pt2.x / pixelSize;

        int yUpper = pt1.y / pixelSize;
        int yLower = pt2.y / pixelSize;

        if (xLeft < 0) xLeft = 0;
        if (yUpper < 0) yUpper = 0;

        if (xRight > model.getWidth() - 1) xRight = model.getWidth() - 1;
        if (yLower > model.getHeight() - 1) yLower = model.getHeight() - 1;

        // draw horizontal lines
        for (int i = xLeft; i <= xRight; ++i) {
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(i * pixelSize, 0), sf::Color::Black),
                sf::Vertex(sf::Vector2f(i * pixelSize, model.getHeight() * pixelSize), sf::Color::Black)};
            win.draw(line, 2, sf::Lines);
        }

        // draw vertial lines
        for (int i = yUpper; i <= yLower; ++i) {
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(0, i * pixelSize), sf::Color::Black),
                sf::Vertex(sf::Vector2f(model.getWidth() * pixelSize, i * pixelSize), sf::Color::Black)};
            win.draw(line, 2, sf::Lines);
        }
    }

    void moveView(sf::Vector2f delta) {
        view.move(delta);
        win.setView(view);
    }

    sf::Vector2f mapPixelToCoords(const sf::Vector2i& point) {
        return win.mapPixelToCoords(point);
    }

    void adjustZoom(float delta) {
        zoom *= powf(1.1, delta);

        drawInfo = zoom < 0.1;

        if (zoom < 0.01) zoom = 0.01;
        if (zoom > 7.f) zoom = 7.f;

        view.setSize(win.getDefaultView().getSize());
        view.zoom(zoom);
        win.setView(view);
    }

    int getPixelSize() {
        return pixelSize;
    }

private:
    const int pixelSize;

    float zoom = 1.f;

    Model& model;
    sf::RenderWindow win;
    sf::View view;

    sf::Image img;
    sf::Texture texture;
    sf::Sprite sprite;

    bool drawInfo = false;
};
