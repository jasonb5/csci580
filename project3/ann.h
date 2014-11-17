#ifndef PROJECT3_ANN_H_
#define PROJECT3_ANN_H_

#include <stdio.h>
#include <vector>

using std::vector;

#define debug(M, ...) fprintf(stdout, "%s:%d:" M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

class ANN {
 public:
	void AddLayer(int nodes);
	void AddWeight(int layer, int node, double weight);
	void TrainNetwork(vector<double> input, vector<double> expected);
	void BackPropagation(vector<double> output, vector<double> expected);
	void TestData(vector<double> intput, vector<double> &output);
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
