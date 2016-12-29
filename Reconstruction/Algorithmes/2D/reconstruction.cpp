
#include "reconstruction.h"
#include "miscellaneous.h"

using namespace std;

void reconstruction(vector<Point*>& W, ofstream& ofile)
{
    /***************
     * nu sequence *
     ***************/

    size_t nu_0 = 1, nu_1 = 2;

    /** reconstruction **/

    size_t n = W.size();

    /* These vectors are going to be updated at each iteration */

    // The set of landmarks
    // L[i] = j ==> L_inv[j] = i
    vector<Point*> L;
    vector<size_t> L_inv(n, 0);
    L.push_back(W[0]);

    for (Point* w : W)
    {
        w->neighbourhood.emplace(make_pair(_distance(w, L[0]), L[0]));
    }

    /* This vector is going to be built for the displaying */

    // The simplicial complex (actually, only the 3-faces)
    // CWL[w] contains a list of vectors [i, j] iff w < i && w < j && [w, i, j] is a triangle of the
    // reconstruction
    SimplicialComplex CWL;

    /* Counts the number of faces of CWL */
    size_t faces = 0;

    /* Let's begin */
    size_t i = 1;
    while (i < n)
    {
        // We add to L the point of W\L which is the farthest from L
        Point* p = farthest(W, L);
        L.push_back(p);
        L_inv[p->index] = i;

        /* update of the neighbourhoods */
        for (Point* w : W)
        {
            auto& neighbours = w->neighbourhood;

            ////////////////////////////
            // If neighbours is not full
            ////////////////////////////
            if (i <= nu_1 - 1)
            {
                auto p_it = neighbours.emplace(make_pair(_distance(w, p), p));

                for (auto point = neighbours.begin(); point != neighbours.end(); ++point)
                {
                    if (point != p_it)
                    {
                        w->edges.emplace(Edge(point->second, p));
                    }
                }
            }
            else
            {
                auto odd_it = --neighbours.end();
                auto odd = *odd_it;

                // If w is affected
                ////////////////////////////////
                if (odd.first > _distance(w, p))
                {
                    // we delete the farthest neighbour
                    neighbours.erase(odd_it);
                    // we insert the new neighbour and get its position in p_it
                    auto p_it = neighbours.emplace(make_pair(_distance(w, p), p));

                    // We add the edges which contain p
                    ///////////////////////////////////
                    for (auto point = neighbours.begin(); point != neighbours.end(); ++point)
                    {
                        if (point != p_it)
                        {
                            w->edges.emplace(Edge(point->second, p));
                        }
                    }

                    // We delete the edges which contain the point which has been removed from
                    // the nu_1 nearest neighbours
                    //////////////////////////////////////////////////////////////////////////
                    for (auto e = w->edges.begin(); e != w->edges.end();)
                    {
                        if (e->index1 == odd.second->index
                            || e->index2 == odd.second->index)
                        {
                            e = w->edges.erase(e);
                        }
                        else
                        {
                            ++e;
                        }
                    }
                }
                else
                {
                    // w is not affect by p
                }
            }
        }

        // We only want the reconstruction for i = 2000 (then we stop)
        if (i == 400)
        {
            for (Point* w : W)
            {
                for (auto& e : w->edges)
                {
                    if (CWL.insert(e).second)
                        ++faces;
                }
            }

            // Geomview OFF format
            // Syntax : "OFF"
            ofile << "OFF" << endl;

            // Syntax : "number of vertices" "number of faces" "number of edges" (number of edges is
            // not read so we can put whatever we want)
            ofile << i + 1 << " " << faces + i + 1 << " " << 42 << endl;

            // Syntax : x y z for each vertex
            for (Point* p : L)
            {
                ofile << p->coordinates.x << " " << p->coordinates.y << " " << "0.000000" << endl;
            }

            // Syntax : n i j k ... for each n-face which indexes are i j k...
            // points
            for (Point* p : L)
            {
                ofile << 1 << " " << L_inv[p->index] << " 255 0 0" << endl;
            }

            // edges
            for (const Edge& e : CWL)
            {
                ofile << 2 << " " << L_inv[e.index1] << " " << L_inv[e.index2] << " 0 0 0" << endl;
            }

            // We stop
            break;
        }
        i++;
    }
}
