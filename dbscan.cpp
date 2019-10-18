//run --> /dbscan.exe inputfile.txt n eps minpts

#include "pch.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <string>
#include <map>
#include <fstream>
#include "dbscan.h"

using namespace std;

struct Point {
	Point(int xx = 0, int yy = 0) : x(xx), y(yy) {}
	float x;
	float y;
	int counter, cluster_matrix;
	friend std::ostream & operator<<(std::ostream &os, const Point& p) {
		return os << "(" << p.x << ";" << p.y << ")";
	}

	int distance(Point a, Point b) {
		return std::sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));

	}
};

const int UNCLASSIFIED = 0;
const int NOISE = NULL;



class Cluster {
public:
	vector <Point> data;
	vector <vector<int> > cluster_matrix;
	vector <vector<int> > core_points;
	int length, cluster_id, n, Eps, MinPts;

	Cluster(vector<Point> data, int n, int Eps, int MinPts) {
		this->cluster_id = -1;
		this->data = data;
		this->length = (int)data.size();
		core_points.resize(length);
		this->n = n;
		this->Eps = Eps;
		this->MinPts = MinPts;

	}

	void FindNeighbors() {

		for (int i = 0; i < length; i++) {
			for (int j = 0; j < length; j++) {
				if (i == j) continue;
				Point P;
				if (P.distance(data[i], data[j]) < Eps) {
					data[i].counter++;
					core_points[i].push_back(j);
				}

			}
		}
	}

	bool CorePoint(int point_id) {
		return data[point_id].counter > MinPts;
	}

	void ExpandCluster(int starting_point, int clustered) {

		for (auto&next : core_points[starting_point]) {
			if (data[next].cluster_matrix != UNCLASSIFIED) continue;
			ExpandCluster(next, clustered);
		}

		data[starting_point].cluster_matrix = clustered;
		if (!CorePoint(starting_point)) return;
	}

		vector<vector<int> > startCluster() {
			return cluster_matrix;
		}


		void dbscan() {

			FindNeighbors();

			for (int i = 0; i < length; i++) {
				if (data[i].cluster_matrix != UNCLASSIFIED) continue;
				if (CorePoint(i)) {
					ExpandCluster(i, ++cluster_id);
				}
				else {
					data[i].cluster_matrix = NOISE;
				}
			}

			cluster_matrix.resize(cluster_id + 1);
			for (int i = 0; i < length; i++) {
				if (data[i].cluster_matrix != NOISE) {
					cluster_matrix[data[i].cluster_matrix].push_back(i);
				}
			}

		}
};

class ReadFile {

private:
	ifstream fin;
	vector<Point> data;
public:
	ReadFile(string filename) {
		fin.open(filename);
		ParseObject();
	}

	void ParseObject() {
		int point_id;
		float x, y;
		while (!fin.eof()) {
			fin >> point_id >> x >> y;
			data.push_back(x);
			data.push_back(y);
			data.push_back(UNCLASSIFIED);
		}

		data.pop_back();
	}

	vector<Point> SelectPoints() {
		return data;
	}

};


class ShowResults {

public:
	ofstream fout;
	vector<vector<int> > cluster_matrix;
	string filename;
	int n;

	ShowResults(vector<vector<int> > cluster_matrix, string filename, int n) {
		this->cluster_matrix = cluster_matrix;
		this->n = n;

		if (filename.size() < 4) {
			cout << filename << "input error";
			exit(0);
		}

		for (int i = 0; i < 4; i++) filename.pop_back();

		this->filename = filename;

		std::sort(cluster_matrix.begin(), cluster_matrix.end(), [](const vector<int> a, const vector<int> b) {
			return (int)a.size() > (int)b.size();
		});

		
	}

	void results(){
			for (int i = 0; i < n; i++) {

				fout.open(filename + "_cluster_" + to_string(i) + ".txt");

				for (int j = 0; j < cluster_matrix[i].size(); j++) {
					fout << cluster_matrix[i][j] << endl;
				}

				fout.close();

			}
		}
};


int main(int argc, const char * argv[]) {
	if (argc != 5) {
		cout << " Fatal: You forgot to include the directory name on the command line.";
		return 0;
	}

	string filename(argv[1]);
	string n(argv[2]);
	string Eps(argv[3]);
	string MinPts(argv[4]);

	ReadFile ReadFile(filename);
	Cluster Cluster(ReadFile.SelectPoints(), stoi(n), stoi(Eps), stoi(MinPts));
	Cluster.dbscan();
	ShowResults ShowResults(Cluster.startCluster(), filename, stoi(n));
	ShowResults.results();

	return 0;



}

