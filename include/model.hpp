#pragma once

#include <cmath>
#include <cstdlib>
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
          perlin(rand()) {}

    struct Point {
        float height = 0.f;         // between [0, 1]
        float distance = INFINITY;  // distance to start
        bool visited = false;       // ignore when visited before
    };

    void randomizePerlin() {
        perlin = siv::PerlinNoise(rand());
    }

    void fillPerlin(int32_t octaves = 10, double stepSize = 0.05) {
        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x)
                terrain[y][x].height = perlin.octave2D_01(x * stepSize, y * stepSize, octaves);
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
    const uint width, height;
    vector<vector<Point>> terrain;

    Point* start = nullptr;
    Point* end = nullptr;

    siv::PerlinNoise perlin;
};
