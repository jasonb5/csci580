#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <iomanip>

using namespace std;

#define DEBUG

#define NORTH   0x8
#define SOUTH   0x4
#define WEST    0x2
#define EAST    0x1

class Robot {
  public:
    int Localize(int, vector<vector<int> > &, vector<int> &);

  private:
    int CntValidPos(vector<vector<int> > &);
    int CntValidPath(int);
    int InitVec(vector<vector<double> > *, int, int, double);
    int InitTransVec(vector<vector<int> > &, vector<vector<double> > *, \
        int, int);
    void TransposeVec(vector<vector<double> > *);

    void PrintVec(vector<vector<double> >);
};

int Robot::Localize(int serror, vector<vector<int> > &mVec, vector<int> &oVec) {
  int valid_pos;
  int total_nodes;
  vector<double> j;
  vector<vector<double> > t;
  vector<vector<double> > o;

#ifdef DEBUG
  cout << fixed;
  cout << setprecision(10);
#endif

  total_nodes = mVec.size() * mVec[1].size();

  valid_pos = CntValidPos(mVec);
  
  for (int i = 0; i < valid_pos; ++i) {
    j.push_back((double)1/(double)valid_pos);
  }

  InitVec(&t, total_nodes, total_nodes, 0.0);

  InitTransVec(mVec, &t, valid_pos, valid_pos);
 
  TransposeVec(&t);

  PrintVec(t);

  return 0;
}

int Robot::CntValidPos(vector<vector<int> > &mVec) {
  int valid = 0;

  for (int i = 0; i < mVec.size(); ++i) {
    for (int j = 0; j < mVec[i].size(); ++j) {
      if (mVec[i][j] != 0xf) {
        ++valid;
      }
    }    
  }

  return valid;
}

int Robot::CntValidPath(int node) {
  int valid = 0;

  for (int i = 0; i < 4; ++i) {
    if (((node >> i) & 0x1) == 0x0) {
      ++valid;
    }
  }

  return valid;
}

int Robot::InitVec(vector<vector<double> > *vec, int width, int height, \
    double value) {
  for (int i = 0; i < height; ++i) {
    vector<double> row;

    for (int j = 0; j < width; ++j) {
       row.push_back(value);
    }

    vec->push_back(row);
  } 
  
  return 0;
}

int Robot::InitTransVec(vector<vector<int> > &mVec, vector<vector<double> > \
    *tVec, int width, int height) {
  int abs;
  int valid_paths;
  double value;

  for (int i = 0; i < mVec.size(); ++i) {
    for (int j = 0; j < mVec[i].size(); ++j) {
      abs = i * mVec[i].size() + j;

      valid_paths = CntValidPath(mVec[i][j]);

      if (valid_paths > 0) {
        value = (double)1/(double)valid_paths;
      } else {
        value = 0;
      }

      if ((mVec[i][j] & NORTH) == 0x0) {
        (*tVec)[abs][abs-mVec[i].size()] = value;
      }

      if ((mVec[i][j] & SOUTH) == 0x0) {
        (*tVec)[abs][abs+mVec[i].size()] = value;
      }

      if ((mVec[i][j] & WEST) == 0x0) {
        (*tVec)[abs][abs-1] = value;
      }

      if ((mVec[i][j] & EAST) == 0x0) {
        (*tVec)[abs][abs+1] = value;
      }
    }
  }

  return 0;
}

void Robot::TransposeVec(vector<vector<double> > *tVec) {
  vector<vector<double> > copy(*tVec);

  for (int i = 0; i < copy.size(); ++i) {
    for (int j = 0; j < copy[i].size(); ++j) {
      (*tVec)[j][i] = copy[i][j];
    }
  }
}

void Robot::PrintVec(vector<vector<double> > vec) {
  for (int i = 0; i < vec.size(); ++i) {
    for (int j = 0; j < vec[i].size(); ++j) {
      cout << vec[i][j] << " ";
    }

    cout << endl;
  }
}

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

    return robot.Localize(atoi(argv[3]), mVec, oVec);
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
