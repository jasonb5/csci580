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

	for (int i = 0; i < argc; ++i) { debug("%s", argv[i]); }

	if (LoadNetwork(ann, argv[5], argv[6])) {
		return 1;
	}

  if (TrainNetwork(ann, argv[1], argv[2], atoi(argv[7]))) {
  	return 1;
	}	
//
//  if (TestNetwork(ann, argv[3], argv[4])) {
//    return 1;
//  }

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
    ann.AddLayer(s[x]);

    if (x > 0) {
      for (y = 0; y < s[x-1]; ++y, ++i) {
        for (z = 0; z < s[x]; ++z) {
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

  ann.PrintNetwork();

  ann.TrainNetwork(input[0], output[0], 1);

  ann.PrintNetwork();  

  return 0;
}

int TestNetwork(ANN &ann, char *input_file, char *output_file) {
	int i;
	string line_i, line_o, temp;
	ifstream ifs_i(input_file);
	ifstream ifs_o(output_file);
	vector<double> input, output;

	while (!ifs_i.eof()) {
		getline(ifs_i, line_i);
		getline(ifs_o, line_o);
		stringstream in_line(line_i);

		while (getline(in_line, temp, ' ')) {
			input.push_back(atof(temp.c_str()));
		}

  }

	ifs_o.close();
	ifs_i.close();

  return 0;
}

int ReadIntegerList(char *file, vector<int> &list) {
  string line;
  ifstream ifs(file);

  while (!ifs.eof()) {
    getline(ifs, line);
    
    list.push_back(atoi(line.c_str()));
  }
  
  ifs.close();

  return 0;
}

int ReadDoubleTable(char *file, vector<vector<double> > &table) {
  string line, value;
  ifstream ifs(file);

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
