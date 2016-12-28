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

Triangle::Triangle(const Point* i, const Point* j, const Point* k) {
    vector<size_t> vec = {i->index, j->index, k->index};
    sort(vec.begin(), vec.end());
    this->index1 = vec[0];
    this->index2 = vec[1];
    this->index3 = vec[2];
}

Edge::Edge(const Point* i, const Point* j) {
    size_t iindex = i->index, jindex = j->index;
    if (iindex < jindex) {
        this->index1 = iindex;
        this->index2 = jindex;
    }
    else {
        this->index1 = jindex;
        this->index2 = iindex;
    }
}
