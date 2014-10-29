#include "em.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <math.h>

using namespace std;

EM::EM()
  : observations_(),
    transition_(3, vector<double>(3, 0)),
    sensory_(3, vector<double>(1, 0)) {
  cout << fixed << setprecision(6);
}

int EM::ParseObservations(const char *file) {
  char c; 
  ifstream ifs(file); 

  while (ifs.good()) {
    ifs.get(c);

    if (c == 'H') {
      observations_.push_back(0);
    } else if (c == 'T') {
      observations_.push_back(1);
    }
  } 

  ifs.close();

  return 0;
}

int EM::ParseTransition(const char *file) {
  int l, r;
  int x, y; 
  string line;
  ifstream ifs(file);

  l = x = y = 0;

  while (ifs.good()) {
    l = r = y = 0; 
   
    getline(ifs, line);

    while ((r = line.find(' ', l)) != -1) {
      transition_[x][y++] = -log(atof(line.substr(l, r - l).c_str()));

      l = r + 1;
    }

    transition_[x][y] = -log(atof(line.substr(l, line.length() - r).c_str()));

    ++x;
  }

  ifs.close(); 
  
  return 0;
}

int EM::ParseSensory(const char *file) {
  int r(0);
  string line; 
  ifstream ifs(file);
  
  while (ifs.good()) {
    getline(ifs, line);
    
    sensory_[r++][0] = -log(atof(line.substr(0, line.length()).c_str())); 
  } 

  ifs.close();

  return 0;
}

int EM::ParseOriginal(const char *file) {
  char c; 
  string line;
  ifstream ifs(file);

  while (ifs.good()) {
    ifs.get(c);

    if (c == 'B') {
      original_.push_back(0); 
    } else if (c == 'L') {
      original_.push_back(1); 
    } else if (c == 'M') {
      original_.push_back(2); 
    }
  }

  ifs.close();

  return 0;
}

void EM::PrintMatrix(matrix<double> m) {
  for (int r = 0; r < m.size(); ++r) {
    for (int c = 0; c < m[r].size(); ++c) {
      cout << m[r][c] << " ";
    }

    cout << endl; 
  } 
}

int EM::CalculateEM(int k) {
  matrix<double> vit(3, vector<double>(observations_.size() + 1));

  for (int i = 0; i < 3; ++i) {
    vit[i][0] = -log((double)1/(double)3);
  } 

  PrintMatrix(transition_); printf("\n");
  PrintMatrix(sensory_); printf("\n");
  PrintMatrix(vit); printf("\n");

  return 0;
}
