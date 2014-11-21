#include "ann.h"

#include <math.h>

// Default constructor
ANN::ANN(double bias)
	:	bias_(bias) {

}

// Adds layer with nodes
void ANN::AddLayer(int nodes) {
	Neuron *n;
	vector<Neuron*> layer;

	for (int i = 0; i < nodes; ++i) {
		n = new Neuron();

		layer.push_back(n);	
	}

	layers_.push_back(layer);
}

// Adds input weight for neuron
void ANN::AddWeight(int layer, int node, double weight) {
	layers_[layer][node]->weights.push_back(weight);
}

// Returns layers
int ANN::Layers() {
  return layers_.size();
}

// Returns nodes in layer
int ANN::NodesInLayer(int layer) {
  return layers_[layer].size();
}

// Returns neuron in layer
Neuron ANN::NeuronAt(int layer, int node) {
  return *layers_[layer][node];
}

// Trains network by pushing input through and referencing
// expected to calculate the error used for backpropagation
void ANN::TrainNetwork(vector<double> input, vector<double> expected) {
	vector<double> output;

	TestData(input, output);

	BackPropagation(output, expected);
}

// Performs backpropagation using the output of the network
// and the corresponding expected results
void ANN::BackPropagation(vector<double> output, vector<double> expected) {	
	int x, y, z;
	int max = layers_.size()-1;

	// Calculate error for output layer
	for (x = 0; x < layers_[max].size(); ++x) {
		double value = layers_[max][x]->value;

		layers_[max][x]->error = value * (1 - value) * (expected[x] - value);
	}

	// Iterate over all layers except the output in reverse
	for (x = max-1; x >= 0; --x) {
		// Iterates over each neuron in the layer
		for (y = 0; y < layers_[x].size(); ++y) {
			double error_weights = 0.0;
			double value = layers_[x][y]->value;

			// Sum the weighted errors of all links
			for (z = 0; z < layers_[x+1].size(); ++z) {
				error_weights += layers_[x+1][z]->error * layers_[x+1][z]->weights[y+1];
			}		

			// Calculate out new error
			layers_[x][y]->error = value * (1-value) * error_weights;
		}
	}

	// Update weights for all nodes except input
  for (x = 1; x < layers_.size(); ++x) {
    for (y = 0; y < layers_[x].size(); ++y) {
      for (z = 0; z < layers_[x][y]->weights.size(); ++z) {
				// Add the new adjusted weights to the existing
        layers_[x][y]->weights[z] += bias_ * ((z == 0) ? 1.0 : layers_[x-1][z-1]->value) * layers_[x][y]->error;
      }
    }  
  }
}

// Pushes the input through the network and copies the results to output
void ANN::TestData(vector<double> input, vector<double> &output) {
	int x, y, z;

  // Set input values
	for (x = 0; x < layers_[0].size(); ++x) {
		layers_[0][x]->value = input[x];
	}

  // Iterate all layers except the input
	for (x = 1; x < layers_.size(); ++x) {
    // Iterate nodes in current layer
		for (y = 0; y < layers_[x].size(); ++y) {
			layers_[x][y]->value = 0.0;
		
      // Sum all weights multiplied by their connecting nodes value
			for (z = 0; z < layers_[x][y]->weights.size(); ++z) {
				if (z == 0) {
					layers_[x][y]->value += layers_[x][y]->weights[z];
				} else {
					layers_[x][y]->value += layers_[x][y]->weights[z] * layers_[x-1][z-1]->value;
				}			
			}
 
      // Set current nodes value using activation function 1/1+exp(-inj)
			layers_[x][y]->value = 1/(1+exp(-layers_[x][y]->value));
		}
	}	

  // Copy output layer to return vector
	for (x = 0; x < layers_[layers_.size()-1].size(); ++x) {
		output.push_back(layers_[layers_.size()-1][x]->value);	
	}
}

// Prints the current state of the network
void ANN::PrintNetwork() {
	for (int x = 0; x < layers_.size(); ++x) {
		debug("Layer %d\n", x);
		for (int y = 0; y < layers_[x].size(); ++y) {
			debug("\tNode %d value %f\n", y, layers_[x][y]->value);
			for (int z = 0; z < layers_[x][y]->weights.size(); ++z) {
				debug("\t\tWeight %f\n", layers_[x][y]->weights[z]);
			}
		}
	}
}
