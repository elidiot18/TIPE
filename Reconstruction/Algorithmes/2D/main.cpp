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

int distance(vector<int> *p1, vector<int> *p2) {
	return ((*p1)[0] - (*p2)[0]) * ((*p1)[0] - (*p2)[0]) + ((*p1)[1] - (*p2)[1]) * ((*p1)[1] - (*p2)[1]);
}

int distance(vector<int> *p, vector< vector<int>* > &L) {
	size_t l = L.size();
	int d = distance(p, L[0]);
	for (int k = 0; k < l; ++k) {
		int dk = distance(p, L[k]);
		if (dk < d) d = dk;
	}
	return d;
}

int farthest(vector< vector<int> > &points, vector< vector<int>* > &L) {
	size_t p = points.size();
	size_t l = L.size();
	int m = 0;
	int dm = distance(&(points[m]), L);
	for (size_t j = 1; j < p; ++j) {
		int dj = distance(&(points[j]), L);
		if (dm < dj) {
			m = j;
		}
	}
	return m;
}

void update_neighbours(vector< vector<int> > &points, vector< vector< vector<int>* > > neighbours, vector< vector<int>* > &L, vector<int> *last) {
	size_t n = points.size();
	for (int i = 0; i < n; ++i) {
		vector<int> **nearest1 = &(neighbours[i][0]);
		vector<int> **nearest2 = &(neighbours[i][1]);
		int d1 = distance(&(points[i]), *nearest1);
		int d2 = distance(&(points[i]), *nearest2);
		vector<int> **farthest = (d1 > d2 ? nearest1 : nearest2);
		int d = distance(&(points[i]), *farthest);
		if (d > distance(&(points[i]), last)) {
			*farthest = last;
		}
	}
}

void reconstruction(vector< vector<int> > &points, ofstream &ofile) {
	size_t n = points.size();
	vector< vector<int>* > L;
	L.push_back(&(points[0]));
	vector< vector< vector<int>* > > neighbours(n, (vector< vector<int>* >){2, L[0]});
	vector< vector< vector<int>* > > CWL;
	vector<int> *first = L[0];
	size_t i = 1;
	while (i < n) {
		int j = farthest(points, L);
		vector<int> *cur = &(points[j]);
		L.push_back(cur);
		update_neighbours(points, neighbours, L, cur);

		if (i == 1) {
			CWL.push_back((vector< vector<int>* >){L[0], cur});
			continue;
		}
		else if (i == 2) {
			CWL.push_back((vector< vector<int>* >){L[0], cur});
			CWL.push_back((vector< vector<int>* >){L[1], cur});
			continue;
		}

		ofile << "Reconstruction n. " << i << " : " << endl;
		vector<int> *nearest1 = neighbours[j][0];
		vector<int> *nearest2 = neighbours[j][1];

		CWL.push_back(vector< vector<int>* > {nearest1, cur});
		CWL.push_back(vector< vector<int>* > {nearest2, cur});
		size_t m = CWL.size();
		for (size_t k = 0; k < m; ++k) {
			if ((CWL[k][0] == nearest1 or CWL[k][0] == nearest2) and (CWL[k][1] == nearest1 or CWL[k][1] == nearest2)) {
				CWL[k] = vector< vector<int>* > {NULL, NULL};
			}
			else if (CWL[k][0] != NULL) {
				ofile << (*(CWL[k][0]))[0] << " , " << (*(CWL[k][0]))[1]
					  << " ; "
					  << (*(CWL[k][1]))[0] << " , " << (*(CWL[k][1]))[1] << endl;
			}
		}
	}
}

int main(int argc, char **argv) {
	if (argc <= 1) {
		cout << "You need to specify a file";
	}
	if (argc > 2) {
		cout << "There is only one argument to be specified";
	}
	ifstream ifile(argv[1], ios::in);

	string name = argv[1];
	name = remove_extension(name);
	name = name + (string)".cplx";
	ofstream ofile(name.c_str(), ios::out | ios::trunc);

	string dim;
	getline(ifile, dim);
	if (dim != "2") {
		cout << "This algorithm does only work for dimension 2";
	}

	vector< vector<int> > points;
	size_t i = 0;
	while (!ifile.eof()) {
        vector<int> point(2, 0);
		points.push_back(point);
		int x, y;
		ifile >> x;
		ifile >> y;
        points[i][0] = x;
        points[i][1] = y;
		i++;
	}
	reconstruction(points, ofile);
	return 0;
}
