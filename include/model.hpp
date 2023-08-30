#pragma once

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "perlin_noise.hpp"

using std::vector;
typedef unsigned int uint;
typedef unsigned char uchar;

class Model {
public:
    Model(uint width, uint height)
        : width(width),
          height(height),
          terrain(height, vector<Point>(width, Point{})),
          perlin(rand()) {
        // set coordinated of points
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                terrain[y][x].x = x;
                terrain[y][x].y = y;
            }
        }

        // initial terrain creation
        fillPerlin();
    }

    struct Point {
        float height = 0.f;         // between [0, 1]
        float distance = INFINITY;  // distance to start
        bool visited = false;       // ignore when visited before
        uint x = 0, y = 0;
    };

    void randomizePerlin() {
        perlin = siv::PerlinNoise(rand());  // new noise
        fillPerlin();                       // calc new terrain
    }

    uint getWidth() { return width; }
    uint getHeight() { return height; }

    Point* getPoint(uint x, uint y) {
        return &(terrain[y][x]);
    }

    void setStart(Point* p) { start = p; }
    void setEnd(Point* p) { end = p; }

    Point* getStart() { return start; }
    Point* getEnd() { return end; }

private:
    float heightCostMult = 2.f;  // multiplier for heigth cost in 3d euclidean distance calculation
    uint octaves = 10;           // how many octaves
    double stepSize = 0.01;      // multiplier for x and y values, to reduce step size
    double persistence = 0.6;    // how much the value of the next octave is reduced

    const uint width, height;       // size of the terrain
    vector<vector<Point>> terrain;  // terrain itself

    Point* start = nullptr;  // search from here
    Point* end = nullptr;    // find path from start to end

    siv::PerlinNoise perlin;  // current noise generator

    void fillPerlin() {
        double lower = 2.0;   // 1 is max of [0, 1], so 2 is bigger
        double upper = -1.0;  // -1 because 0 is minimal val

        // set noise of terrain
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                double noise = perlin.octave2D_01(x * stepSize, y * stepSize, octaves, persistence);
                if (noise < lower) lower = noise;
                if (noise > upper) upper = noise;
                terrain[y][x].height = noise;
            }
        }

        // rescale terrain so that its between [0, 1]
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                terrain[y][x].height = (terrain[y][x].height - lower) / (upper - lower);
            }
        }
    }

    // euclidean distance from p to end
    float heuristic(Point& p) {
        float dx = float(p.x) - float(end->x);
        float dy = float(p.y) - float(end->y);
        return sqrtf(dx * dx + dy * dy);
    }

    // euclidean distance between points where height is the 3rd dimension
    float distance(Point& p1, Point& p2) {
        float dx = float(p1.x) - float(p2.x);
        float dy = float(p1.y) - float(p2.y);
        float dz = float(p1.height) - float(p2.height);
        dz *= heightCostMult;
        return sqrtf(dx * dx + dy * dy + dz * dz);
    }
};
