#pragma once

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <queue>
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
          perlin(rand()),
          queue(PointCompare(this)) {
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
        Point* prev = nullptr;      // previous point for shortest path
        uint x = 0, y = 0;
    };

    struct PointCompare {
        PointCompare(Model* model) : model(model) {}

        Model* model;

        bool operator()(Point* l, Point* r) {
            return (l->distance + model->heuristic(l)) > (r->distance + model->heuristic(r));
        }
    };

    void regenerateTerrain() {
        perlin.reseed(rand());  // new noise
        fillPerlin();                       // calc new terrain
        clearPathState();
    }

    void clearPathState() {
        // remove start and end
        start = nullptr;
        end = nullptr;

        // cleanup Points
        for (auto& row : terrain) {
            for (auto& p : row) {
                p.distance = INFINITY;
                p.prev = nullptr;
                p.visited = false;
            }
        }

        // clean queue
        queue = std::priority_queue<Point*, vector<Point*>, PointCompare>(PointCompare(this));
    }

    void setupPathfinding() {
        start->distance = 0;
        queue.push(start);
    }

    Point* getBest() {
        if (queue.size())
            return queue.top();
        else
            return nullptr;
    }

    bool iteratePathfinding() {
        // TODO: füge ich Points doppelt hinzu????

        // get top element from queue
        Point* active = queue.top();

        // if the active Point is the end, we're done!
        if (active == end)
            return true;

        queue.pop();

        // get neighbors
        auto neighbors = getNeighbors(active);

        for (auto& p : neighbors) {
            if (p->visited) continue;  // skip visited Points

            // new distanc to p
            float dist = active->distance + distance(active, p);

            // if distance is lower
            if (dist < p->distance) {
                p->distance = dist;
                p->prev = active;

                queue.push(p);  // TODO: Here!
            }
        }

        // Point is done, don't visit it anymore
        active->visited = true;

        return false;
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
    float heightCostMult = 500.f;  // multiplier for heigth cost in 3d euclidean distance calculation
    uint octaves = 10;              // how many octaves
    double stepSize = 0.03;         // multiplier for x and y values, to reduce step size
    double persistence = 0.5;       // how much the value of the next octave is reduced

    const uint width, height;       // size of the terrain
    vector<vector<Point>> terrain;  // terrain itself

    Point* start = nullptr;  // search from here
    Point* end = nullptr;    // find path from start to end

    siv::PerlinNoise perlin;  // current noise generator

    std::priority_queue<Point*, vector<Point*>, PointCompare> queue;  // keep track of best Point for shortest path

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
    float heuristic(Point* p) {
        float h = heightCostMult;

        heightCostMult = 1.f;
        float dist = distance(p, end);  // TODO: consider a better heuristic.. ?
        heightCostMult = h;
        return dist;
    }

    // euclidean distance between points where height is the 3rd dimension
    float distance(Point* p1, Point* p2) {
        float dx = float(p1->x) - float(p2->x);
        float dy = float(p1->y) - float(p2->y);
        float dz = float(p1->height) - float(p2->height);
        dz *= heightCostMult;
        return sqrtf(dx * dx + dy * dy + dz * dz);
    }

    // get neighbors of point p
    vector<Point*> getNeighbors(Point* p) {
        vector<Point*> res;

        uint x = p->x;
        uint y = p->y;

        if (y > 0) {
            res.push_back(&terrain[y - 1][x]);

            if (y < height - 1) {
                res.push_back(&terrain[y + 1][x]);

                if (x > 0) {
                    res.push_back(&terrain[y - 1][x - 1]);
                    res.push_back(&terrain[y][x - 1]);
                    res.push_back(&terrain[y + 1][x - 1]);
                }

                if (x < width - 1) {
                    res.push_back(&terrain[y - 1][x + 1]);
                    res.push_back(&terrain[y][x + 1]);
                    res.push_back(&terrain[y + 1][x + 1]);
                }

            } else {
                if (x > 0) {
                    res.push_back(&terrain[y - 1][x - 1]);
                    res.push_back(&terrain[y][x - 1]);
                }

                if (x < width - 1) {
                    res.push_back(&terrain[y - 1][x + 1]);
                    res.push_back(&terrain[y][x + 1]);
                }
            }
        } else {
            res.push_back(&terrain[y + 1][x]);

            if (x > 0) {
                res.push_back(&terrain[y][x - 1]);
                res.push_back(&terrain[y + 1][x - 1]);
            }

            if (x < width - 1) {
                res.push_back(&terrain[y][x + 1]);
                res.push_back(&terrain[y + 1][x + 1]);
            }
        }

        return res;
    }
};
