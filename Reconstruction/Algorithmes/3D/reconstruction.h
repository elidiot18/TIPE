#ifndef INCLUDE_RECONSTRUCTION
#define INCLUDE_RECONSTRUCTION

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "geometry.h"

using namespace std;

bool is_witnessed(size_t, size_t, vector<Point>&, vector< vector<size_t> >&);
bool is_witnessed(size_t, size_t, size_t, vector<Point>&, vector< vector<size_t> >&);
bool in(size_t, size_t, vector< vector< vector< vector<size_t> > > >&);
bool in(size_t, size_t, size_t, vector< vector< vector< vector<size_t> > > >&);
void reconstruction(vector<Point>&, ofstream&);
void take_out(size_t, size_t, vector< vector< vector< vector<size_t> > > >*);
void take_out(size_t, size_t, size_t, vector< vector< vector< vector<size_t> > > >*);
void add_cwl(size_t, size_t, vector< vector< vector< vector<size_t> > > >*);
void add_cwl(size_t, size_t, size_t k, vector< vector< vector< vector<size_t> > > >*);
void add_rvl(size_t, vector<size_t>*);

#endif // INCLUDE_RECONSTRUCTION
