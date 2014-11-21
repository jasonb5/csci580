#include "ann.h"

#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <limits>
#include <math.h>

using std::numeric_limits;

using namespace std;

int LoadNetwork(ANN &ann, char *structure, char *weights);
int TrainNetwork(ANN &ann, char *input_file, char *output_file, int iterations);
int TestNetwork(ANN &ann, char *input_file, char *output_file);
int ReadIntegerList(char *file, vector<int> &list);
int ReadDoubleTable(char *file, vector<vector<double> > &table);

const double kBias = 0.01;

int main(int argc, char **argv) {
	ANN ann(kBias);

	if (LoadNetwork(ann, argv[5], argv[6])) {
		return 1;
	}

  if (TrainNetwork(ann, argv[1], argv[2], atoi(argv[7]))) {
  	return 1;
	}	

  if (TestNetwork(ann, argv[3], argv[4])) {
		return 1;
  }

	return 0;
}

// Loads a network from two files, structure and weights.
// Format of files:
// 
// For structure:
// 2
// 3
// 2
//
// This creates a network with 2 inputs, 2 outputs and 1 hidden 
// layer containing 3 nuerons.
//
// For weights:
// 0.2 -0.1 0.1
// 0.3 0.2 -0.6
// 0.2 0.8
// 0.4 0.1
// -0.2 -0.3
//
// First and second line denote the weights leaving each input node.
// The three last lines denote the weights leaving each node in the 
// hidden layer.
int LoadNetwork(ANN &ann, char *structure, char *weights) {
  vector<int> s;
  vector<vector<double> > w;

  if (ReadIntegerList(structure, s)) {
    return 1;
  }

  if (ReadDoubleTable(weights, w)) {
    return 1;
  }

  int x, y, z, i;

	// Add each layer of the network and their neurons
  for (x = 0, i = 0; x < s.size(); ++x) {
    ann.AddLayer(s[x]);

		// This implementation stores the incoming weights
		// rather then the outgoing so we start on layer 1
		// to n
    if (x > 0) {
			// Add a bias nueron for each nueron in every layer except 
			// the input
			for (y = 0; y < s[x]; ++y) {
				ann.AddWeight(x, y, kBias);
			}

			// Assign each nueron all of its incoming weights
      for (y = 0; y < s[x-1]; ++y, ++i) {
        for (z = 0; z < s[x]; ++z) {
          ann.AddWeight(x, z, w[i][z]);
        }
      }
    } 
  }

	return 0;
}

// Trains a network given two files and the number of iterations, 
// train_input and train_output.
// Format of files:
//
// train_input:
// 0.2 0.1
// 0.3 0.4
// 0.1 0.2
//
// Each row corresponds with 1 set of input data for the network.
//
// train_output:
// 0
// 1
// 0
//
// Each row corresponds with the expected results of the input data.
// These values get converted to a list by shift 0.1 x spaces and 
// setting the rest to 0.9. e.g. 0 = 0.1 0.9 and 1 = 0.9 0.1
int TrainNetwork(ANN &ann, char *input_file, char *output_file, int iterations) {
  vector<int> output_class;
  vector<vector<double> > input, output;

  if (ReadDoubleTable(input_file, input)) {
    return 1;
  }

  if (ReadIntegerList(output_file, output_class)) {
    return 1;
  }

  int x, y;

	// Iterate through each set of input data
  for (x = 0; x < output_class.size(); ++x) {
    vector<double> row;

		// Convert out class to a list of expected values
    for (y = 0; y < ann.NodesInLayer(ann.Layers()-1); ++y) {
      if (y == output_class[x]) {
        row.push_back(0.1);
      } else {
        row.push_back(0.9);
      }
    }

    output.push_back(row);
  } 

	// One iteration constitutes running each test onces 
  for (x = 0; x < iterations; ++x) {
		// Run each test onces
    for (y = 0; y < input.size(); ++y) {
      ann.TrainNetwork(input[y], output[y]);
    }
  }

  return 0;
}

// Test data with two files, test_input and test_output.
// Format of files:
//
// See TrainNetwork above. 
int TestNetwork(ANN &ann, char *input_file, char *output_file) {
	vector<int> class_list;
	vector<vector<double> > input, output;	
	map<int, vector<double> > class_map;

	if (ReadDoubleTable(input_file, input)) {
		return 1;
	}

	if (ReadIntegerList(output_file, class_list)) {
		return 1;
	}

	int x, y;

	// Build a class map associating the class with expected output
	for (x = 0; x < class_list.size(); ++x) {
		vector<double> row;

		for (y = 0; y < ann.NodesInLayer(ann.Layers()-1); ++y) {
			if (y == class_list[x]) {
				row.push_back(0.1);	
			} else {
				row.push_back(0.9);
			}
		}

		class_map.insert(pair<int, vector<double> >(class_list[x], row));	
	}

	// Test each input and add results to vector
	for (x = 0; x < input.size(); ++x) {
		vector<double> output_test;

		ann.TestData(input[x], output_test);

		output.push_back(output_test);	
	}

	// Print the learned weights from the first neuron in the last hidden layer
	// to each output neuron
  for (x = 0; x < ann.NodesInLayer(ann.Layers()-1); ++x) {
    Neuron n = ann.NeuronAt(ann.Layers()-1, x);

    msg("%.16f\n", n.weights[1]);
  }

	int min_index, matches = 0;
	double minimum, value;
	vector<int> classified;
	map<int, vector<double> >::iterator it;

	// Determine classification for each input set.
	// Calculate Euclidean distance of each input set and
	// each class set finding the minimum distance. 
	for (x = 0; x < output.size(); ++x) {
		min_index = -1;
		minimum = numeric_limits<double>::max();

		// Calculate Euclidean distance of current set and each class set
		for (it = class_map.begin(); it != class_map.end(); ++it) {
			value = 0.0;
			
			for (y = 0; y < it->second.size(); ++y) {
				value += pow(output[x][y] - it->second[y], 2);
			}		

			value = sqrt(value);

			// Determine minimum distance
			if (value < minimum) {
				min_index = it->first;

				minimum = value;
			}
		}			

		classified.push_back(min_index);

		if (classified[x] == class_list[x]) {
			++matches;
		}
	}

	// Print the accuracy of the classifier
	msg("\n%.2f%%\n", (double)matches*100/classified.size());

	// Write classifications to output file
	char buf[2];
	ofstream ofs("ann_output.txt");

	for (x = 0; x < classified.size(); ++x) {
		sprintf(buf, "%d\n", classified[x]);

		ofs.write(buf, 2);	
	}

	ofs.close();

  return 0;
}

// Reads a list of integers from file
int ReadIntegerList(char *file, vector<int> &list) {
  string line;
  ifstream ifs(file);

	if (ifs.fail() || ifs.bad()) {
		error("Failed to open file %s", file);
		
		return 1;
	}

  while (!ifs.eof()) {
    getline(ifs, line);
   
		if (line.empty()) {
			continue;
		}
 
    list.push_back(atoi(line.c_str()));
  }
  
  ifs.close();

  return 0;
}

// Reads table of double values from file
int ReadDoubleTable(char *file, vector<vector<double> > &table) {
  string line, value;
  ifstream ifs(file);

	if (ifs.fail() || ifs.bad()) {
		error("Failed to open file %s", file);
		
		return 1;
	}

  while (!ifs.eof()) {
    vector<double> row;
    getline(ifs, line);
    stringstream line_stream(line);

		if (line.empty()) {
			continue;
		}

    while (getline(line_stream, value, ' ')) {
      row.push_back(atof(value.c_str()));
    }

    table.push_back(row);
  }

  ifs.close();

  return 0;
}
