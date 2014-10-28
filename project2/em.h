#ifndef PROJECT2_EM_H_
#define PROJECT2_EM_H_

#include <stdio.h>

#ifndef DEBUG
#define debug(M, ...) fprintf(stdout, "%s:%d:" M "\n", __FILE__, __LINE__, ##__VA_ARGS__);
#else
#define debug(M, ...)
#endif

#define error(M, ...) fprintf(stderr, "%s:%d:" M "\n", __FILE__, __LINE__, ##__VA_ARGS__);

#include <vector>
#include <stdlib.h>

using std::vector;

template<typename T>
using matrix = vector<vector<T> >;

class EM {
 public:
  EM();

  int ParseObservations(const char *file);
  int ParseTransition(const char *file);

 private:
  void PrintMatrix(matrix<double> m);

  vector<int> observations_;
  matrix<double> transition_;
};

#endif // PROJECT2_EM_H_
