#include "ann.h"

#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <stdlib.h>

using namespace std;

int load_network(ANN &ann, char *structure, char *weights);
int train_network(ANN &ann, char *input_file, char *output_file);

int main(int argc, char **argv) {
	ANN ann;

	for (int i = 0; i < argc; ++i) { debug("%s", argv[i]); }

	if (load_network(ann, argv[5], argv[6])) {
		return 1;
	}

	if (train_network(ann, argv[1], argv[2])) {
		return 1;
	}	

	ann.PrintNetwork();

	return 0;
}

int load_network(ANN &ann, char *structure, char *weights) {
	int i, x, y;
	int nodes;
	string line, weight;
	ifstream ifs_s(structure);
	ifstream ifs_w(weights);

	i = 0;

	while (!ifs_s.eof()) {
		getline(ifs_s, line);
		nodes = atoi(line.c_str());

		ann.AddLayer(nodes);

		if (i++ > 0) {
			for (x = 0; x < nodes; ++x) {
				ann.AddWeight(i-1, x, 0.01);
			}

			for (x = 0; x < nodes; ++x) {
				getline(ifs_w, line);
				stringstream weight_line(line);				
				y = 0;

				while (getline(weight_line, weight, ' ')) {
					ann.AddWeight(i-1, y++, atof(weight.c_str()));
				}	
			}	
		}		
	}

	ifs_s.close();
	ifs_w.close();

	return 0;
}

int train_network(ANN &ann, char *input_file, char *output_file) {
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

		for (i = 0; i < input.size(); ++i) {
			if (i == atoi(line_o.c_str())) {
				output.push_back(0.1);
			} else {
				output.push_back(0.9);
			}
		}

		ann.TrainNetwork(input, output);

		break;
	}

	ifs_o.close();
	ifs_i.close();

	return 0;
}
