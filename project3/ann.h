#ifndef PROJECT3_ANN_H_
#define PROJECT3_ANN_H_

#include <stdio.h>
#include <vector>
#include <map>

using std::vector;
using std::map;

// Helper macro prints message denoting current file and line number
#define error(M, ...) fprintf(stderr, "%s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

// Enables debug macro if DEBUG is defined
#ifdef DEBUG
#define debug(M, ...) fprintf(stdout, M, ##__VA_ARGS__)
#else 
#define debug(M, ...)
#endif

// Helper macro prints a message
#define msg(M, ...) fprintf(stdout, M, ##__VA_ARGS__)

// Struct represents neuron for a Neural Network
// Stores error, current value and the weights
// that are coming into the node
struct Neuron {
  double error;
  double value;
  vector<double> weights;		
};

// See comment at top of file for a complete description
class ANN {
 public:
	// Constructor
	// Initializes network with given alpha value
	ANN(double alpha);	
	// Adds a layer containg neurons equal to nodes
	void AddLayer(int nodes);
	// Adds a weight to a neuron node in a layer of the network
	void AddWeight(int layer, int node, double weight);
	// Returns the number of layers
  int Layers();
	// Returns the number of neurons in layer
  int NodesInLayer(int layer);
	// Returns the neuron described by node and layer
  Neuron NeuronAt(int layer, int node); 
	// Trains a network using input and expected.
	// The values in input will be fed through the network,
	// where the results will be used to calulate the error
	// and back propagation will be performed.
	void TrainNetwork(vector<double> input, vector<double> expected);
	// The network learns through back propagation. The 
	// error is calculated using output and expected. This
	// error is fed through the network backwards updating
	// the weights between neurons.
	void BackPropagation(vector<double> output, vector<double> expected);
	// Feeds the input vector through the network and 
	// copys the results into the output vector.
	void TestData(vector<double> input, vector<double> &output);
	// Prints the current state of the network
	void PrintNetwork();

 private:
	// Bias value used in dummy weights and
	// error calculations
	double bias_; 
	// Stores the network structure
	vector<vector<Neuron*> > layers_;	
};

#endif // PROJECT3_ANN_H_
