#pragma once

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <set>
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
          heap(PointCompare(this)) {
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

        bool operator()(const Point* l, const Point* r) const {
            return (l->distance + model->heuristic(l)) < (r->distance + model->heuristic(r));
        }
    };

    void regenerateTerrain() {
        perlin.reseed(rand());  // new noise
        fillPerlin();           // calc new terrain
        clearPathState();
        start = nullptr;
        end = nullptr;
    }

    void clearPathState() {
        // cleanup Points
        for (auto& row : terrain) {
            for (auto& p : row) {
                p.distance = INFINITY;
                p.prev = nullptr;
                p.visited = false;
            }
        }

        // clean queue
        heap.clear();
    }

    void setupPathfinding() {
        start->distance = 0;
        heap.insert(start);
    }

    Point* getBest() {
        if (heap.size())
            return *heap.begin();
        else
            return nullptr;
    }

    bool iteratePathfinding() {
        // get top element from queue
        Point* active = *heap.begin();

        // if the active Point is the end, we're done!
        if (active == end)
            return true;

        heap.erase(heap.begin());

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

                // We have to remove the older point first and insert it again
                // in order to reorganize the set with the new point value,
                // since if we would just insert the already existing point,
                // the set would not reorganize itself.
                if (heap.contains(p))
                    heap.erase(p);
                heap.insert(p);
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

    void setStart(Point* p) {
        if (heap.size())  // clear if there is already a path
            clearPathState();
        start = p;
    }

    void setEnd(Point* p) {
        if (heap.size())  // clear if there is already a path
            clearPathState();
        end = p;
    }

    Point* getStart() { return start; }
    Point* getEnd() { return end; }

private:
    float heightCostMult = 200.f;  // multiplier for heigth cost in 3d euclidean distance calculation
    uint octaves = 20;             // how many octaves
    double stepSize = 0.03;        // multiplier for x and y values, to reduce step size
    double persistence = 0.4;      // how much the value of the next octave is reduced
    uint levels = 16;

    const uint width, height;       // size of the terrain
    vector<vector<Point>> terrain;  // terrain itself

    Point* start = nullptr;  // search from here
    Point* end = nullptr;    // find path from start to end

    siv::PerlinNoise perlin;  // current noise generator

    std::set<Point*, PointCompare> heap;

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

        // cluster terrain into levels
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                terrain[y][x].height = (int(terrain[y][x].height * levels)) / float(levels);
            }
        }
    }

    // euclidean distance from p to end
    float heuristic(const Point* p) {
        return distance(p, end);
    }

    // euclidean distance between points where height is the 3rd dimension
    float distance(const Point* p1, const Point* p2) {
        float dx = float(p1->x) - float(p2->x);
        float dy = float(p1->y) - float(p2->y);
        float dz = float(p1->height) - float(p2->height);
        dz *= heightCostMult;
        return sqrtf(dx * dx + dy * dy + dz * dz);
    }

    // get neighbors of point p
    vector<Point*> getNeighbors(const Point* p) {
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
