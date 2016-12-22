#include "reconstruction.h"
#include "miscellaneous.h"

using namespace std;

void reconstruction(vector<PointDouble> &W, ofstream &ofile) {
    size_t n = W.size();

    /* These vectors are going to be updated at each iteration */

    // The set of landmarks
    // L[i] = j ==> L_inv[j] = i
    vector<Point> L;
    vector<size_t> L_inv(n, 0);
    L.push_back(0);

    // List of the 3 nearest neighbours in L of each point of W
    vector<Neighbourhood> neighbours(n, {0, 0, 0});

    // List of the reverse-landmarks of each point of L
    // reverse_landmarks[p] contains the points w in W such as p is in neighbours[w]
    vector< vector<Point> > reverse_landmarks(n, {0});

    // The simplicial complex (actually, only the 3-faces)
    // CWL[w] contains a list of vectors [i, j] iff w < i && w < j && [w, i, j] is a triangle of the reconstruction
    SimplicialComplex CWL;

    /* Counts the number of faces of CWL */
    size_t faces = 0;

    /* Let's begin */
    size_t i = 1;
    while (i < n) {
        cout << i << endl;

        // We add to L the point of W\L which is the farthest from L
        Point p = farthest(W, L);
        L.push_back(p);
        L_inv[p] = i;

        // Will contain a list of neighbourhoods which witnesses have changed because of p
        set<Neighbourhood, Neighbourhood_less> maybe_to_be_removed;

        /** update of neighbours and reverse_landmarks **/
        for (Point k = 0; k < n; ++k) {
            // If i <= 2 we know how to update reverse_landmarks
            if (i == 1 || i == 2) {
                reverse_landmarks[p].push_back(k);
            }
            // If i is too small, nu is going to be greater than |L| (which is a problem)
            if (i == 1) {
                neighbours[k].p2 = p;
                continue;
            }

            double d1 = distance(k, neighbours[k].p1, W);
            double d2 = distance(k, neighbours[k].p2, W);
            double d3 = distance(k, neighbours[k].p3, W);

            double dp = distance(k, p, W);

            if (i == 2) {
                // We want the neighbours to be order by distance from k
                if (d2 < dp) {
                    neighbours[k].p3 = p;
                }
                else {
                    Point t = neighbours[k].p2;
                    neighbours[k].p2 = p;
                    neighbours[k].p3 = t;
                }
                continue;
            }

            // i isn't a problem any longer

            // if d(k, p) < d(k, neighbours of k) then we have to update the neighbours
            if (d3 > dp) {
                maybe_to_be_removed.insert(neighbours[k]);

                del(k, &(reverse_landmarks[neighbours[k].p3]));
                neighbours[k].p3 = p;
                reverse_landmarks[p].push_back(k);

                // We want the neighbours to be order by distance from k
                if (d2 > dp) {
                    Point t = neighbours[k].p2;
                    neighbours[k].p2 = neighbours[k].p3;
                    neighbours[k].p3 = t;
                }
                if (d1 > dp) {
                    Point t = neighbours[k].p1;
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
        /*else if (i == 2) {
            add(L[0], L[1], p, &CWL);
            ++faces;
        }*/
        else {
            vector<Point> p_reverse_landmarks = reverse_landmarks[p];
            size_t l = p_reverse_landmarks.size();
            for (Point k = 0; k < l; ++k) {
                maybe_to_be_removed.insert(neighbours[p_reverse_landmarks[k]]);
            }

            for (set<Neighbourhood>::iterator it = maybe_to_be_removed.begin(); it != maybe_to_be_removed.end(); ++it) {
                Point p1 = it->p1;
                Point p2 = it->p2;
                Point p3 = it->p3;

                bool p1_witnessed = false;

                vector<Point> p1_potential_witnesses = reverse_landmarks[p1];
                size_t l_p1 = p1_potential_witnesses.size();
                for (size_t q = 0; q < l_p1; ++q) {
                    if (neighbours[p1_potential_witnesses[q]].p1 == p1) {
                        p1_witnessed = true;
                        break;
                    }
                }

                bool p2_witnessed = false;

                vector<Point> p2_potential_witnesses = reverse_landmarks[p2];
                size_t l_p2 = p2_potential_witnesses.size();
                for (size_t q = 0; q < l_p2; ++q) {
                    if (neighbours[p2_potential_witnesses[q]].p1 == p2) {
                        p2_witnessed = true;
                        break;
                    }
                }

                bool p3_witnessed = false;

                vector<Point> p3_potential_witnesses = reverse_landmarks[p3];
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
                p1__p2_witnessed = p1__p2_witnessed && p1_witnessed && p2_witnessed;

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
                p1__p3_witnessed = p1__p3_witnessed && p1_witnessed && p3_witnessed;

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
                p2__p3_witnessed = p2__p3_witnessed && p2_witnessed && p3_witnessed;

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

                // if every edge is witnessed then the triangle is witnessed (it is itself witnessed by definition)
                if (p1__p2_witnessed && p1__p3_witnessed && p2__p3_witnessed) {
                    if (!in(p1, p2, p3, CWL)) {
                        add(p1, p2, p3, &CWL);
                        ++faces;
                    }
                }
                else if (in(p1, p2, p3, CWL)) {
                    del(p1, p2, p3, &CWL);
                    --faces;
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
            for (Point k = 0; k < i + 1; ++k) {
                ofile << W[L[k]].x << " " << W[L[k]].y << " " << W[L[k]].z << endl;
            }

            // Syntax : n i j k ... for each n-face which indexes are i j k...
            // points
            for (set<Point>::iterator it = CWL.points.begin(); it != CWL.points.end(); ++it) {
                ofile << 1 << " " << L_inv[*it] << " 255 0 0" << endl;
            }

            // edges
            for (set<Edge>::iterator it = CWL.edges.begin(); it != CWL.edges.end(); ++it) {
                ofile << 2 << " " << L_inv[it->p1] << " " << L_inv[it->p2] << " 0 0 0" << endl;
            }

            // faces
            for (set<Triangle>::iterator it = CWL.triangles.begin(); it != CWL.triangles.end(); ++it) {
                ofile << 3 << " " << L_inv[it->p1]  << " " << L_inv[it->p2]  << " " << L_inv[it->p3] << endl;
            }

            // We stop
            break;
        }
        i++;
    }
}

bool in(Point i, Point j, Point k, SimplicialComplex &CWL) {
    order(&i, &j, &k);
    Triangle t = {i, j, k};
    return CWL.triangles.find(t) != CWL.triangles.end();
}

bool in(Point i, Point j, SimplicialComplex &CWL) {
    order(&i, &j);
    Edge e = {i, j};
    return CWL.edges.find(e) != CWL.edges.end();
}

bool in(Point i, SimplicialComplex &CWL) {
    return CWL.points.find(i) != CWL.points.end();
}

void del(Point i, Point j, Point k, SimplicialComplex *CWL) {
    order(&i, &j, &k);
    Triangle t = {i, j, k};
    CWL->triangles.erase(t);
}

void del(Point i, Point j, SimplicialComplex *CWL) {
    order(&i, &j);
    Edge e = {i, j};
    CWL->edges.erase(e);
}

void del(Point i, SimplicialComplex *CWL) {
    CWL->points.erase(i);
}

void add(Point i, Point j, Point k, SimplicialComplex *CWL) {
    order(&i, &j, &k);
    Triangle t = {i, j, k};
    CWL->triangles.insert(t);
}

void add(Point i, Point j, SimplicialComplex *CWL) {
    order(&i, &j);
    Edge e = {i, j};
    CWL->edges.insert(e);
}

void add(Point i, SimplicialComplex *CWL) {
    CWL->points.insert(i);
}

void del(Point w, vector<Point> *subset) {
    subset->erase(remove(subset->begin(), subset->end(), w), subset->end());
}
