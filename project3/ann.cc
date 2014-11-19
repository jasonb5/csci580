#include "ann.h"

#include <math.h>
#include <limits>

using std::numeric_limits;

void ANN::AddLayer(int nodes) {
	Neuron *n;
	vector<Neuron*> layer;

	for (int i = 0; i < nodes; ++i) {
		n = new Neuron();

		layer.push_back(n);	
	}

	layers_.push_back(layer);
}

void ANN::AddWeight(int layer, int node, double weight) {
	layers_[layer][node]->weights.push_back(weight);
}

int ANN::Layers() {
  return layers_.size();
}

int ANN::NodesInLayer(int layer) {
  return layers_[layer].size();
}

Neuron ANN::NeuronAt(int layer, int node) {
  return *layers_[layer][node];
}

void ANN::TrainNetwork(vector<double> input, vector<double> expected) {
	vector<double> output;

	TestData(input, output);

	BackPropagation(output, expected);
}

void ANN::BackPropagation(vector<double> output, vector<double> expected) {	
	int x, y, z;
	int max = layers_.size()-1;

  debug("\nRunning Back Propagation\n");

	for (x = 0; x < layers_[max].size(); ++x) {
		double value = layers_[max][x]->value;

		layers_[max][x]->delta = value * (1 - value) * (expected[x] - value);

    debug("\tCalculating error for layer %d node %d\n", max, x);    
    debug("\t%f * (1 - %f) * (%f - %f) = %f\n", value, value, expected[x], value, layers_[max][x]->delta);
	}

	for (x = max-1; x >= 0; --x) {
		for (y = 0; y < layers_[x].size(); ++y) {
			double error_weights = 0.0;
			double value = layers_[x][y]->value;

      debug("\tCalculating error for layer %d node %d\n\t", x, y);
			for (z = 0; z < layers_[x+1].size(); ++z) {
				error_weights += layers_[x+1][z]->delta * layers_[x+1][z]->weights[y+1];
        debug("%f * %f + ", layers_[x+1][z]->delta, layers_[x+1][z]->weights[y+1]);
			}		

			layers_[x][y]->delta = value * (1-value) * error_weights;
      debug("\n\t%f * (1 - %f) * %f = %f\n", value, value, error_weights, layers_[x][y]->delta);
		}
	}

  for (x = 1; x < layers_.size(); ++x) {
    for (y = 0; y < layers_[x].size(); ++y) {
      debug("Adjusting weight for node %d in layer %d\n", x, y);
      for (z = 0; z < layers_[x][y]->weights.size(); ++z) {
        debug("\t%f + ", layers_[x][y]->weights[z]);
        layers_[x][y]->weights[z] += 0.01 * ((z == 0) ? 1.0 : layers_[x-1][z-1]->value) * layers_[x][y]->delta;
        debug("%f * %f * %f = %f\n", 0.01, (z == 0) ? 1.0 : layers_[x-1][z-1]->value, layers_[x][y]->delta, layers_[x][y]->weights[z]);
      }
    }  
  }
}

void ANN::TestData(vector<double> input, vector<double> &output) {
	int x, y, z;

  debug("Testing values ");

  // Set input values
	for (x = 0; x < layers_[0].size(); ++x) {
		layers_[0][x]->value = input[x];
    debug("%f ", layers_[0][x]->value);
	}
  debug("\n");
  // Iterate all layers except the input
	for (x = 1; x < layers_.size(); ++x) {
    // Iterate nodes in current layer
		for (y = 0; y < layers_[x].size(); ++y) {
			layers_[x][y]->value = 0.0;

      debug("Layer %d node %d\n", x, y);
		
      // Sum all weights multiplied by their connecting nodes value
			for (z = 0; z < layers_[x][y]->weights.size(); ++z) {
				if (z == 0) {
          debug("\t%f", layers_[x][y]->weights[z]);
					layers_[x][y]->value += layers_[x][y]->weights[z];
				} else {
          debug(" + %f * %f", layers_[x][y]->weights[z], layers_[x-1][z-1]->value);
					layers_[x][y]->value += layers_[x][y]->weights[z] * layers_[x-1][z-1]->value;
				}			
			}

      debug("\n\t sum %f activation %f\n", layers_[x][y]->value, 1/(1+exp(-layers_[x][y]->value)));     
 
      // Set current nodes value using activation function 1/1+exp(-inj)
			layers_[x][y]->value = 1/(1+exp(-layers_[x][y]->value));
		}
	}	

  // Copy output layer to return vector
	for (x = 0; x < layers_[layers_.size()-1].size(); ++x) {
		output.push_back(layers_[layers_.size()-1][x]->value);	
	}
}

void ANN::ClassifyData(vector<vector<double> > data, map<int, vector<double> > class_map, vector<int> &results) {
	int x, y, z, classified;
	double minimum;
	map<int, vector<double> >::iterator it;

	for (x = 0; x < data.size(); ++x) {
    minimum = numeric_limits<double>::max();

    for (it = class_map.begin(); it != class_map.end(); ++it) {
			double value = 0.0;

			for (y = 0; y < it->second.size(); ++y) {
				value += (double)pow(data[x][y] - it->second[y], 2);
			}

			value = sqrt(value);

			if (value < minimum) {
				minimum = value;
			
				classified = it->first;
			}
		}

    results.push_back(minimum);
	}	
}

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
