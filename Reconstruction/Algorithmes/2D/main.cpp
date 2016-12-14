// Imports
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

inline string remove_extension(string&);
double distance(size_t, size_t, vector< vector<double> >&);
double distance(size_t, size_t, vector< vector<double> >&);
double distance(size_t, vector<size_t>&, vector< vector<double> >&);
size_t farthest(vector< vector<double> >&, vector<size_t>&);
bool is_witnessed(size_t, size_t, vector< vector<double> >&, vector< vector<size_t> >&);
bool in(size_t, vector<size_t>&);
bool in(size_t, size_t, vector< vector<size_t> >&);
void add(size_t, vector<size_t>*);
void add(size_t, size_t, vector< vector<size_t> >*);
void take_out(size_t, vector<size_t>*);
void take_out(size_t, size_t, vector< vector<size_t> >*);
void reconstruction(vector< vector<double> >&, ofstream&);

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
        vector<size_t> reverse_landmarks;
        vector< vector<size_t> > reverse_landmarks_n;

        for (size_t k = 0; k < n; ++k) {
            // nearest1 and nearest2 are the nearest landmarks of W[i]
            size_t neigh1 = neighbours[k][0];
            size_t neigh2 = neighbours[k][1];

            // d1 and d2 are their respective distances to W[i]
            double d1 = distance(k, neigh1, W);
            double d2 = distance(k, neigh2, W);

            //we want neigh1 to be the farthest
            size_t farthest = 0;
            if (d1 < d2) {
                size_t t = neigh1;
                neigh1 = neigh2;
                neigh2 = t;
                farthest = 1;
            }

            // df resp. dp are the distances from W[i] to the farthest resp. p
            double df = d1 > d2 ? d1 : d2;
            double dp = distance(k, p, W);

            // if we have to do an update of W[i]'s neighbours
            if (df > dp || neigh1 == neigh2) {
                // W[i] is a reverse landmark of p ! We keep in memory its former two nearest neighbours from L, in reverse_landmarks
                if (i != 1 && find(reverse_landmarks_n.begin(), reverse_landmarks_n.end(), vector<size_t>{neigh1, neigh2}) == reverse_landmarks_n.end() && find(reverse_landmarks_n.begin(), reverse_landmarks_n.end(), vector<size_t>{neigh2, neigh1}) == reverse_landmarks_n.end()) {
                    reverse_landmarks_n.push_back({neigh1, neigh2});
                }
                if (i != 1 && find(reverse_landmarks.begin(), reverse_landmarks.end(), neigh1) == reverse_landmarks.end()) {
                    reverse_landmarks.push_back(neigh1);
                }
                if (i != 1 && find(reverse_landmarks.begin(), reverse_landmarks.end(), neigh2) == reverse_landmarks.end()) {
                    reverse_landmarks.push_back(neigh2);
                }
                neighbours[k][farthest] = p;
            }
        }
        if (i == 1) {
            add(0, p, &CWL);
        }
        else {
            size_t q = reverse_landmarks_n.size();
            for (size_t k = 0; k < q; ++k) {
                //ofile << "\ncurrently tested reverse landmarks_n : " << reverse_landmarks_n[k][0] << "; " << reverse_landmarks_n[k][1] << endl;
                if (in(reverse_landmarks_n[k][0], reverse_landmarks_n[k][1], CWL)) {
                    // (k, p) is an edge of CWL : do we have to remove it ?
                    //ofile << "this belongs to the CWL" << endl;
                    if (!is_witnessed(reverse_landmarks_n[k][0], reverse_landmarks_n[k][1], W, neighbours)) {
                        take_out(reverse_landmarks_n[k][0], reverse_landmarks_n[k][1], &CWL);
                        //ofile << "it's been removed" << endl;
                    }
                    /*else {
                        ofile << "no problem for this edge" << endl;
                    }*/
                }
            }
            q = reverse_landmarks.size();
            for (size_t k = 0; k < q; ++k) {
                // (k, p) is not an edge of CWL : do we have to add it ?
                if (is_witnessed(reverse_landmarks[k], p, W, neighbours)) {
                    add(reverse_landmarks[k], p, &CWL);
                    //ofile << "added 1" << endl;
                }
            }
        }


        ofile << "Reconstruction n. " << i << " : " << endl;

        size_t m = CWL.size();
        for (size_t k = 0; k < m; ++k) {
            size_t s = CWL[k].size();
            for (size_t l = 0; l < s; ++l) {
                double x1 = W[k][0], y1 = W[k][1];
                double x2 = W[CWL[k][l]][0], y2 = W[CWL[k][l]][1];
                ofile << "plt.plot((" << x1 << ", " << x2 << "), " << "(" << y1 << ", " << y2 << "))" << endl;
                //ofile << "Edge : " << k << "; " << CWL[k][l] << endl;
            }
        }

        i++;
    }
}

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
        if (dk < d) {
            d = dk;
        }
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
            dm = dj;
            m = j;
        }
    }
    return m;
}

bool is_witnessed(size_t i, size_t j, vector< vector<double> > &W, vector< vector<size_t> > &neighbours) {
    size_t n = W.size();
    for (size_t k = 0; k < n; ++k) {
        vector<size_t> neigh = neighbours[k];
        if ((neigh[0] == i && neigh[1] == j) || (neigh[0] == j && neigh[1] == i)) {
            return true;
        }
    }
    return false;
}

bool in(size_t i, vector<size_t> &siblings) {
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
