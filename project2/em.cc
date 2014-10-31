//The MIT License (MIT)
//
//Copyright (c) 2014 Jason Boutte'
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#include "em.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <limits>

using namespace std;

// Default constructor
// Initializes all matrices needed for setup
EM::EM()
  : observations_(1),
    transition_(3, vector<double>(3, 0)),
    sensory_(3, vector<double>(2, 0)),
    original_(1) {
  cout << fixed << setprecision(6);
}

// Iterates over contents of a file building a
// matrix containing observation data
int EM::ParseObservations(const char *file) {
  char c; 
  ifstream ifs(file);

  while (ifs.get(c)) {
    if (c == 'H') {
      observations_[0].push_back(0);
    } else if (c == 'T') {
      observations_[0].push_back(1);
    }
  }

  ifs.close();

  return 0;
}

// Iterates over contents of a file building a
// matrix containing transition data
int EM::ParseTransition(const char *file) {
  int l, r;
  int x, y; 
  string line;
  ifstream ifs(file);

  l = x = y = 0;

  while (ifs.good()) {
    l = r = y = 0; 
   
    getline(ifs, line);

    if (line.empty()) {
      continue;
    }

    while ((r = line.find(' ', l)) != -1) {
      transition_[x][y++] = -log2(atof(line.substr(l, r - l).c_str()));

      l = r + 1;
    }

    transition_[x][y] = -log2(atof(line.substr(l, line.length() - r).c_str()));

    ++x;
  }

  ifs.close(); 
  
  return 0;
}

// Iterates over contents of a file building a 
// matrix containing sensory data
int EM::ParseSensory(const char *file) {
  int r(0);
  string line; 
  ifstream ifs(file);
  
  while (ifs.good()) {
    getline(ifs, line);
   
    if (line.empty()) {
      continue;
    }

    sensory_[r][0] = -log2(atof(line.substr(0, line.length()).c_str())); 
    sensory_[r++][1] = -log2(1 - atof(line.substr(0, line.length()).c_str())); 
  } 

  ifs.close();

  return 0;
}

// Iterates over contents of a file building a 
// matrix containing original states
int EM::ParseOriginal(const char *file) {
  char c; 
  string line;
  ifstream ifs(file);

  while (ifs.good()) {
    ifs.get(c);

    if (c == 'B') {
      original_[0].push_back(0); 
    } else if (c == 'L') {
      original_[0].push_back(1); 
    } else if (c == 'M') {
      original_[0].push_back(2); 
    }
  }

  ifs.close();

  return 0;
}

// Prints matrix
template<typename T>
void EM::PrintMatrix(matrix<T> m) {
  for (int r = 0; r < m.size(); ++r) {
    cout << " "; 
    
    for (int c = 0; c < m[r].size(); ++c) {
      cout << m[r][c] << " ";
    }

    cout << endl; 
  } 
}

// Prints states matrix
void EM::PrintStateMatrix(matrix<int> m) {
  cout << " ";

  for (int i = 0; i < m[0].size(); ++i) {
    cout << kModelStates[m[0][i]] << " ";
  }
  
  cout << endl;
}

// Prints observation matrix
void EM::PrintObservationMatrix(matrix<int> m) {
  cout << " ";

  for (int i = 0; i < m[0].size(); i++) {
    cout << kModelObservations[m[0][i]];
  }

  cout << endl;
}

// Calculates EM over x iterations
int EM::CalculateEM(int iterations) {
  matrix(vit, double, 3, observations_[0].size()+1);
  matrix(back_trace, int, 3, observations_[0].size()+1);
  matrix(state_seq, int, 1, observations_[0].size()+1);
  
  for (int x = 0; x < 3; ++x) {
    vit[x][0] = -log2((double)1/(double)3);
  }  
  
  PopulateViterbiMatrix(&vit, &back_trace);

  PopulateLikelyStateSequence(vit, back_trace, &state_seq);
  
  for (int i = 0; i < iterations; ++i) {
    UpdateTransitionMatrix(state_seq);
   
    UpdateSensoryMatrix(state_seq);

    PopulateViterbiMatrix(&vit, &back_trace);

    PopulateLikelyStateSequence(vit, back_trace, &state_seq);
  }
  
  UndoLog2Matrix(&transition_);
  UndoLog2Matrix(&sensory_);  

  WriteStates(state_seq);

  cout << "Transition probabilities learned:" << endl;
  PrintMatrix(transition_);
  cout << endl << "Sensory probabilities learned:" << endl;
  PrintMatrix(sensory_);
  cout << endl << "Accuracy:" << endl;
  cout << " " << CalculateClassifierAccuracy(state_seq) * 100 << endl;

  return 0;
}

// Undoes log base 2 on a matrix
void EM::UndoLog2Matrix(matrix<double> *matrix) {
  for (int x = 0; x < matrix->size(); ++x) {
    for (int y = 0; y < (*matrix)[0].size(); ++y) {
      (*matrix)[x][y] = pow(2, -(*matrix)[x][y]);
    }
  }
}

// Calculates accuracy of em classifier
double EM::CalculateClassifierAccuracy(matrix<int> state_seq) {
  int match;
  
  for (int x = 0; x < original_[0].size(); ++x) {
    if (state_seq[0][x+1] == original_[0][x]) {
      ++match;
    }
  }
  
  return (double)match/(double)original_[0].size();
}

// Backtraces through matrix create by Viterbi algorithm
void EM::PopulateLikelyStateSequence(matrix<double> vit, matrix<int> back_trace, matrix<int> *state_seq) {
  int index;
  double min = numeric_limits<double>::max();

  for (int x = 0; x < vit.size(); ++x) {
    if (vit[x][vit[0].size()-1] < min) {
      index = x;

      min = vit[x][vit[0].size()-1];
    }
  }

  (*state_seq)[0][(*state_seq)[0].size()-1] = index; 

  for (int x = (*state_seq)[0].size()-1; x > 0; --x) {
    index = back_trace[index][x];

    (*state_seq)[0][x-1] = index;
  }
}

// Performs Viterbi algorithm
void EM::PopulateViterbiMatrix(matrix<double> *vit, matrix<int> *back_trace) {
  for (int x = 1; x < (*vit)[0].size(); ++x) {
    for (int y = 0; y < vit->size(); ++y) {
      int index; 
      double min = numeric_limits<double>::max(); 

      for (int z = 0; z < vit->size(); ++z) {
        double value = (*vit)[z][x-1] + transition_[y][z];
     
        if (value < min) {
          index = z;

          min = value;
        }
      }
      
      (*back_trace)[y][x] = index;

      (*vit)[y][x] = min + ((observations_[0][x-1] == 0) ? sensory_[y][0] : sensory_[y][1]);
    }
  }
}

// Analyzes most likely state sequence and updates transition matrix
void EM::UpdateTransitionMatrix(matrix<int> states) {
  int b(0), l(0), m(0); 
  int b_b(0), b_l(0), b_m(0); 
  int l_b(0), l_l(0), l_m(0);
  int m_b(0), m_l(0), m_m(0); 
  
  for (int x = 1; x < states[0].size(); ++x) {
    
    if (states[0][x-1] == 0 && states[0][x] == 0) {
     ++b; ++b_b;
    } else if (states[0][x-1] == 0 && states[0][x] == 1) {
     ++b; ++b_l;
    } else if (states[0][x-1] == 0 && states[0][x] == 2) {
     ++b; ++b_m;
    } else if (states[0][x-1] == 1 && states[0][x] == 0) {
     ++l; ++l_b;
    } else if (states[0][x-1] == 1 && states[0][x] == 1) {
     ++l; ++l_l;
    } else if (states[0][x-1] == 1 && states[0][x] == 2) {
     ++l; ++l_m;
    } else if (states[0][x-1] == 2 && states[0][x] == 0) {
     ++m; ++m_b;
    } else if (states[0][x-1] == 2 && states[0][x] == 1) {
     ++m; ++m_l;
    } else if (states[0][x-1] == 2 && states[0][x] == 2) {
     ++m; ++m_m;
    } 
  }

  transition_[0][0] = (double)(b_b + 1)/(double)(b + 3 * 1);
  transition_[1][0] = (double)(b_l + 1)/(double)(b + 3 * 1);
  transition_[2][0] = (double)(b_m + 1)/(double)(b + 3 * 1);

  transition_[0][1] = (double)(l_b + 1)/(double)(l + 3 * 1);
  transition_[1][1] = (double)(l_l + 1)/(double)(l + 3 * 1);
  transition_[2][1] = (double)(l_m + 1)/(double)(l + 3 * 1);

  transition_[0][2] = (double)(m_b + 1)/(double)(m + 3 * 1);
  transition_[1][2] = (double)(m_l + 1)/(double)(m + 3 * 1);
  transition_[2][2] = (double)(m_m + 1)/(double)(m + 3 * 1);

  for (int x = 0; x < 3; ++x) {
    for (int y = 0; y < 3; ++y) {
      transition_[x][y] = -log2(transition_[x][y]);
    }
  }
}

// Analyzes most likely state sequence and updates sensory matrix
void EM::UpdateSensoryMatrix(matrix<int> states) {
  int b(0), l(0), m(0); 
  int h_b(0), h_l(0), h_m(0); 
  
  for (int x = 0; x < observations_[0].size(); ++x) {
    if (states[0][x+1] == 0) {
      ++b;
    } else if (states[0][x+1] == 1) {
      ++l;
    } else if (states[0][x+1] == 2) {
      ++m;
    } 
    
    if (observations_[0][x] == 0 && states[0][x+1] == 0) {
      ++h_b;
    } else if (observations_[0][x] == 0 && states[0][x+1] == 1) {
      ++h_l;
    } else if (observations_[0][x] == 0 && states[0][x+1] == 2) {
      ++h_m;
    } 
  }
  
  sensory_[0][0] = (double)(h_b + 1)/(double)(b + 2 * 1);
  sensory_[1][0] = (double)(h_l + 1)/(double)(l + 2 * 1);
  sensory_[2][0] = (double)(h_m + 1)/(double)(m + 2 * 1);

  for (int x = 0; x < sensory_.size(); ++x) {
    sensory_[x][1] = -log2(1-sensory_[x][0]);
    sensory_[x][0] = -log2(sensory_[x][0]);
  }
}

// Writes most likely state to output file
void EM::WriteStates(matrix<int> states) {
  ofstream ofs("output.txt");

  for (int i = 0; i < states[0].size(); ++i) {
    ofs.write(kModelStates[states[0][i]], 1); 
  }

  ofs.close();
}
