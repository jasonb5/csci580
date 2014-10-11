#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdlib.h>

using namespace std;

#define NORTH   0x8
#define SOUTH   0x4
#define WEST    0x2
#define EAST    0x1

// Type alias defined in c++11
// http://en.cppreference.com/w/cpp/language/type_alias
template<typename T>
using Matrix = vector<vector<T> >;

#define Matrix(t, n, w, h) Matrix<t> n(h, vector<t>(w, 0))
#define PMatrix(t, n, w, h) new Matrix<t>(h, vector<t>(w, 0)) 

// Determines robots probable location based on given observations
// Map definition:
// 
// 10 12  9
// 7  15  7
//
// Each value defines a bit obstacle mask
// NSWE e.g. NW=10
//
// Observations follow the same format as the map definition
class Robot {
  public:
    // Initializes robot, requires arguments as follows
    // [1]        - File containing map data
    // [2]        - Sensor error
    // [3]...[x]  - List of observations
    int Init(int argc, char **argv);
    // Determines robots probable position and prints the results
    // as a coordinate and probability
    int Localize();

  private:
    double m_error; 
    Matrix<int> m_map; 
    vector<int> m_obs; 

    // Initializes matrix with sensory data
    void InitSensoryMatrix(double error, Matrix<double> *s);
    // Initializes matrix with transition data
    void InitTransitionMatrix(Matrix<int> &map, Matrix<double> *t);
    // Populates matrix with observation data
    void InitObsMatrix(int obs, Matrix<int> &map, Matrix<double> &s, \
        Matrix<double> *o);

    // Transposes matrix
    // m[x][y] becomes m[y][x]
    void TransposeMatrix(Matrix<double> *m);
    // Returns resultant matrix
    //
    // Multiplies matrices
    Matrix<double> &MultiplyMatrices(Matrix<double> &m, Matrix<double> &n);
    // Normalizes matrix by summing all values and dividing each
    // by this value
    void NormalizeMatrix(Matrix<double> *m);
    // Returns the difference between the observation and state
    // e.g.
    // NW (10) = states
    // NS (12) = observation
    //
    // NW   1010
    // NS   1100
    // XOR  0110
    //
    // Diff = 2
    int CalcObsStateDiff(int obs, int state);
    // Returns the valid paths of a given state
    // NW = 2 because S and E are not blocked by an obstacle
    int CountValidPaths(int state);
    // Returns the valid states in a map
    // 
    // 10 12 9
    // 7  15 7
    //
    // This example would return 5
    // Each of the 5 valid states containt atleast 1 valid path e.g. not 
    // equal to 15 which has all four sides blocked
    int CountValidStates(Matrix<int> &m);
    // Returns true on success
    // 
    // Parses map contained in file. Format looks as follows
    // 10 12 9
    // 7  15 7
    int ParseMap(const char *file);
    // Returns int representation of observation
    // 
    // Parses observation given as a combination of
    // NWSE
    // 
    // e.g.
    //
    // NS = 1100 or 12
    int ParseObs(const char *obs);
    // Prints programs usage
    void PrintUsage();
    // Prints a matrix
    void PrintMatrix(Matrix<double> &);
};

// Returns true on success
int Robot::Init(int argc, char **argv) {
  cout << fixed;
  cout << setprecision(6);

  if (argv[1] == NULL) {
    PrintUsage(); 
    
    return 1;
  }

  if (ParseMap(argv[1]) == 1) {
    return 1;
  } 

  m_error = atof(argv[2]);

  for (int i = 3; i < argc; ++i) {
    m_obs.push_back(ParseObs(argv[i]));
  } 
  
  return 0;
}

int Robot::Localize() {
  // Variable initializations
  int states = m_map.size() * m_map[0].size();
  int valid_states = CountValidStates(m_map);  
  Matrix(double, j, 1, states);
  Matrix(double, t, states, states);
  Matrix(double, s, 1, 5);
  Matrix(double, o, states, states);
  
  // Initialize joint matrix
  for (int x = 0; x < m_map.size(); ++x) {
    for (int y = 0; y < m_map[x].size(); ++y) {
      int index = x*m_map[x].size()+y;

      // Assigns initial probability to state
			if (m_map[x][y] == 0xf) {
        j[index][0] = 0;
      } else {
        j[index][0] = (double)1/(double)valid_states;
      }
    }
  }

  InitSensoryMatrix(m_error, &s);
  InitTransitionMatrix(m_map, &t);

  // Transposes transition matrix  
  TransposeMatrix(&t);

	j = MultiplyMatrices(t, j);

  // Loops through observations
  for (int x = 0; x < m_obs.size()-1; ++x) {
    // Populates matrix with observation data
    InitObsMatrix(m_obs[x], m_map, s, &o);
    
    // Multiplies transition by observation matrix
    Matrix<double> temp = MultiplyMatrices(t, o);

    // Adjusts joint matrix base on transition and observation matrix
    j = MultiplyMatrices(temp, j);
  }

  // Populates matrix with final observation
  InitObsMatrix(m_obs[m_obs.size()-1], m_map, s, &o);
  
  // Adjusts joint matrix
	j = MultiplyMatrices(o, j);

  // Normalizes joint matrix
  NormalizeMatrix(&j);

  double max(0);
  vector<int> results;

  // Create a vector containing states
  // with the highest probabilities
  for (int x = 0; x < j.size(); ++x) {
    if (j[x][0] > max) {
      max = j[x][0];

      results.clear();

      results.push_back(x);
    } else if (j[x][0] == max) {
      results.push_back(x);
    }
  }

  // Prints results
  for (int x = 0; x < results.size(); ++x) {
    int row = results[x] / m_map[0].size();
    int col = results[x] - (row * m_map[0].size());

    cout << "(" << row << "," << col << ") " << max << endl;
  }

  return 0;
}

// Initializes sensory matrix using the following equation
// p(e|s) = error^d * (1-error)^x-d
// error: sensor error
// d: Difference between state and observation
// x: Max differences between state and observation
void Robot::InitSensoryMatrix(double error, Matrix<double> *s) {
  for (double x = 0; x < 5; ++x) {
    (*s)[x][0] = pow(error, x) * pow(1-error, 4-x);
  }
}

// Initializes transition matrix
void Robot::InitTransitionMatrix(Matrix<int> &m, Matrix<double> *t) {
  // Determines if states has valid path in each direction
  for (int x = 0; x < m.size(); ++x) {
    for (int y = 0; y < m[x].size(); ++y) {
      int valid_paths = CountValidPaths(m[x][y]);
      double value = (double)1/(double)valid_paths;
      int index = x*m[x].size()+y;
      
      // Shifts row index one full row backwards
      if ((m[x][y] & NORTH) == 0x0) {
        (*t)[index][index-m[x].size()] = value;
      }
      
      // Shifts row index one full row forwards
      if ((m[x][y] & SOUTH) == 0x0) {
        (*t)[index][index+m[x].size()] = value;
      }
 
      // Shifts row index one value left
      if ((m[x][y] & WEST) == 0x0) {
        (*t)[index][index-1] = value;
      }
      
      // Shifts row index one value right
      if ((m[x][y] & EAST) == 0x0) {
        (*t)[index][index+1] = value;
      }
    }
  } 
}

// Initializes observation matrix
void Robot::InitObsMatrix(\
    int obs, Matrix<int> &m, Matrix<double> &s, Matrix<double> *o) {
  for (int x = 0; x < m.size(); ++x) {
    for (int y = 0; y < m[x].size(); ++y) {
      int index = x*m[x].size()+y;
      // Determines difference between state and observation
			int diff = CalcObsStateDiff(m[x][y], obs);
     
     	(*o)[index][index] = s[diff][0]; 
    }
  }
}

// Transposes matrix
void Robot::TransposeMatrix(Matrix<double> *m) {
  Matrix<double> c(*m);

  for (int x = 0; x < c.size(); ++x) {
    for (int y = 0; y < c[x].size(); ++y) {
      (*m)[x][y] = c[y][x];
    }
  }
}

// Multiplies matrix
Matrix<double> &Robot::MultiplyMatrices(Matrix<double> &m, Matrix<double> &n) {
  Matrix<double> *r = PMatrix(double, r, n[0].size(), m.size());

  for (int x = 0; x < m.size(); ++x) {
    for (int y = 0; y < n[0].size(); ++y) {
      double total(0);
      
      for (int z = 0; z < n.size(); ++z) {
        total += m[x][z] * n[z][y];
      }
      
			(*r)[x][y] = total; 
    }
  } 

  return *r;
}

// Normalizes matrix
void Robot::NormalizeMatrix(Matrix<double> *m) {
  double total(0);

  for (int x = 0; x < m->size(); ++x) {
    for (int y = 0; y < (*m)[x].size(); ++y) {
      total += (*m)[x][y];
    }
  }

  for (int x = 0; x < m->size(); ++x) {
    for (int y = 0; y < (*m)[x].size(); ++y) {
      (*m)[x][y] /= total;
    }
  }
}

// Returns difference between state and observation
int Robot::CalcObsStateDiff(int state, int obs) {
  int value(0); 
  int diff = state ^ obs;

  for (int i = 0; i < 4; ++i) {
    if (((diff >> i) & 0x1) == 0x1) {
      ++value;
    } 
  }

  return value;
}

// Returns valid paths of a given state
int Robot::CountValidPaths(int state) {
  int value(0);

  for (int x = 0; x < 4; ++x) {
    if (((state >> x) & 0x1) == 0x0) {
      ++value;
    }
  }

  return value; 
}

// Returns valid states in a map
int Robot::CountValidStates(Matrix<int> &m) {
  int value(0);

  for (int x = 0; x < m.size(); ++x) {
    for (int y = 0; y < m[x].size(); ++y) {
      if (m[x][y] != 0xf) {
        ++value;
      }
    }
  }

  return value;
}

int Robot::ParseMap(const char *file) {
  string line; 
  ifstream ifs(file);

  if (ifs.fail()) {
    cerr << "Failed to open " << file << endl; 

    return 1;
  }

  while (!ifs.eof()) {
    getline(ifs, line);
  
    if (!line.empty()) {
      int l(0), r(0);
      vector<int> row;

      while ((r = line.find(' ', l)) != -1) {
        row.push_back(atoi(line.substr(l, r-l).c_str()));
        
        l=r+1;
      }

      row.push_back(atoi(line.substr(l, line.length()-l).c_str()));
    
      m_map.push_back(row); 
    } 
  }

  ifs.close(); 

  return 0;
}

int Robot::ParseObs(const char *obs) {
  string s(obs);
  int value(0); 
 
  for (int i = 0; i < s.length(); ++i) {
    if (s[i] == 'N') {
      value |= NORTH;
    } else if (s[i] == 'S') {
      value |= SOUTH;
    } else if (s[i] == 'W') {
      value |= WEST;
    } else if (s[i] == 'E') {
      value |= EAST;
    } 
  }

  return value;
}

void Robot::PrintUsage() {
  cout << "./robot <input file> <error> <obs1> <obs2>..." << endl;
}

void Robot::PrintMatrix(Matrix<double> &m) {
  for (int x = 0; x < m.size(); ++x) {
    for (int y = 0; y < m[x].size(); ++y) {
      cout << m[x][y] << "\t";
    }

    cout << endl;
  }
}

int main(int argc, char **argv) {
  Robot robot;

  if (robot.Init(argc, argv) == 1) {
    return 1;
  } 
 
  return robot.Localize();
}
