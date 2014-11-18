#include "ann.h"

#include <math.h>

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

void ANN::TrainNetwork(vector<double> input, vector<double> expected, int iterations) {
  int i;
	vector<double> output;

  for (i = 0; i < iterations; ++i) {
    output.clear(); 

    TestData(input, output);

    BackPropagation(output, expected);
  }
}

void ANN::BackPropagation(vector<double> output, vector<double> expected) {	
	int x, y, z;
	int max = layers_.size()-1;

	for (x = 0; x < layers_[max].size(); ++x) {
		double value = layers_[max][x]->value;

		layers_[max][x]->delta = value * (1 - value) * (expected[x] - value);
	}

	for (x = max-1; x >= 0; --x) {
		for (y = 0; y < layers_[x].size(); ++y) {
			double error_weights = 0;
			double value = layers_[x][y]->value;

			for (z = 0; z < layers_[x+1].size(); ++z) {
				error_weights += layers_[x+1][z]->delta * layers_[x+1][z]->weights[y+1];
			}		

			layers_[x][y]->delta = value * (1-value) * error_weights;
		}
	}

  for (x = 1; x < layers_.size(); ++x) {
    for (y = 0; y < layers_[x].size(); ++y) {
      for (z = 0; z < layers_[x][y]->weights.size(); ++z) {
        layers_[x][y]->weights[z] += 0.01 * ((z == 0) ? 1.0 : layers_[x][y]->value) * layers_[x][y]->delta;
      }
    }  
  }
}

void ANN::TestData(vector<double> input, vector<double> &output) {
	int x, y, z;

	for (x = 0; x < layers_[0].size(); ++x) {
		layers_[0][x]->value = input[x];
	}

	for (x = 1; x < layers_.size(); ++x) {
		for (y = 0; y < layers_[x].size(); ++y) {
			layers_[x][y]->value = 0.0;
		
			for (z = 0; z < layers_[x][y]->weights.size(); ++z) {
				if (z == 0) {
					layers_[x][y]->value += layers_[x][y]->weights[z];
				} else {
					layers_[x][y]->value += layers_[x][y]->weights[z] * layers_[x-1][z-1]->value;
				}			
			}
			layers_[x][y]->value = 1/(1+exp(-layers_[x][y]->value));
		}
	}	

	for (x = 0; x < layers_[layers_.size()-1].size(); ++x) {
		output.push_back(layers_[layers_.size()-1][x]->value);	
	}
}

void ANN::PrintNetwork() {
	for (int x = 0; x < layers_.size(); ++x) {
		debug("Layer %d", x);
		for (int y = 0; y < layers_[x].size(); ++y) {
			debug("\tNode %d value %f", y, layers_[x][y]->value);
			for (int z = 0; z < layers_[x][y]->weights.size(); ++z) {
				debug("\t\tWeight %f", layers_[x][y]->weights[z]);
			}
		}
	}
}
