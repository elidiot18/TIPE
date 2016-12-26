#include "geometry.h"

using namespace std;

double distance(const Point* p1, const Point* p2) {
    const Coordinates& c1 = p1->coordinates;
    const Coordinates& c2 = p2->coordinates;
    return pow(c1.x - c2.x, 2) + pow(c1.y - c2.y, 2) + pow(c1.z - c2.z, 2);
}

double distance(const Point* p, const vector<Point*>& L) {
    double d = numeric_limits<double>::max();
    for (Point* l : L) {
        double dl = distance(p, l);
        if (dl < d) {
            d = dl;
        }
    }
    return d;
}

Point* farthest(const vector<Point*>& W, const vector<Point*>& L) {
    Point* out;
    double dm(0);
    for (Point* p : W) {
        double d = distance(p, L);
        if (d > dm) {
            dm = d;
            out = p;
        }
    }
    return out;
}

Triangle::Triangle(Point* i, Point* j, Point* k) {
    if (*i < *j) {
        if (*i < *k) {
            if (*j < *k) {
                p1 = i; p2 = j; p3 = k;
            }
            else {
                p1 = i; p2 = k; p3 = j;
            }
        }
        else {
            p1 = k; p2 = i; p3 = j;
        }
    }
    else {
        if (*j < *k) {
            if (*i < *k) {
                p1 = j; p2 = i; p3 = k;
            }
            else {
                p1 = j; p2 = k; p3 = i;
            }
        }
        else {
            p1 = k; p2 = j; p3 = i;
        }
    }
}

Edge::Edge(Point* i, Point* j) {
    i < j ? (p1 = i, p2 = j) : (p1 = j, p2 = i);
}
