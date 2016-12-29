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
struct Triangle;

double _distance(const Point*, const Point*);
double distance(const Point*, const std::vector<Point*>&);
Point* farthest(const std::vector<Point*>&, const std::vector<Point*>&);
Edge get_edge(Point*, Point*);
Triangle get_triangle(Point*, Point*, Point*);

struct Coordinates {
    double x;
    double y;
    double z;
};

struct SimplicialComplex {
    std::set<Edge> edges;
    std::set<Triangle> triangles;
};

struct Point {
    Coordinates coordinates;
    size_t index;
    bool operator<(const Point& rhs) const {return this->index < rhs.index;}
    bool operator==(const Point& rhs) const {return this->index == rhs.index;}

    Point(size_t index, double x, double y, double z) {this->index = index; coordinates.x = x; coordinates.y = y; coordinates.z = z;}

    std::multimap<double, Point*> neighbourhood;

    SimplicialComplex simplices;
};

struct Edge {
    Edge(Point*, Point*);
    bool operator<(const Edge& e) const {return e.index1 < this->index1 || (e.index1 == this->index1 && e.index2 < this->index2);}
    size_t index1;
    size_t index2;
};

struct Triangle {
    Triangle(Point*, Point*, Point*);
    bool operator<(const Triangle& t) const {return t.index1 < this->index1 || (t.index1 == this->index1 && t.index2 < this->index2) || (t.index1 == this->index1 && t.index2 == this->index2 && t.index3 < this->index3);}
    size_t index1;
    size_t index2;
    size_t index3;
};

#endif // INCLUDE_GEOMETRY
