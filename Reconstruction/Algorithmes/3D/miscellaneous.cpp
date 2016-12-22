#include "miscellaneous.h"

string remove_extension(string &s) {
    size_t lastdot = s.find_last_of('.');
    if (lastdot == string::npos) return s;
    return s.substr(0, lastdot);
}

void order(Point *i, Point *j, Point *k) {
    Point ii, jj, kk;
    ii = min(min(*i, *j), *k);
    kk = max(max(*i, *j), *k);
    jj = *i + *j + *k - ii - kk;
    *i = ii;
    *j = jj;
    *k = kk;
}

void order(Point *i, Point *j) {
    Point ii, jj;
    ii = min(*i, *j);
    jj = max(*i, *j);
    *i = ii;
    *j = jj;
}
