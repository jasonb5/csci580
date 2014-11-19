#ifndef PROJECT3_ANN_H_
#define PROJECT3_ANN_H_

#include <stdio.h>
#include <vector>
#include <map>

using std::vector;
using std::map;

#define error(M, ...) fprintf(stderr, "%s:%d: " M "\n", ##__VA_ARGS__)

#ifndef DEBUG
#define debug(M, ...) fprintf(stdout, M, ##__VA_ARGS__)
#else 
#define debug(M, ...)
#endif

#define msg(M, ...) fprintf(stdout, M, ##__VA_ARGS__)

class ANN {
 public:
	void AddLayer(int nodes);
	void AddWeight(int layer, int node, double weight);
  int Layers();
  int NodesInLayer(int layer);
	void TrainNetwork(vector<double> input, vector<double> expected);
	void BackPropagation(vector<double> output, vector<double> expected);
	void TestData(vector<double> intput, vector<double> &output);
	void ClassifyData(vector<vector<double> > data, map<int, vector<double> > class_map);
	void PrintNetwork();

 private:
	struct Neuron {
		double delta;
		double value;
		vector<double> weights;		
	};

	vector<vector<Neuron*> > layers_;	
};

#endif // PROJECT3_ANN_H_
