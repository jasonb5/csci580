#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <math.h>

using namespace std;

#define NORTH   0x8
#define SOUTH   0x4
#define WEST    0x2
#define EAST    0x1

#define DEBUG

template<typename T>
using Matrix = vector<vector<T> >;

#define Matrix(t, n, w, h) Matrix<t> n(h, vector<t>(w, 0))
#define PMatrix(t, n, w, h) new Matrix<t>(h, vector<t>(w, 0)) 

class Robot {
  public:
    int Init(int argc, char **argv);
    int Localize();

  private:
    double m_error; 
    Matrix<int> m_map; 
    vector<int> m_obs; 

    void InitSensoryMatrix(double, Matrix<double> *);
    void InitTransitionMatrix(Matrix<int> &, Matrix<double> *);
    void InitObsMatrix(int, Matrix<int> &, Matrix<double> &, Matrix<double> *);

    void TransposeMatrix(Matrix<double> *);
    Matrix<double> &MultiplyMatrices(Matrix<double> &, Matrix<double> &);

    int CalcObsStateDiff(int, int);

    int CountValidPaths(int);
    int CountValidStates(Matrix<int> &);
    
    int ParseMap(const char *);
    int ParseObs(const char *);

    void PrintUsage();
    void PrintMatrix(Matrix<double> &);
};

int Robot::Init(int argc, char **argv) {
#ifdef DEBUG
  cout << fixed;
  cout << setprecision(10);
#endif

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
  int states = m_map.size() * m_map[0].size();
  int valid_states = CountValidStates(m_map);  
  Matrix(double, j, 1, states);
  Matrix(double, t, states, states);
  Matrix(double, s, 1, 5);
  Matrix(double, o, states, states);
  
  for (int x = 0; x < m_map.size(); ++x) {
    for (int y = 0; y < m_map[x].size(); ++y) {
      if (m_map[x][y] == 0xf) {
        j[x*m_map.size()+y][0] = 0;
      } else {
        j[x*m_map.size()+y][0] = (double)1/(double)valid_states;
      }
    }
  }

  InitSensoryMatrix(m_error, &s);
  InitTransitionMatrix(m_map, &t);

  TransposeMatrix(&t);

  for (int x = 0; x < m_obs.size()-1; ++x) {
    InitObsMatrix(m_obs[x], m_map, s, &o);
  }

  Matrix(double, m1, 3, 3);

  m1[0][0] = 1;
  m1[0][1] = 0;
  m1[0][2] = 1;

  m1[1][0] = 1;
  m1[1][1] = 1;
  m1[1][2] = 1;

  m1[2][0] = 0;
  m1[2][1] = 0;
  m1[2][2] = 0;

  Matrix(double, m2, 1, 3);

  m2[0][0] = 1;
  m2[1][0] = 0;
  m2[2][0] = 1;

  Matrix<double> m3 = MultiplyMatrices(m1, m2);

  PrintMatrix(m3);

  return 0;
}

void Robot::InitSensoryMatrix(double error, Matrix<double> *s) {
  for (double x = 0; x < 5; ++x) {
    (*s)[x][0] = pow(error, x) * pow(1-error, 4-x);
  }
}

void Robot::InitTransitionMatrix(Matrix<int> &m, Matrix<double> *t) {
  for (int x = 0; x < m.size(); ++x) {
    for (int y = 0; y < m[x].size(); ++y) {
      int valid_paths = CountValidPaths(m[x][y]);
      double value = (double)1/(double)valid_paths;
      int index = x*m[x].size()+y;
      
      if ((m[x][y] & NORTH) == 0x0) {
        (*t)[index][index-m[x].size()] = value;
      }
    
      if ((m[x][y] & SOUTH) == 0x0) {
        (*t)[index][index+m[x].size()] = value;
      }
 
      if ((m[x][y] & WEST) == 0x0) {
        (*t)[index][index-1] = value;
      }
      
      if ((m[x][y] & EAST) == 0x0) {
        (*t)[index][index+1] = value;
      }
    }
  } 
}

void Robot::InitObsMatrix(\
    int obs, Matrix<int> &m, Matrix<double> &s, Matrix<double> *o) {
  for (int x = 0; x < m.size(); ++x) {
    for (int y = 0; y < m[x].size(); ++y) {
      int index = x*m[x].size()+y;
      int diff = CalcObsStateDiff(m[x][y], obs);
     
      (*o)[index][index] = s[diff][0]; 
    }
  }
}

void Robot::TransposeMatrix(Matrix<double> *m) {
  Matrix<double> c(*m);

  for (int x = 0; x < c.size(); ++x) {
    for (int y = 0; y < c[x].size(); ++y) {
      (*m)[x][y] = c[y][x];
    }
  }
}

/*
 * 0 1 0    0 1    1 0 
 * 1 0 1    1 0    1 2
 * 1 1 1    1 1    2 2
 * 0 1 1           2 1
 * 
 *  4x3     3x2
 */
Matrix<double> &Robot::MultiplyMatrices(Matrix<double> &m, Matrix<double> &n) {
  Matrix<double> *r = PMatrix(double, r, n[0].size(), m.size());

  PrintMatrix(m);
  cout << endl;
  PrintMatrix(n);
  cout << endl;

  for (int x = 0; x < m.size(); ++x) {
    for (int y = 0; y < n[0].size(); ++y) {
      double total(0);
      
      for (int z = 0; z < n.size(); ++z) {
        cout << m[x][z] << " * " << n[z][y] << endl;
    
        total += m[x][z] * n[z][y];
      }

      cout << endl;
   
      (*r)[x][y] = total; 
    }
  } 

  return *r;
}

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

int Robot::CountValidPaths(int state) {
  int value(0);

  for (int x = 0; x < 4; ++x) {
    if (((state >> x) & 0x1) == 0x0) {
      ++value;
    }
  }

  return value; 
}

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
