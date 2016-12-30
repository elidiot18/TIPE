
#include "reconstruction.h"
#include "miscellaneous.h"

using namespace std;

void reconstruction(vector<Point*>& W, ofstream& ofile)
{
    /***************
     * nu sequence *
     ***************/

    size_t nu_0 = 1, nu_1 = 6, nu_2 = 6;

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

    // List of the reverse-landmarks of each point of L
    // reverse_landmarks[p] contains the points w in W such as p is in neighbours[w]
    vector<vector<Point*>> reverse_landmarks(n, { L[0] });


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
        cout << i << endl;
        // We add to L the point of W\L which is the farthest from L
        Point* p = farthest(W);
        L.push_back(p);
        L_inv[p->index] = i;

        /* update of the neighbourhoods and reverse_landmarks */
        for (Point* w : W)
        {
            auto& neighbours = w->neighbourhood;
            auto& simplices = w->simplices;

            // If neighbours is not full
            ////////////////////////////
            if (i <= nu_2 - 1)
            {
                reverse_landmarks[p->index].push_back(w);

                auto p_it = neighbours.emplace(make_pair(_distance(w, p), p));

                /* Triangles */
                // We put as many triangles as possible
                ///////////////////////////////////////
                for (auto p1 = neighbours.begin(); p1 != neighbours.end(); ++p1)
                {
                    for (auto p2 = next(p1); p2 != neighbours.end(); ++p2)
                    {
                        if (p1->second != p && p2->second != p)
                            simplices.triangles.emplace(Triangle(p1->second, p2->second, p));
                    }
                }

                /* Edges */
                // If nu_1 == nu_2 we put as many edges as possible
                ///////////////////////////////////////////////////
                if (i <= nu_1 - 1 || nu_1 == nu_2)
                {
                    for (auto point = neighbours.begin(); point != neighbours.end(); ++point)
                    {
                        if (point != p_it)
                        {
                            simplices.edges.emplace(Edge(point->second, p));
                        }
                    }
                }
                // Else, we have to check if w's edges are affected
                ///////////////////////////////////////////////////
                else
                {
                    auto& e_max = *next(neighbours.begin(), nu_1);
                    if (p_it->first < e_max.first)
                    {
                        // We add the edges which contain p
                        ///////////////////////////////////
                        for (auto point = neighbours.begin();
                             point->second->index != e_max.second->index; ++point)
                        {
                            if (point != p_it)
                            {
                                simplices.edges.emplace(Edge(point->second, p));
                            }
                        }
                        // We delete the edges which contain the point which has been removed from
                        // the nu_1 first neighbours
                        ////////////////////////////////////////////////////////////////////////////////////////////////////
                        for (auto e = simplices.edges.begin(); e != simplices.edges.end();)
                        {
                            if (e->index1 == e_max.second->index
                                || e->index2 == e_max.second->index)
                            {
                                e = simplices.edges.erase(e);
                            }
                            else
                            {
                                ++e;
                            }
                        }
                    }
                    else
                    {
                        // p only affected w's triangles and not its edges
                    }
                }
            }
            else
            {
                auto t_max_it = --neighbours.end();
                auto t_max = *t_max_it;

                // If w's triangles are affected
                ////////////////////////////////
                if (t_max.first > _distance(w, p))
                {
                    // w is not a reverse landmark of odd any longer
                    del(w, &(reverse_landmarks[t_max.second->index]));
                    // but now, w is a reverse landmark of p
                    reverse_landmarks[p->index].push_back(w);

                    neighbours.erase(t_max_it);
                    // we insert the new neighbour and get its position in p_it
                    auto p_it = neighbours.emplace(make_pair(_distance(w, p), p));

                    /* Triangles */

                    // We remove the triangles which contain the points which has been removed
                    //////////////////////////////////////////////////////////////////////////
                    for (auto t = simplices.triangles.begin(); t != simplices.triangles.end();)
                    {
                        if (t->index1 == t_max.second->index || t->index2 == t_max.second->index
                            || t->index3 == t_max.second->index)
                        {
                            t = simplices.triangles.erase(t);
                        }
                        else
                        {
                            ++t;
                        }
                    }

                    // We add the triangles which contain p
                    ///////////////////////////////////////
                    for (auto p1 = neighbours.begin(); p1 != neighbours.end(); ++p1)
                    {
                        if (p1 != p_it)
                        {
                            for (auto p2 = next(p1); p2 != neighbours.end(); ++p2)
                            {
                                if (p2 != p_it)
                                {
                                    simplices.triangles.emplace(
                                        Triangle(p1->second, p2->second, p));
                                }
                            }
                        }
                    }

                    /* Edges */

                    // if nu_1 == nu_2 then the points which has been removed from the nu_1 nearest
                    // neighbours IS the one which has been removed;
                    // else, this point is the first next to the nu_1 nearest neighbours
                    auto& e_max = (nu_1 == nu_2) ? t_max : *next(neighbours.begin(), nu_1);

                    // If w's edges are affected
                    ////////////////////////////
                    if (p_it->first < e_max.first)
                    {
                        // We add the edges which contain p
                        ///////////////////////////////////
                        for (auto point = neighbours.begin(); point != neighbours.end()
                             && point->second->index != e_max.second->index;
                             ++point)
                        {
                            if (point != p_it)
                            {
                                simplices.edges.emplace(Edge(point->second, p));
                            }
                        }

                        // We delete the edges which contain the point which has been removed from
                        // the nu_1 nearest neighbours
                        //////////////////////////////////////////////////////////////////////////
                        for (auto e = simplices.edges.begin(); e != simplices.edges.end();)
                        {
                            if (e->index1 == e_max.second->index
                                || e->index2 == e_max.second->index)
                            {
                                e = simplices.edges.erase(e);
                            }
                            else
                            {
                                ++e;
                            }
                        }
                    }
                    else
                    {
                        // p only affected w's triangles and not its edges
                    }
                }
                else
                {
                    // w is not affect by p
                }
            }
        }

        auto& p_reverse_landmarks = reverse_landmarks[p->index];
        for (Point* w : p_reverse_landmarks)
        {
            for (auto t = w->simplices.triangles.begin(); t != w->simplices.triangles.end();)
            {
                // we want to know if we have to add the triangle [p1, p2, p3]
                Point* p1 = W[t->index1];
                Point* p2 = W[t->index2];
                Point* p3 = W[t->index3];

                auto& p1_potential_witnesses = reverse_landmarks[p1->index];
                auto& p2_potential_witnesses = reverse_landmarks[p2->index];
                // auto& p3_potential_witnesses = reverse_landmarks[p3->index];

                Edge e1 = Edge(p1, p2);
                Edge e2 = Edge(p1, p3);
                Edge e3 = Edge(p2, p3);

                bool byebye_triangle = true;

                // checking if the edge e1 is witnessed
                for (const auto& rev_land : p1_potential_witnesses)
                {
                    if (rev_land->simplices.edges.find(e1) != rev_land->simplices.edges.end())
                    {
                        byebye_triangle = false;
                        break;
                    }
                }
                /*if (byebye_triangle) {
                    for (const auto& rev_land : p2_potential_witnesses) {
                        if (rev_land->simplices.edges.find(e1) != rev_land->simplices.edges.end()) {
                            byebye_triangle = false;
                            break;
                        }
                    }
                }*/

                // checking if the edge e2 is witnessed
                if (byebye_triangle)
                {
                    for (const auto& rev_land : p1_potential_witnesses)
                    {
                        if (rev_land->simplices.edges.find(e2) != rev_land->simplices.edges.end())
                        {
                            byebye_triangle = false;
                            break;
                        }
                    }
                }
                /*if (byebye_triangle) {
                    for (const auto& rev_land : p3_potential_witnesses) {
                        if (rev_land->simplices.edges.find(e2) != rev_land->simplices.edges.end()) {
                            byebye_triangle = false;
                            break;
                        }
                    }
                }*/

                // checking if the edge e3 is witnessed
                if (byebye_triangle)
                {
                    for (const auto& rev_land : p2_potential_witnesses)
                    {
                        if (rev_land->simplices.edges.find(e3) != rev_land->simplices.edges.end())
                        {
                            byebye_triangle = false;
                            break;
                        }
                    }
                }
                /*if (byebye_triangle) {
                    for (const auto& rev_land : p3_potential_witnesses) {
                        if (rev_land->simplices.edges.find(e3) != rev_land->simplices.edges.end()) {
                            byebye_triangle = false;
                            break;
                        }
                    }
                }*/

                // if every edge is witnessed then the triangle is witnessed (it is itself witnessed
                // by definition)
                // for the moment, t is in w.simplices
                // do we have to remove it ?
                if (byebye_triangle)
                {
                    t = w->simplices.triangles.erase(t);
                }
                else
                {
                    ++t;
                }
            }
        }

        if (i == 15000)
        {
            for (Point* w : W)
            {
                for (auto& e : w->simplices.edges)
                {
                    //if (CWL.edges.insert(e).second)
                        //++faces;
                }
                for (auto& t : w->simplices.triangles)
                {
                    if (CWL.triangles.insert(t).second)
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
                ofile << p->coordinates.x << " " << p->coordinates.y << " " << p->coordinates.z
                      << endl;
            }

            // Syntax : n i j k ... for each n-face which indexes are i j k...
            // points
            for (Point* p : L)
            {
                ofile << 1 << " " << L_inv[p->index] << " 255 0 0" << endl;
            }

            // edges
            for (const Edge& e : CWL.edges)
            {
                ofile << 2 << " " << L_inv[e.index1] << " " << L_inv[e.index2] << " 0 0 0" << endl;
            }

            // faces
            for (const Triangle& t : CWL.triangles)
            {
                ofile << 3 << " " << L_inv[t.index1] << " " << L_inv[t.index2] << " "
                      << L_inv[t.index3] << endl;
            }

            // We stop
            break;
        }
        i++;
    }
}

void del(Point* w, vector<Point*>* subset)
{
    subset->erase(remove(subset->begin(), subset->end(), w), subset->end());
}
