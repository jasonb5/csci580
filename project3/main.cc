#include "ann.h"

#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <stdlib.h>

using namespace std;

int LoadNetwork(ANN &ann, char *structure, char *weights);
int TrainNetwork(ANN &ann, char *input_file, char *output_file, int iterations);
int TestNetwork(ANN &ann, char *input_file, char *output_file);
int ReadIntegerList(char *file, vector<int> &list);
int ReadDoubleTable(char *file, vector<vector<double> > &table);

int main(int argc, char **argv) {
	ANN ann;

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

  for (x = 0, i = 0; x < s.size(); ++x) {
		debug("Adding layer with %d nodes\n", s[x]);

    ann.AddLayer(s[x]);

    if (x > 0) {
			for (y = 0; y < s[x]; ++y) {
				debug("Adding dummy weight %f to node %d in layer %d\n", 0.01, y, x);

				ann.AddWeight(x, y, 0.01);
			}

      for (y = 0; y < s[x-1]; ++y, ++i) {
        for (z = 0; z < s[x]; ++z) {
					debug("Adding weight %f to node %d in layer %d\n", w[i][z], z, x);

          ann.AddWeight(x, z, w[i][z]);
        }
      }
    } 
  }

	return 0;
}

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

  for (x = 0; x < output_class.size(); ++x) {
    vector<double> row;

    for (y = 0; y < ann.NodesInLayer(ann.Layers()-1); ++y) {
      if (y == output_class[x]) {
        row.push_back(0.1);
      } else {
        row.push_back(0.9);
      }
    }

    output.push_back(row);
  } 

  for (x = 0; x < iterations; ++x) {
    for (y = 0; y < input.size(); ++y) {
      ann.TrainNetwork(input[y], output[y]);
    }
  }

  return 0;
}

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

	for (x = 0; x < class_list.size(); ++x) {
		vector<double> row;

		for (y = 0; y < ann.NodesInLayer(ann.Layers()-1); ++y) {
			if (y == class_list[x]) {
				row.push_back(0.1);	
			} else {
				row.push_back(0.9);
			}
		}

		class_map[class_list[x]] = row;	
	}

	for (x = 0; x < input.size(); ++x) {
		vector<double> output_test;

		debug("\nTesting data ");

		for (y = 0; y < input[x].size(); ++y) {
			debug("%f ", input[x][y]);
		}

		debug("\n");

		ann.TestData(input[x], output_test);

		debug("Network results ");

		for (y = 0; y < output_test.size(); ++y) {
			debug("%f ", output_test[y]);
		}

		debug("\n");

		output.push_back(output_test);	
	}

  for (x = 0; x < ann.NodesInLayer(ann.Layers()-1); ++x) {
    Neuron n = ann.NeuronAt(ann.Layers()-1, x);

    msg("%.16f\n", n.weights[1]);
  }
  
  vector<int> classified;

	ann.ClassifyData(output, class_map, classified);

  int matches = 0;
  map<int, vector<double> >::iterator it;

  for (x = 0, it = class_map.begin(); it != class_map.end(); ++it, ++x) {
    if (classified[x] == it->first) {
      ++matches;
    }    
  }

  msg("\n%.2f\n", (double)matches*100/(double)classified.size());

  return 0;
}

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

    while (getline(line_stream, value, ' ')) {
      row.push_back(atof(value.c_str()));
    }

    table.push_back(row);
  }

  ifs.close();

  return 0;
}
