#include "reconstruction.h"
#include "miscellaneous.h"

using namespace std;

void reconstruction(vector<Point*>& W, ofstream& ofile) {
    /***************
     * nu sequence *
     ***************/

    size_t nu_0 = 1, nu_1 = 2, nu_2 = 3;

    /** reconstruction **/

    size_t n = W.size();

    /* These vectors are going to be updated at each iteration */

    // The set of landmarks
    // L[i] = j ==> L_inv[j] = i
    vector<Point*> L;
    vector<size_t> L_inv(n, 0);
    L.push_back(W[0]);

    for (Point* w : W) {
        w->neighbourhood.insert(pair<double, Point*>(distance(w, L[0]), L[0]));
    }

    // List of the reverse-landmarks of each point of L
    // reverse_landmarks[p] contains the points w in W such as p is in neighbours[w]
    vector<vector<Point*>> reverse_landmarks(n, {L[0]});


    /* This vector is going to be built for the displaying */

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
        Point* p = farthest(W, L);
        L.push_back(p);
        L_inv[p->index] = i;

        /* update of the neighbourhoods and reverse_landmarks */
        for (Point* w : W) {
//            cout<<w->index<<endl;
            if (i <= nu_2 - 1) {
                // if i <= max(\nu_i) - 1 we know how to update reverse_landmarks and w.neighbourhood
                reverse_landmarks[p->index].push_back(w);
                w->neighbourhood.insert(pair<double, Point*>(distance(w, p), p));
                auto& points = w->neighbourhood;
                for (const auto& point : points) {
                    w->simplices.edges.emplace(Edge(point.second, p));
                }

                for (auto p1 = points.begin(); p1 != points.end(); ++p1) {
                    for (auto p2 = next(p1); p2 != points.end(); ++p2) {
                        w->simplices.triangles.emplace(Triangle(p1->second, p2->second, p));
                    }
                }
            }
            else {
                auto& points = w->neighbourhood;

                // odd is w's neighbour which is the furthest from w
                auto odd = points.rbegin().base();

                if (odd->first > distance(w, p)) {
                    cout << "foobar" << endl;
                    // as we remove odd, all triangles containing odd aren't witnessed by w any longer
                    for (auto t = w->simplices.triangles.begin(); t != w->simplices.triangles.end(); ++t) {
                        if (t->index1 == odd->second->index || t->index2 == odd->second->index || t->index3 == odd->second->index) {
                            w->simplices.triangles.erase(t);
                        }
                    }

                    // same for the edge
                    for (auto e = w->simplices.edges.begin(); e != w->simplices.edges.end(); ++e) {
                        if (e->index1 == odd->second->index || e->index2 == odd->second->index) {
                            w->simplices.edges.erase(e);
                        }
                    }

                    // w is not a reverse landmark of odd any longer
                    del(w, &(reverse_landmarks[odd->second->index]));
                    // but now, w is a reverse landmark of p
                    reverse_landmarks[p->index].push_back(w);

                    points.erase(odd);
                    // we insert the new neighbour and get its position in p_it
                    auto p_it = w->neighbourhood.insert(pair<double, Point*>(distance(w, p), p));

                    for (auto p1 = points.begin(); p1 != points.end(); ++p1) {
                        cout << "hello1" << endl;
                        if (p1 != p_it) {
                            for (auto p2 = next(p1); p2 != points.end(); ++p2) {
                                cout << "hello2" << endl;
                                if (p2 != p_it) {
                                    w->simplices.triangles.emplace(Triangle(p1->second, p2->second, p));
                                    cout << "yo" << endl;
                                }
                            }
                        }
                    }

                    // nu_1 is the number of neighbours to consider for an edge
                    auto edge_max = next(w->neighbourhood.begin(), nu_1);

                    if (p_it->first < edge_max->first) {
                        for (auto point = points.begin(); point != edge_max; ++point) {
                            if (point != p_it) {
                                w->simplices.edges.emplace(Edge(point->second, p));
                            }
                        }
                    }
                }
                else {
                    // w is not affect by p
                }
            }
        }

        if (i == 1) {
            // no triangle to add !
        }
        else {
            vector<Point*>& p_reverse_landmarks = reverse_landmarks[p->index];
            for (Point* w : p_reverse_landmarks) {
                for (Triangle t : w->simplices.triangles) {
                    //we want to know if we have to add the triangle [p1, p2, p3]
                    Point* p1 = W[t.index1];
                    Point* p2 = W[t.index2];
                    Point* p3 = W[t.index3];

                    vector<Point*> p1_potential_witnesses = reverse_landmarks[p1->index];
                    vector<Point*> p2_potential_witnesses = reverse_landmarks[p2->index];
                    vector<Point*> p3_potential_witnesses = reverse_landmarks[p3->index];

                    Edge e1 = Edge(p1, p2);
                    Edge e2 = Edge(p1, p3);
                    Edge e3 = Edge(p2, p3);

                    // checking if the edge e1 is witnessed
                    bool e1_witnessed = false;

                    for (const auto rev_land : p1_potential_witnesses) {
                        if (rev_land->simplices.edges.find(e1) != rev_land->simplices.edges.end()) {
                            e1_witnessed = true;
                            break;
                        }
                    }
                    if (!e1_witnessed) {
                        for (const auto rev_land : p2_potential_witnesses) {
                            if (rev_land->simplices.edges.find(e1) != rev_land->simplices.edges.end()) {
                                e1_witnessed = true;
                                break;
                            }
                        }
                    }

                    // checking if the edge e2 is witnessed
                    bool e2_witnessed = false;

                    for (const auto rev_land : p1_potential_witnesses) {
                        if (rev_land->simplices.edges.find(e2) != rev_land->simplices.edges.end()) {
                            e2_witnessed = true;
                            break;
                        }
                    }
                    if (!e2_witnessed) {
                        for (const auto rev_land : p3_potential_witnesses) {
                            if (rev_land->simplices.edges.find(e2) != rev_land->simplices.edges.end()) {
                                e2_witnessed = true;
                                break;
                            }
                        }
                    }

                    // checking if the edge e3 is witnessed
                    bool e3_witnessed = false;

                    for (const auto rev_land : p2_potential_witnesses) {
                        if (rev_land->simplices.edges.find(e3) != rev_land->simplices.edges.end()) {
                            e3_witnessed = true;
                            break;
                        }
                    }
                    if (!e3_witnessed) {
                        for (const auto rev_land : p3_potential_witnesses) {
                            if (rev_land->simplices.edges.find(e3) != rev_land->simplices.edges.end()) {
                                e3_witnessed = true;
                                break;
                            }
                        }
                    }

                    if (e1_witnessed) {
                        if (w->simplices.edges.insert(e1).second)
                            ++faces;
                    }
                    else {
                        if (!w->simplices.edges.erase(e1))
                           --faces;
                    }


                    if (e2_witnessed) {
                        if (w->simplices.edges.insert(e2).second)
                            ++faces;
                    }
                    else {
                        if (!w->simplices.edges.erase(e2))
                           --faces;
                    }

                    if (e3_witnessed) {
                        if (w->simplices.edges.insert(e3).second)
                            ++faces;
                    }
                    else {
                        if (!w->simplices.edges.erase(e3))
                           --faces;
                    }

                    // if every edge is witnessed then the triangle is witnessed (it is itself witnessed by definition)
                    // for the moment, t is in w.simplices
                    // do we have to remove it ?
                    if (!(e1_witnessed && e2_witnessed && e3_witnessed)) {
                        w->simplices.triangles.erase(t);
                    }
                }
            }
        }

        // We only want the reconstruction for i = 500 (then we stop)
        if (i == 100) {
            for (Point* w : W) {
                for (Edge e : w->simplices.edges) {
                    CWL.edges.insert(e);
                }
                for (Triangle t : w->simplices.triangles) {
                    CWL.triangles.insert(t);
                }
            }

            //Geomview OFF format
            // Syntax : "OFF"
            ofile << "OFF" << endl;

            // Syntax : "number of vertices" "number of faces" "number of edges" (number of edges is not read so we can put whatever we want)
            ofile << i + 1 << " " << faces << " " << 42 << endl;

            // Syntax : x y z for each vertex
            for (Point* p : L) {
                ofile << p->coordinates.x << " " << p->coordinates.y << " " << p->coordinates.z << endl;
            }

            // Syntax : n i j k ... for each n-face which indexes are i j k...
            // points
            for (Point* p : L) {
                ofile << 1 << " " << p->index << " 255 0 0" << endl;
            }

            // edges
            for (const Edge& e : CWL.edges) {
                ofile << 2 << " " << L_inv[e.index1] << " " << L_inv[e.index2] << " 0 0 0" << endl;
            }

            // faces
            for (const Triangle& t : CWL.triangles) {
                ofile << 3 << " " << L_inv[t.index1]  << " " << L_inv[t.index2]  << " " << L_inv[t.index3] << endl;
            }

            // We stop
            break;
        }
        i++;
    }
}

void del(Point* w, vector<Point*> *subset) {
    subset->erase(remove(subset->begin(), subset->end(), w), subset->end());
}
