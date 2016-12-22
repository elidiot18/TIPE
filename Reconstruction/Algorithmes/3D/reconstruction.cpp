#include "reconstruction.h"
#include "miscellaneous.h"

using namespace std;

void reconstruction(vector<PointDouble> &W, ofstream &ofile) {
    size_t n = W.size();

    /* These vectors are going to be updated at each iteration */

    // The set of landmarks
    // L[i] = j ==> L_inv[j] = i
    vector<size_t> L;
    vector<size_t> L_inv(n, 0);
    L.push_back(0);

    // List of the 3 nearest neighbours in L of each point of W
    vector<Neighbourhood> neighbours(n, {0, 0, 0});

    // List of the reverse-landmarks of each point of L
    // reverse_landmarks[p] contains the points w in W such as p is in neighbours[w]
    vector< vector<size_t> > reverse_landmarks(n, {0});

    // The simplicial complex (actually, only the 3-faces)
    // CWL[w] contains a list of vectors [i, j] iff w < i && w < j && [w, i, j] is a triangle of the reconstruction
    SimplicialComplex CWL;
    init_cwl(&CWL, n);

    /* Counts the number of faces of CWL */
    size_t faces = 0;

    /* Let's begin */
    size_t i = 1;
    while (i < n) {
        cout << i << endl;

        // We add to L the point of W\L which is the farthest from L
        size_t p = farthest(W, L);
        L.push_back(p);
        L_inv[p] = i;

        /** update of neighbours and reverse_landmarks **/
        for (size_t k = 0; k < n; ++k) {
            // If i <= 2 we know how to update reverse_landmarks
            if (i == 1 || i == 2) {
                reverse_landmarks[p].push_back(k);
            }
            // If i is too small, nu is going to be greater than |L| (which is a problem)
            if (i == 1) {
                neighbours[k].p2 = p;
                continue;
            }

            size_t neigh1 = neighbours[k].p1;
            size_t neigh2 = neighbours[k].p2;
            size_t neigh3 = neighbours[k].p3;

            double d1 = distance(k, neigh1, W);
            double d2 = distance(k, neigh2, W);
            double d3 = distance(k, neigh3, W);

            double dp = distance(k, p, W);

            if (i == 2) {
                // We want the neighbours to be order by distance from k
                if (d2 < dp) {
                    neighbours[k].p3 = p;
                }
                else {
                    size_t t = neighbours[k].p2;
                    neighbours[k].p2 = p;
                    neighbours[k].p3 = t;
                }
                continue;
            }

            // i isn't a problem any longer

            // if d(k, p) < d(k, neighbours of k) then we have to update the neighbours
            if (d3 > dp) {
                del(k, &(reverse_landmarks[neighbours[k].p3]));
                neighbours[k].p3 = p;
                reverse_landmarks[p].push_back(k);

                // We want the neighbours to be order by distance from k
                if (d2 > dp) {
                    size_t t = neighbours[k].p2;
                    neighbours[k].p2 = neighbours[k].p3;
                    neighbours[k].p3 = t;
                }
                if (d1 > dp) {
                    size_t t = neighbours[k].p1;
                    neighbours[k].p1 = neighbours[k].p2;
                    neighbours[k].p2 = t;
                }
            }
        }

/*      for (size_t k = 0; k < n; ++k) {
            size_t neigh1 = neighbours[k][0];
            size_t neigh2 = neighbours[k][1];
            size_t neigh3 = neighbours[k][2];

            reverse_landmarks[neigh1].push_back(k);
            reverse_landmarks[neigh2].push_back(k);
            reverse_landmarks[neigh3].push_back(k);
        }
*/
        if (i == 1) {
            // no triangle to add !
        }
        else if (i == 2) {
            add(L[0], L[1], p, &CWL);
            ++faces;
        }
        else {
            vector<size_t> p_reverse_landmarks = reverse_landmarks[p];
            size_t l = p_reverse_landmarks.size();

            for (size_t k = 0; k < l; ++k) {
                size_t p1 = neighbours[p_reverse_landmarks[k]].p1;
                size_t p2 = neighbours[p_reverse_landmarks[k]].p2;
                size_t p3 = neighbours[p_reverse_landmarks[k]].p3;

                bool p1_witnessed = false;

                vector<size_t> p1_potential_witnesses = reverse_landmarks[p1];
                size_t l_p1 = p1_potential_witnesses.size();
                for (size_t q = 0; q < l_p1; ++q) {
                    if (neighbours[p1_potential_witnesses[q]].p1 == p1) {
                        p1_witnessed = true;
                        break;
                    }
                }

                bool p2_witnessed = false;

                vector<size_t> p2_potential_witnesses = reverse_landmarks[p2];
                size_t l_p2 = p2_potential_witnesses.size();
                for (size_t q = 0; q < l_p2; ++q) {
                    if (neighbours[p2_potential_witnesses[q]].p1 == p2) {
                        p2_witnessed = true;
                        break;
                    }
                }

                bool p3_witnessed = false;

                vector<size_t> p3_potential_witnesses = reverse_landmarks[p3];
                size_t l_p3 = p3_potential_witnesses.size();
                for (size_t q = 0; q < l_p3; ++q) {
                    if (neighbours[p3_potential_witnesses[q]].p1 == p3) {
                        p3_witnessed = true;
                        break;
                    }
                }

                if (p1_witnessed && !in(p1, CWL)) {
                    add(p1, &CWL);
                    ++faces;
                }
                else if (!p1_witnessed && in(p1, CWL)) {
                    del(p1, &CWL);
                    --faces;
                }

                if (p2_witnessed && !in(p2, CWL)) {
                    add(p2, &CWL);
                    ++faces;
                }
                else if (!p2_witnessed && in(p2, CWL)) {
                    del(p2, &CWL);
                    --faces;
                }

                if (p3_witnessed && !in(p3, CWL)) {
                    add(p3, &CWL);
                    ++faces;
                }
                else if (!p3_witnessed && in(p3, CWL)) {
                    del(p3, &CWL);
                    --faces;
                }

                if (p1_witnessed && p2_witnessed && p3_witnessed) {
                    // checking if the edge p1 -- p2 is witnessed
                    bool p1__p2_witnessed = false;
                    for (size_t q = 0; q < l_p1; ++q) {
                        if (neighbours[p1_potential_witnesses[q]].p1 == p1 && neighbours[p1_potential_witnesses[q]].p2 == p2 ||
                            neighbours[p1_potential_witnesses[q]].p1 == p2 && neighbours[p1_potential_witnesses[q]].p2 == p1) {
                            p1__p2_witnessed = true;
                            break;
                        }
                    }
                    if (!p1__p2_witnessed) {
                        for (size_t q = 0; q < l_p2; ++q) {
                            if (neighbours[p2_potential_witnesses[q]].p1 == p1 && neighbours[p2_potential_witnesses[q]].p2 == p2 ||
                                neighbours[p2_potential_witnesses[q]].p1 == p2 && neighbours[p2_potential_witnesses[q]].p2 == p1) {
                                p1__p2_witnessed = true;
                                break;
                            }
                        }
                    }

                    // checking if the edge p1 -- p3 is witnessed
                    bool p1__p3_witnessed = false;
                    for (size_t q = 0; q < l_p1; ++q) {
                        if (neighbours[p1_potential_witnesses[q]].p1 == p1 && neighbours[p1_potential_witnesses[q]].p2 == p3 ||
                            neighbours[p1_potential_witnesses[q]].p1 == p3 && neighbours[p1_potential_witnesses[q]].p2 == p1) {
                            p1__p3_witnessed = true;
                            break;
                        }
                    }
                    if (!p1__p3_witnessed) {
                        for (size_t q = 0; q < l_p3; ++q) {
                            if (neighbours[p3_potential_witnesses[q]].p1 == p1 && neighbours[p3_potential_witnesses[q]].p2 == p3 ||
                                neighbours[p3_potential_witnesses[q]].p1 == p3 && neighbours[p3_potential_witnesses[q]].p2 == p1) {
                                p1__p3_witnessed = true;
                                break;
                            }
                        }
                    }

                    // checking if the edge p2 -- p3 is witnessed
                    bool p2__p3_witnessed = false;
                    for (size_t q = 0; q < l_p2; ++q) {
                        if (neighbours[p2_potential_witnesses[q]].p1 == p2 && neighbours[p2_potential_witnesses[q]].p2 == p3 ||
                            neighbours[p2_potential_witnesses[q]].p1 == p3 && neighbours[p2_potential_witnesses[q]].p2 == p2) {
                            p2__p3_witnessed = true;
                            break;
                        }
                    }
                    if (!p2__p3_witnessed) {
                        for (size_t q = 0; q < l_p3; ++q) {
                            if (neighbours[p3_potential_witnesses[q]].p1 == p2 && neighbours[p3_potential_witnesses[q]].p2 == p3 ||
                                neighbours[p3_potential_witnesses[q]].p1 == p3 && neighbours[p3_potential_witnesses[q]].p2 == p2) {
                                p2__p3_witnessed = true;
                                break;
                            }
                        }
                    }

                    if (p1__p2_witnessed && !in(p1, p2, CWL)) {
                        add(p1, p2, &CWL);
                        ++faces;
                    }
                    else if (!p1__p2_witnessed && in(p1, p2, CWL)) {
                        del(p1, p2, &CWL);
                        --faces;
                    }

                    if (p1__p3_witnessed && !in(p1, p3, CWL)) {
                        add(p1, p3, &CWL);
                        ++faces;
                    }
                    else if (!p1__p3_witnessed && in(p1, p3, CWL)) {
                        del(p1, p3, &CWL);
                        --faces;
                    }

                    if (p2__p3_witnessed && !in(p2, p3, CWL)) {
                        add(p2, p3, &CWL);
                        ++faces;
                    }
                    else if (!p2__p3_witnessed && in(p2, p3, CWL)) {
                        del(p2, p3, &CWL);
                        --faces;
                    }

                    // if every edge is witnessed then the triangle is witnessed
                    if (p1__p2_witnessed && p1__p3_witnessed && p2__p3_witnessed) {
                        if (!in(p1, p2, p3, CWL)) {
                            add(p1, p2, p3, &CWL);
                            ++faces;
                        }
                    }
                    else {
                        if (in(p1, p2, p3, CWL)) {
                            del(p1, p2, p3, &CWL);
                            --faces;
                        }
                    }
                }
            }
        }
        // We only want the reconstruction for i = 500 (then we stop)
        if (i == 500) {
            ofile << "Reconstruction n. " << i << " : " << endl;

            //Geomview OFF format
            // Syntax : "OFF"
            ofile << "OFF" << endl;

            // Syntax : "number of vertices" "number of faces" "number of edges" (number of edges is not read so we can put whatever we want)
            ofile << i + 1 << " " << faces << " " << 42 << endl;

            // Syntax : x y z for each vertex
            for (size_t k = 0; k < i + 1; ++k) {
                ofile << W[L[k]].x << " " << W[L[k]].y << " " << W[L[k]].z << endl;
            }

            // Syntax : n i j k ... for each n-face which indexes are i j k...
            // points
            size_t n_points = CWL.points.size();
            for (size_t k = 0; k < n_points; ++k) {
                ofile << 1 << " " << L_inv[CWL.points[k]] << " 255 0 0" << endl;
            }

            // edges
            for (size_t k = 0; k < n; ++k) {
                size_t s = CWL.edges[k].size();
                for (size_t l = 0; l < s; ++l) {
                    ofile << 2 << " " << L_inv[k] << " " << L_inv[CWL.edges[k][l]] << " 0 0 0" << endl;
                }
            }

            // faces
            for (size_t k = 0; k < n; ++k) {
                size_t s = CWL.triangles[k].size();
                for (size_t l = 0; l < s; ++l) {
                    ofile << 3 << " " << L_inv[k]  << " " << L_inv[CWL.triangles[k][l][0]]  << " " << L_inv[CWL.triangles[k][l][1]] << endl;
                }
            }

            // We stop
            break;
        }
        i++;
    }
}

bool in(size_t i, size_t j, size_t k, SimplicialComplex &CWL) {
    order(&i, &j, &k);
    return find(CWL.triangles[i].begin(), CWL.triangles[i].end(), vector<size_t>{j, k}) != CWL.triangles[i].end();
}

bool in(size_t i, size_t j, SimplicialComplex &CWL) {
    order(&i, &j);
    return find(CWL.edges[i].begin(), CWL.edges[i].end(), j) != CWL.edges[i].end();
}

bool in(size_t i, SimplicialComplex &CWL) {
    return find(CWL.points.begin(), CWL.points.end(), i) != CWL.points.end();
}

void del(size_t i, size_t j, size_t k, SimplicialComplex *CWL) {
    order(&i, &j, &k);
    (CWL->triangles)[i].erase(remove((CWL->triangles)[i].begin(), (CWL->triangles)[i].end(), vector<size_t>{j, k}), (CWL->triangles)[i].end());
}

void del(size_t i, size_t j, SimplicialComplex *CWL) {
    order(&i, &j);
    (CWL->edges)[i].erase(remove((CWL->edges)[i].begin(), (CWL->edges)[i].end(), j), (CWL->edges)[i].end());
}

void del(size_t i, SimplicialComplex *CWL) {
    CWL->points.erase(remove(CWL->points.begin(), CWL->points.end(), i), CWL->points.end());
}

void add(size_t i, size_t j, size_t k, SimplicialComplex *CWL) {
    order(&i, &j, &k);
    (CWL->triangles)[i].push_back({j, k});
}

void add(size_t i, size_t j, SimplicialComplex *CWL) {
    order(&i, &j);
    (CWL->edges)[i].push_back(j);
}

void add(size_t i, SimplicialComplex *CWL) {
    CWL->points.push_back(i);
}

void del(size_t w, vector<size_t> *subset) {
    subset->erase(remove(subset->begin(), subset->end(), w), subset->end());
}
