#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdlib.h>

using namespace std;

#define NORTH   0x8
#define SOUTH   0x4
#define WEST    0x2
#define EAST    0x1

class Robot {

};

void print_usage(void);
void parse_map(char *, vector<vector<int> > *);
void parse_observations(int, char **, vector<int> *);

int main(int argc, char **argv) {
    Robot robot;
    vector<int> oVec;
    vector<vector<int> > mVec;
    
    if (argc < 4 || argv[1] == NULL) {
        print_usage();

        return 1;
    }

    parse_map(argv[1], &mVec);

    parse_observations(argc, argv, &oVec);   

    return 0;
}

void print_usage(void) {
    cout << "./robot input.txt serror obs1 obs2 ..." << endl;
}

void parse_map(char *file, vector<vector<int> > *mVec) {
    string line;
    ifstream ifs(file);

    while (!ifs.eof()) {
        getline(ifs, line);
        
        if (!line.empty()) {
            int l = 0, r;
            vector<int> row;
    
            while ((r = line.find(' ', l)) != -1) {
                row.push_back(atoi(line.substr(l, r-l).c_str()));
                
                l = r+1;
            }

            row.push_back(atoi(line.substr(l, line.length()-l).c_str()));
        
            mVec->push_back(row);
        }
    }

    ifs.close();
}

void parse_observations(int argc, char **argv, vector<int> *oVec) {
    for (int i = 3; i < argc; ++i) {
        int o = 0;
        string obs(argv[i]);

        for (int j = 0; j < obs.length(); ++j) {
            if (obs[j] == 'N') {
                o |= NORTH;       
            } else if (obs[j] == 'S') {
                o |= SOUTH;
            } else if (obs[j] == 'W') {
                o |= WEST;
            } else if (obs[j] == 'E') {
                o |= EAST;
            } else {
                cerr << "Unknown value in observation" << endl;

                return;
            }   
        }

        oVec->push_back(o);
    }
}
