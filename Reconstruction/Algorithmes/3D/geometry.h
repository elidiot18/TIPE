#ifndef INCLUDE_GEOMETRY
#define INCLUDE_GEOMETRY

#include <vector>

using namespace std;

typedef struct {
    double x;
    double y;
    double z;
} PointDouble;

typedef struct {
    size_t p1;
    size_t p2;
    size_t p3;
} Neighbourhood;

typedef struct {
    vector<size_t> points;
    vector< vector<size_t> > edges;
    vector <vector< vector<size_t> > > triangles;
} SimplicialComplex;

void init_cwl(SimplicialComplex*, size_t);
double distance(size_t, size_t, vector<PointDouble>&);
double distance(size_t, vector<size_t>&, vector<PointDouble>&);
size_t farthest(vector<PointDouble>&, vector<size_t>&);

#endif // INCLUDE_GEOMETRY
