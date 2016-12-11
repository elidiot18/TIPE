// Imports
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

inline string remove_extension(string &s) {
    size_t lastdot = s.find_last_of('.');
    if (lastdot == string::npos) return s;
    return s.substr(0, lastdot);
}

double distance(size_t p1, size_t p2, vector< vector<double> > &W) {
    return (W[p1][0] - W[p2][0]) * (W[p1][0] - W[p2][0]) + (W[p1][1] - W[p2][1]) * (W[p1][1] - W[p2][1]);
}

double distance(size_t p, vector<size_t> &L, vector< vector<double> > &W) {
    size_t l = L.size();
    double d = distance(p, L[0], W);
    for (size_t k = 0; k < l; ++k) {
        double dk = distance(p, L[k], W);
        if (dk < d) d = dk;
    }
    return d;
}

size_t farthest(vector< vector<double> > &W, vector<size_t> &L) {
    size_t n = W.size();
    size_t m = 0;
    double dm = distance(m, L, W);
    for (size_t j = 1; j < n; ++j) {
        double dj = distance(j, L, W);
        if (dm < dj) {
            m = j;
        }
    }
    return m;
}

bool is_witnessed(size_t i, size_t j, vector< vector<double> > &W, vector< vector<size_t> > neighbours) {
    size_t n = W.size();
    for (size_t k = 0; k < n; ++k) {
        vector<size_t> neigh = neighbours[k];
        if ((neigh[0] == i && neigh[1] == j) || (neigh[0] == j && neigh[1] == i)) {
            return true;
        }
    }
    return false;
}

bool in(size_t i, vector<size_t> siblings) {
    return find(siblings.begin(), siblings.end(), i) != siblings.end();
}

bool in(size_t i, size_t j, vector< vector<size_t> > &CWL) {
    return i < j ? in(j, CWL[i]) : in(i, CWL[j]);
}

void add(size_t i, vector<size_t> *siblings) {
    siblings->push_back(i);
}

void add(size_t i, size_t j, vector< vector<size_t> > *CWL) {
    i < j ? add(j, &((*CWL)[i])) : add(i, &((*CWL)[j]));
}

void take_out(size_t i, vector<size_t> *siblings) {
    siblings->erase(remove(siblings->begin(), siblings->end(), i), siblings->end());
}

void take_out(size_t i, size_t j, vector< vector<size_t> > *CWL) {
    i < j ? take_out(j, &((*CWL)[i])) : take_out(i, &((*CWL)[j]));
}

void reconstruction(vector< vector<double> > &W, ofstream &ofile) {
    size_t n = W.size();
    //L is the set of landmarks, to which is added a point of W at each iteration
    vector<size_t> L;
    L.push_back(0);

    // neighbours is the list of pointers to the 2-nearest landmarks of each point of W
    vector< vector<size_t> > neighbours(n, vector<size_t>(2, 0));

    // CWL is the witness complex of L
    // Structure : adjacency list
    vector< vector<size_t> > CWL(n);

    size_t i = 1;
    while (i < n) {
        // j is the index of the point of W which is the furthest from L
        size_t p = farthest(W, L);

        // We add p to the list of the landmarks
        L.push_back(p);

        // We search for the 2-reverse-nearest landmarks of p, and the list of the former neighbours of those witnesses
        // reverse_landmarks is going to contain the reverse nearest landmarks of p (their former neighbours [former_n1, former_n2])
        vector< vector<size_t> > reverse_landmarks;

        for (size_t k = 0; k < n; ++k) {
            // nearest1 and nearest2 point to the nearest landmarks from W[i]
            size_t *nearest1 = &(neighbours[k][0]);
            size_t *nearest2 = &(neighbours[k][1]);

            // d1 and d2 are the respective distances to W[i]
            double d1 = distance(k, *nearest1, W);
            double d2 = distance(k, *nearest2, W);

            // farthest will point to the farthest neighbour
            size_t *farthest = (d1 > d2 ? nearest1 : nearest2);

            // df resp. dp are the distances from W[i] to farthest resp. p
            double df = distance(k, *farthest, W);
            double dp = distance(k, p, W);

            // if we have to do an update of W[i]'s neighbours
            if (df > dp) {
                // W[i] is a reverse landmark of p ! We keep in memory its former two nearest neighbours from L, in reverse_landmarks
                if (find(reverse_landmarks.begin(), reverse_landmarks.end(), vector<size_t>{*nearest1, *nearest2}) == reverse_landmarks.end() && find(reverse_landmarks.begin(), reverse_landmarks.end(), vector<size_t>{*nearest2, *nearest1}) == reverse_landmarks.end()) {
                    reverse_landmarks.push_back({*nearest1, *nearest2});
                }
                *farthest = p;
            }
        }

        size_t q = reverse_landmarks.size();
        cout << q << endl;
        for (size_t k = 0; k < q; ++k) {
            if (in(reverse_landmarks[k][1], reverse_landmarks[k][2], CWL)) {
                // (k, p) is an edge of CWL : do we have to remove it ?
                if (!is_witnessed(reverse_landmarks[k][1], reverse_landmarks[k][2], W, neighbours)) {
                    take_out(reverse_landmarks[k][1], reverse_landmarks[k][2], &CWL);
                }
            }
            else {
                // (k, p) is not an edge of CWL : do we have to add it ?
                if (is_witnessed(reverse_landmarks[k][1], p, W, neighbours)) {
                    add(reverse_landmarks[k][1], p, &CWL);
                }
                if (is_witnessed(reverse_landmarks[k][0], p, W, neighbours)) {
                    add(reverse_landmarks[k][0], p, &CWL);
                }
            }
        }

        ofile << "Reconstruction n. " << i << " : " << endl;

        size_t m = CWL.size();
        for (size_t k = 0; k < m; ++k) {
            size_t s = CWL[k].size();
            for (size_t l = 0; l < s; ++l) {
                double x1 = W[k][0], y1 = W[k][1];
                double x2 = W[l][0], y2 = W[l][1];
                ofile << "plt.plot((" << x1 << ", " << x2 << "), " << "(" << y1 << ", " << y2 << "))" << endl;
            }
        }

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

    vector< vector<double> > W;
    size_t i = 0;
    while (!ifile.eof()) {
        vector<double> w(2, 0);
        W.push_back(w);
        double x, y;
        ifile >> x;
        ifile >> y;
        W[i][0] = x;
        W[i][1] = y;
        i++;
    }
    reconstruction(W, ofile);
    ifile.close();
    ofile.close();
    return 0;
}
