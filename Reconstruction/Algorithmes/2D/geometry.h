#ifndef INCLUDE_GEOMETRY
#define INCLUDE_GEOMETRY

#include <vector>
#include <map>
#include <set>
#include <limits>
#include <cmath>
#include <memory>

struct Coordinates;
struct Point;
struct Edge;

double _distance(const Point*, const Point*);
double distance(const Point*, const std::vector<Point*>&);
Point* farthest(const std::vector<Point*>&, const std::vector<Point*>&);
Edge get_edge(Point*, Point*);

struct Coordinates {
    double x;
    double y;
};

typedef std::set<Edge> SimplicialComplex;

struct Point {
    Coordinates coordinates;
    size_t index;
    bool operator<(const Point& rhs) const {return this->index < rhs.index;}
    bool operator==(const Point& rhs) const {return this->index == rhs.index;}

    Point(size_t index, double x, double y) {this->index = index; coordinates.x = x; coordinates.y = y;}

    std::multimap<double, Point*> neighbourhood;
    std::set<Edge> edges;
};

struct Edge {
    Edge(Point*, Point*);
    bool operator<(const Edge& e) const {return e.index1 < this->index1 || (e.index1 == this->index1 && e.index2 < this->index2);}
    size_t index1;
    size_t index2;
};

#endif // INCLUDE_GEOMETRY
