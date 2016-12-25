#ifndef INCLUDE_RECONSTRUCTION
#define INCLUDE_RECONSTRUCTION

#include <climits>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>
#include "geometry.h"

#define POINT_UNDEF UINT_MAX

void reconstruction(std::vector<Point>&, std::ofstream&);
void del(Point*, std::vector<Point*>*);

#endif // INCLUDE_RECONSTRUCTION
