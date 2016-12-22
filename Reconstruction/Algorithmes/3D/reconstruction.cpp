#include "reconstruction.h"
#include "miscellaneous.h"

using namespace std;

void reconstruction(vector<Point> &W, ofstream &ofile) {
    size_t n = W.size();
    vector<size_t> L;
    vector<size_t> L_inv(n, 0);

    L.push_back(0);

    vector< vector<size_t> > neighbours(n, vector<size_t>(3, 0));

    vector< vector < vector< vector<size_t> > > > CWL(2, vector < vector< vector<size_t> > >(n));

    size_t faces = 0;

    size_t i = 1;
    while (i < n) {
        size_t p = farthest(W, L);

        L.push_back(p);
        L_inv[p] = i;

        vector<size_t> reverse_landmarks;

        for (size_t k = 0; k < n; ++k) {
            size_t neigh1 = neighbours[k][0];
            size_t neigh2 = neighbours[k][1];
            size_t neigh3 = neighbours[k][2];

            double d1 = distance(k, neigh1, W);
            double d2 = distance(k, neigh2, W);
            double d3 = distance(k, neigh3, W);

            order(&neigh1, &neigh2, &neigh3);

            double dp = distance(k, p, W);

            if (d3 > dp) {
                if (i - 1 && i - 2) {
                    add_rvl(neigh1, &reverse_landmarks);
                    add_rvl(neigh2, &reverse_landmarks);
                    add_rvl(neigh3, &reverse_landmarks);
                }

                neighbours[k][2] = p;

                // We want k neighbours to be order by distance from k
                if (d2 > dp) {
                    size_t t = neighbours[k][1];
                    neighbours[k][1] = neighbours[k][2];
                    neighbours[k][2] = t;
                }
                if (d1 > dp) {
                    size_t t = neighbours[k][0];
                    neighbours[k][0] = neighbours[k][1];
                    neighbours[k][1] = t;
                }
            }
        }

        // If i = 1 or 2, we don't want to compute the reverse landmarks etc.
        if (i == 1) {
            add_cwl(L[0], p, &CWL);
        }
        else if (i == 2) {
            add_cwl(L[0], p, &CWL);
            add_cwl(L[1], p, &CWL);
            add_cwl(L[0], L[1], p, &CWL);
            ++faces;
        }
        else {
            size_t q = reverse_landmarks.size();

            for (size_t k = 0; k < q; ++k) {
                size_t p1 = reverse_landmarks[k];

                // Try to add an edge beginning from p
                if (!in(p1, p, CWL) && is_witnessed(p1, p, W, neighbours)) {
                    /*ofile << "hello edge" << p1 << " " << p  << " : " << L_inv[p1] << " " << L_inv[p] << endl;*/
                    add_cwl(p1, p, &CWL);
                }

                for (size_t l = k + 1; l < q; ++l) {
                    size_t p2 = reverse_landmarks[l];

                    // Try to remove edge
                    if (in(p1, p2, CWL) && !is_witnessed(p1, p2, W, neighbours)) {
                        /*ofile << "bye bye edge" << p1 << " " << p2  << " : " << L_inv[p1] << " " << L_inv[p2] << endl;*/
                        take_out(p1, p2, &CWL);
                    }

                    // Try to add triangle
                    if (!in(p1, p2, p, CWL) && is_witnessed(p1, p2, p, W, neighbours)) {
                        /*ofile << "hello triangle" << endl;*/
                        add_cwl(p1, p2, p, &CWL);
                        ++faces;
                    }

                    for (size_t m = l + 1; m < q; ++m) {
                        size_t p3 = reverse_landmarks[m];

                        // Try to remove triangle
                        if (in(p1, p2, p3, CWL) && !is_witnessed(p1, p2, p3, W, neighbours)) {
                            /*ofile << "bye bye triangle" << endl;*/
                            take_out(p1, p2, p3, &CWL);
                            --faces;
                        }
                    }
                }
            }
        }

        // We only want the reconstruction for i = 500 (then we stop)
        if (i == 200) {
            ofile << "Reconstruction n. " << i << " : " << endl;

            //Geomview OFF format
            // Syntax : "OFF"
            ofile << "OFF" << endl;

            // Syntax : "number of vertices" "number of faces" "number of edges" (number of edges is not read so we can put whatever we want)
            ofile << i + 1 << " " << faces << " " << 42 << endl;

            // Syntax : x y z for each vertex
            for (size_t k = 0; k < i + 1; ++k) {
                ofile << "\t" << W[L[k]].x << " " << W[L[k]].y << " " << W[L[k]].z << endl;
            }

            // Syntax : n i j k ... for each n-face which indexes are i j k...
            // edges
            for (size_t k = 0; k < n; ++k) {
                size_t s = CWL[0][k].size();
                for (size_t l = 0; l < s; ++l) {
                    ofile << 2 << " " << L_inv[k] << " " << L_inv[CWL[0][k][l][0]] << endl;
                }
            }
            // faces
            for (size_t k = 0; k < n; ++k) {
                size_t s = CWL[1][k].size();
                for (size_t l = 0; l < s; ++l) {
                    ofile << 3 << " " << L_inv[k]  << " " << L_inv[CWL[1][k][l][0]]  << " "  << L_inv[CWL[1][k][l][1]] << endl;
                }
            }

            // We stop
            break;

        }
        i++;
    }
}

bool is_witnessed(size_t i, size_t j, vector<Point> &W, vector< vector<size_t> > &neighbours) {
    if (i == j) {
        return false;
    }
    size_t n = W.size();
    for (size_t k = 0; k < n; ++k) {
        vector<size_t> neigh = neighbours[k];
        if ((neigh[0] == i && neigh[1] == j) || (neigh[0] == j && neigh[1] == i)) {
            return true;
        }
    }
    return false;
}

bool is_witnessed(size_t i, size_t j, size_t k, vector<Point> &W, vector< vector<size_t> > &neighbours) {
    if (i == j || j == k || i == k) {
        return false;
    }

    order(&i, &j, &k);

    size_t n = W.size();
    for (size_t k = 0; k < n; ++k) {
        vector<size_t> neigh = neighbours[k];

        order(&(neigh[0]), &(neigh[1]), &(neigh[2]));

        if (neigh[0] == i && neigh[1] == j && neigh[2] == k) {
            return true;
        }
    }
    return false;
}

bool in(size_t i, size_t j, vector< vector< vector< vector<size_t> > > > &CWL) {
    return find(CWL[0][i].begin(), CWL[0][i].end(), vector<size_t>{j}) != CWL[0][i].end();
}

bool in(size_t i, size_t j, size_t k, vector< vector< vector< vector<size_t> > > > &CWL) {
    return find(CWL[1][i].begin(), CWL[1][i].end(), vector<size_t>{j, k}) != CWL[1][i].end();
}

void take_out(size_t i, size_t j, vector< vector< vector< vector<size_t> > > > *CWL) {
    ((*CWL)[0][i]).erase(remove(((*CWL)[0][i]).begin(), ((*CWL)[0][i]).end(), vector<size_t>{j}), ((*CWL)[0][i]).end());
}

void take_out(size_t i, size_t j, size_t k, vector< vector< vector< vector<size_t> > > > *CWL) {
    ((*CWL)[1][i]).erase(remove(((*CWL)[1][i]).begin(), ((*CWL)[1][i]).end(), vector<size_t>{j, k}), ((*CWL)[1][i]).end());
}

void add_cwl(size_t i, size_t j, vector< vector< vector< vector<size_t> > > > *CWL) {
    i < j ? ((*CWL)[0][i]).push_back({j}) : ((*CWL)[0][j]).push_back({i});
}

void add_cwl(size_t i, size_t j, size_t k, vector< vector< vector< vector<size_t> > > > *CWL) {
    order(&i, &j, &k);
    ((*CWL)[1][i]).push_back({j, k});
}

void add_rvl(size_t i, vector<size_t> *reverse_landmarks) {
    if (find(reverse_landmarks->begin(), reverse_landmarks->end(), i) == reverse_landmarks->end()) {
        reverse_landmarks->push_back(i);
    }
}
