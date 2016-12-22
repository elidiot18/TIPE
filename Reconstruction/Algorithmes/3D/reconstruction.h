#ifndef INCLUDE_RECONSTRUCTION
#define INCLUDE_RECONSTRUCTION

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "geometry.h"

using namespace std;

bool in(size_t, size_t, size_t, SimplicialComplex&);
bool in(size_t, size_t, SimplicialComplex&);
bool in(size_t, SimplicialComplex&);
void del(size_t, size_t, size_t, SimplicialComplex*);
void del(size_t, size_t, SimplicialComplex*);
void del(size_t, SimplicialComplex*);
void add(size_t, size_t, size_t, SimplicialComplex*);
void add(size_t, size_t, SimplicialComplex*);
void add(size_t, SimplicialComplex*);
void del(size_t, vector<size_t>*);

#endif // INCLUDE_RECONSTRUCTION
