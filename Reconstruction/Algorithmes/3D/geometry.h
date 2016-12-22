#ifndef INCLUDE_GEOMETRY
#define INCLUDE_GEOMETRY

#include <vector>
#include <set>

using namespace std;

typedef struct {
    double x;
    double y;
    double z;
} PointDouble;

typedef size_t Point;

typedef struct {
    Point p1;
    Point p2;
} Edge;

typedef struct {
    Point p1;
    Point p2;
    Point p3;
} Triangle;

struct Edge_less {
    operator()(Edge e1, Edge e2) {
        return e1.p1 < e2.p1 || (e1.p1 == e2.p1 && e1.p2 < e2.p2);
    }
};

struct Triangle_less {
    operator()(Triangle t1, Triangle t2) {
        return t1.p1 < t2.p1 || (t1.p1 == t2.p1 && t1.p2 < t2.p2) || (t1.p1 == t2.p1 && t1.p2 == t2.p2 && t1.p3 < t2.p3);
    }
};

typedef struct {
    set<Point> points;
    set<Edge, Edge_less> edges;
    set<Triangle, Triangle_less> triangles;
} SimplicialComplex;

typedef struct {
    Point p1;
    Point p2;
    Point p3;
} Neighbourhood;

struct Neighbourhood_less {
    operator()(Neighbourhood n1, Neighbourhood n2) {
        return n1.p1 < n2.p1 || (n1.p1 == n2.p1 && n1.p2 < n2.p2) || (n1.p1 == n2.p1 && n1.p2 == n2.p2 && n1.p3 < n2.p3);
    }
};

void init_cwl(SimplicialComplex*, size_t);
double distance(Point, Point, vector<PointDouble>&);
double distance(Point, vector<size_t>&, vector<PointDouble>&);
Point farthest(vector<PointDouble>&, vector<Point>&);

#endif // INCLUDE_GEOMETRY
