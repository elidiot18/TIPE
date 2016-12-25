#ifndef INCLUDE_GEOMETRY
#define INCLUDE_GEOMETRY

#include <vector>
#include <set>
#include <limits>
#include <cmath>

struct Coordinates;
struct Point;
struct Edge;
struct Triangle;

double distance(const Coordinates&, const Coordinates&);
double distance(const Point&, const Point&);
double distance(const Point&, const std::vector<Point*>&);
Point farthest(const std::vector<Point>&, const std::vector<Point*>&);
Edge get_edge(const Point&, const Point&);
Triangle get_triangle(const Point&, const Point&, const Point&);

struct Coordinates {
    double x;
    double y;
    double z;
};

struct SimplicialComplex;

struct Point {
    static Coordinates coordinates;
    size_t index;

    struct Compare {
        bool operator()(Point* lhs, Point* rhs) {
            return distance(lhs->coordinates, Point::coordinates) < distance(rhs->coordinates, Point::coordinates);
        }
    };

    bool operator<(const Point& rhs) const {return this->index < rhs.index;}
    bool operator==(const Point& rhs) const {return this->index == rhs.index;}

    bool operator<(Point* rhs) const {return this->index < rhs->index;}
    bool operator==(Point* rhs) const {return this->index == rhs->index;}

    std::multiset<Point*, Compare> neighbourhood;

    SimplicialComplex* simplices;
};

struct Edge {
    Edge(Point const *, Point const *);
    Point const * p1;
    Point const * p2;
};

struct Triangle {
    Triangle(Point const *, Point const *, Point const *);
    Point const * p1;
    Point const * p2;
    Point const * p3;
};

struct Edge_less {
    bool operator()(const Edge& e1, const Edge& e2) {
        return e1.p1 < e2.p1 || (e1.p1 == e2.p1 && e1.p2 < e2.p2);
    }
};

struct Triangle_less {
    bool operator()(Triangle t1, Triangle t2) {
        return t1.p1 < t2.p1 || (t1.p1 == t2.p1 && t1.p2 < t2.p2) || (t1.p1 == t2.p1 && t1.p2 == t2.p2 && t1.p3 < t2.p3);
    }
};

struct SimplicialComplex {
    std::set<Edge, Edge_less> edges;
    std::set<Triangle, Triangle_less> triangles;
};

#endif // INCLUDE_GEOMETRY
