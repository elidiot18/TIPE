#include "geometry.h"

double distance(Point p1, Point p2, vector<PointDouble> &W) {
    return (W[p1].x - W[p2].x) * (W[p1].x - W[p2].x) + (W[p1].y - W[p2].y) * (W[p1].y - W[p2].y) + (W[p1].z - W[p2].z) * (W[p1].z - W[p2].z);
}

double distance(Point p, vector<Point> &L, vector<PointDouble> &W) {
    size_t l = L.size();
    double d = distance(p, L[0], W);
    for (size_t k = 0; k < l; ++k) {
        double dk = distance(p, L[k], W);
        if (dk < d) {
            d = dk;
        }
    }
    return d;
}

size_t farthest(vector<PointDouble> &W, vector<Point> &L) {
    size_t n = W.size();
    size_t m = 0;
    double dm = distance(m, L, W);
    for (size_t j = 1; j < n; ++j) {
        double dj = distance(j, L, W);
        if (dm < dj) {
            dm = dj;
            m = j;
        }
    }
    return m;
}
