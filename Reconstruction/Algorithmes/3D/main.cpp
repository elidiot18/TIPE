#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include "reconstruction.cpp"
#include "geometry.cpp"
#include "miscellaneous.cpp"

using namespace std;

int main(int argc, char **argv) {
    if (argc <= 1) {
        cout << "You need to specify a file";
    }
    if (argc > 2) {
        cout << "There is only one argument to be specified";
    }
    ifstream ifile;
    ifile.open(argv[1], ios::in);
    if (!ifile.is_open()) {
        return -1;
    }
    string name = argv[1];
    name = remove_extension(name);
    name = name + (string)".cplx";
    ofstream ofile(name.c_str(), ios::out | ios::trunc);
    if (!ofile.is_open()) {
        return -1;
    }

    string dim;
    getline(ifile, dim);
    if (dim != "3") {
        cout << "This algorithm does only work for dimension 3";
        return 42;
    }

    vector<Point> W;

    size_t i = 0;
    while (ifile) {
        double x, y, z;
        ifile >> x;
        ifile >> y;
        ifile >> z;
        Point w(i, x, y, z);
        W.push_back(move(w));
        ++i;
    }

    vector<Point*> _W;
    for (auto w : W)
        _W.push_back(&w);

    reconstruction(_W, ofile);
    ifile.close();
    ofile.close();
    return 0;
}
