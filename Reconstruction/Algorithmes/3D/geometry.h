#ifndef INCLUDE_GEOMETRY
#define INCLUDE_GEOMETRY

#include <vector>

using namespace std;

typedef struct {
    double x;
    double y;
    double z;
} Point;

double distance(size_t, size_t, vector<Point>&);
double distance(size_t, vector<size_t>&, vector<Point>&);
size_t farthest(vector<Point>&, vector<size_t>&);

#endif // INCLUDE_GEOMETRY
