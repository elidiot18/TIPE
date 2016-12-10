// Imports
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

inline string remove_extension(string &s) {
    size_t lastdot = s.find_last_of('.');
    if (lastdot == string::npos) return s;
    return s.substr(0, lastdot);
}

double distance(vector<double> *p1, vector<double> *p2) {
    return ((*p1)[0] - (*p2)[0]) * ((*p1)[0] - (*p2)[0]) + ((*p1)[1] - (*p2)[1]) * ((*p1)[1] - (*p2)[1]);
}

double distance(vector<double> *p, vector< vector<double>* > &L) {
    size_t l = L.size();
    double d = distance(p, L[0]);
    for (size_t k = 0; k < l; ++k) {
        double dk = distance(p, L[k]);
        if (dk < d) d = dk;
    }
    return d;
}

size_t farthest(vector< vector<double> > &points, vector< vector<double>* > &L) {
    size_t p = points.size();
    size_t m = 0;
    double dm = distance(&(points[m]), L);
    for (size_t j = 1; j < p; ++j) {
        double dj = distance(&(points[j]), L);
        if (dm < dj) {
            m = j;
        }
    }
    return m;
}

void update_neighbours(vector< vector<double> > &points, vector< vector< vector<double>* > > *neighbours, vector<double> *last) {
    size_t n = points.size();
    for (size_t i = 0; i < n; ++i) {
        vector<double> **nearest1 = &((*neighbours)[i][0]);
        vector<double> **nearest2 = &((*neighbours)[i][1]);
        double d1 = distance(&(points[i]), *nearest1);
        double d2 = distance(&(points[i]), *nearest2);
        vector<double> **farthest = (d1 > d2 ? nearest1 : nearest2);
        double d = distance(&(points[i]), *farthest);
        if (d > distance(&(points[i]), last)) {
            *farthest = last;
        }
    }
}

void reconstruction(vector< vector<double> > &points, ofstream &ofile) {
    size_t n = points.size();
    vector< vector<double>* > L;
    L.push_back(&(points[0]));
    vector< vector< vector<double>* > > neighbours(n, vector< vector<double>* >(2, L[0]));
    vector< vector< vector<double>* > > CWL;
    size_t i = 1;
    while (i < n) {
        size_t j = farthest(points, L);
        vector<double> *cur = &(points[j]);
        L.push_back(cur);
        if (i == 1) {
            CWL.push_back({L[0], cur});
            update_neighbours(points, &neighbours, cur);
            i++;
            continue;
        }
        else if (i == 2) {
            CWL.push_back({L[0], cur});
            CWL.push_back({L[1], cur});
            update_neighbours(points, &neighbours, cur);
            i++;
            continue;
        }

        ofile << "Reconstruction n. " << i << " : " << endl;
        vector<double> *nearest1 = neighbours[j][0];
        vector<double> *nearest2 = neighbours[j][1];
        CWL.push_back({nearest1, cur});
        CWL.push_back({nearest2, cur});
        size_t m = CWL.size();
        ofile << m << endl;
        for (size_t k = 0; k < m; ++k) {
            if ((CWL[k][0] == nearest1 or CWL[k][0] == nearest2) and (CWL[k][1] == nearest1 or CWL[k][1] == nearest2)) {
                CWL[k] = {NULL, NULL};
            }
            /*else if (CWL[k][0] != NULL) {
                ofile << (*(CWL[k][0]))[0] << " , " << (*(CWL[k][0]))[1]
                      << " ; "
                      << (*(CWL[k][1]))[0] << " , " << (*(CWL[k][1]))[1] << endl;
            }*/
            else if (CWL[k][0] != NULL) {
                ofile << "plt.plot((" << (*(CWL[k][0]))[0] << " , " << (*(CWL[k][1]))[0]
                      << "), ("
                      << (*(CWL[k][0]))[1] << " , " << (*(CWL[k][1]))[1] << "))" << endl;
            }
        }
        update_neighbours(points, &neighbours, cur);
        i++;
    }
}

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
    if (dim != "2") {
        cout << "This algorithm does only work for dimension 2";
    }

    vector< vector<double> > points;
    size_t i = 0;
    while (!ifile.eof()) {
        vector<double> point(2, 0);
        points.push_back(point);
        double x, y;
        ifile >> x;
        ifile >> y;
        points[i][0] = x;
        points[i][1] = y;
        i++;
    }
    reconstruction(points, ofile);
    ifile.close();
    ofile.close();
    return 0;
}
