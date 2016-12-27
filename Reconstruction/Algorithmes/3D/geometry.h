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

double distance(const Coordinates&, const Coordinates&);
double distance(const Point*, const Point&);
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

    Point(size_t index, double x, double y, double z) {this->index = index; coordinates.x = x; coordinates.y = y; coordinates.z =z;}

    std::multimap<double, Point*> neighbourhood;

    SimplicialComplex simplices;
};

bool pointer_sort(Point* lhs, Point* rhs) {
    return lhs->index < rhs->index;
};

struct Edge {
    Edge(Point*, Point*);
    bool operator<(const Edge& e) const {return *e.p1 < *this->p1 || (*e.p1 == *this->p1 && *e.p2 < *this->p2);}
    Point* p1;
    Point* p2;
};

struct Triangle {
    Triangle(Point*, Point*, Point*);
    bool operator<(const Triangle& t) const {return *t.p1 < *this->p1 || (*t.p1 == *this->p1 && *t.p2 < *this->p2) || (*t.p1 == *this->p1 && *t.p2 == *this->p2 && *t.p3 < *this->p3);}
    Point* p1;
    Point* p2;
    Point* p3;
};

#endif // INCLUDE_GEOMETRY
