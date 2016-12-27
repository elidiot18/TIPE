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
    vector<Point*> ptr_vec = {i, j, k};
    sort(ptr_vec.begin(), ptr_vec.end(), pointer_sort);
    this->p1 = ptr_vec[0];
    this->p2 = ptr_vec[1];
    this->p3 = ptr_vec[2];
}

Edge::Edge(Point* i, Point* j) {
    i < j ? (this->p1 = i, this->p2 = j) : (this->p1 = j, this->p2 = i);
}
