#ifndef PROJECT3_ANN_H_
#define PROJECT3_ANN_H_

#include <stdio.h>
#include <vector>
#include <map>

using std::vector;
using std::map;

#define error(M, ...) fprintf(stderr, "%s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#ifdef DEBUG
#define debug(M, ...) fprintf(stdout, M, ##__VA_ARGS__)
#else 
#define debug(M, ...)
#endif

#define msg(M, ...) fprintf(stdout, M, ##__VA_ARGS__)

struct Neuron {
  double delta;
  double value;
  vector<double> weights;		
};

class ANN {
 public:
	void AddLayer(int nodes);
	void AddWeight(int layer, int node, double weight);
  int Layers();
  int NodesInLayer(int layer);
  Neuron NeuronAt(int layer, int node); 
	void TrainNetwork(vector<double> input, vector<double> expected);
	void BackPropagation(vector<double> output, vector<double> expected);
	void TestData(vector<double> intput, vector<double> &output);
	void ClassifyData(vector<vector<double> > data, map<int, vector<double> > class_map, vector<int> &results);
	void PrintNetwork();

 private:
	vector<vector<Neuron*> > layers_;	
};

#endif // PROJECT3_ANN_H_
