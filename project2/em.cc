#include "em.h"

#include <fstream>

using namespace std;

EM::EM()
  : observations_(),
    transition_(3, vector<double>(3, 0)) {

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
    getline(ifs, line);
 
    y = 0;

    while ((r = line.find(' ', l)) != -1) {
      transition_[x][y++] = atof(line.substr(l, r - l).c_str());

      l = r + 1; 
    }

    transition_[x][y] = atof(line.substr(l, line.length() - r).c_str());

    ++x;
  }

  PrintMatrix(transition_);

  ifs.close(); 
  
  return 0;
}

void EM::PrintMatrix(matrix<double> m) {
  for (int r = 0; r < m.size(); ++r) {
    for (int c = 0; c < m[r].size(); ++c) {
      printf("%f\t", m[r][c]);
    }
 
    printf("\n");   
  } 
}
