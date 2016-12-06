// Imports
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std

inline string remove_extension(const string &s) {
	size_t lastdot = s.find_last_of('.');
    if (lastdot == string::npos) return s;
    return s.substr(0, lastdot);
}

int distance(const vector<int> &p, const vector<int*> &L, const vector< vector<int> > &points) {
	size_t l = L.size();
	int d = (p[0] - !(L[0])) * (p[0] - !(L[0])) + (p[1] - !(L[1])) * (p[1] - !(L[1]));
	for (int k = 0; i < l; ++k) {
		int xk = !(L[k])[0], yk = !(L[k])[1];
		int dk = (p[0] - xk) * (p[0] - xk) + (p[1] - yk) * (p[1] - yk);
		if (dk < d) d = dk;
	}
	return d;
}

size_t farthest(const vector< vector<int> > &points, const vector<int*> &L) {
	size_t p = points.size()
	size_t l = L.size()
	size_t m = 0;
	int dm = distance(points[m], L, points);
	for (size_t j = 1; i < p; ++j) {
		int dj = distance(points[j], L, points);
		if (dm < dj) {
			m = j;
		}
	}
	return m;
}

void update_neighboors(const vector< vector<int> > &points, const vector< vector<int*> > neighboors, const vector<int*> &L) {
	n = points.size();
	for (int i = 0; i < n; ++i) {
		
	}
}

void reconstruct(const vector< vector<int> > &points, ofstream &ofile) {
	size_t n = points.size();
	vector< vector<int*> > neighboors(n, vector<int*>(2, NULL));
	vector<int*> L;
	L.push_back(&points);
	size_t i = 1;
	while (i <= n) {
		ofstream << "Reconstruction n. " << i << " : " << endl;
		size_t j = farthest(points, L);
		update_neighboors(points, neighboors, L);

		L.push_back(points[i])
	}
}

void main(int argc, char *argv[]) {
	if (argc <= 1) {
		cout << "You need to specify a file";
	}
	if (argc > 2) {
		cout << "There is only one argument to be specified";
	}
	ifstream ifile(argv[1], ios::in);

	string name = argv;
	name = remove_extension(name);
	ofstream ofile(name.c_str() + ".cplx", ios::out | ios::trunc);

	string dim;
	getline(ifile, dim)
	if (dim != "2") {
		cout << "This algorithm does only work for dimension 2"
	}

	vector< vector<int> > points;
	size_t i = 0;
	while (!ifile.eof()) {
		points.push_back(vector<int>(2, 0));
		ifile >> points[i][0];
		ifile >> points[i][1];
		i++;
	}
	reconstruct(points, ofile);

}