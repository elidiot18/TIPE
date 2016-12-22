#ifndef INCLUDE_RECONSTRUCTION
#define INCLUDE_RECONSTRUCTION

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>
#include "geometry.h"

using namespace std;

bool in(Point, Point, Point, SimplicialComplex&);
bool in(Point, Point, SimplicialComplex&);
bool in(Point, SimplicialComplex&);
void del(Point, Point, Point, SimplicialComplex*);
void del(Point, Point, SimplicialComplex*);
void del(Point, SimplicialComplex*);
void add(Point, Point, Point, SimplicialComplex*);
void add(Point, Point, SimplicialComplex*);
void add(Point, SimplicialComplex*);
void del(Point, vector<Point>*);

#endif // INCLUDE_RECONSTRUCTION
