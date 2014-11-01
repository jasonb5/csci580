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

#ifndef PROJECT2_EM_H_
#define PROJECT2_EM_H_

#include <stdio.h>

// Debug defines
#ifndef DEBUG
#define debug(M, ...) fprintf(stdout, "%s:%d:" M "\n", __FILE__, __LINE__, ##__VA_ARGS__);
#else
#define debug(M, ...)
#endif

// Error defines
#define error(M, ...) fprintf(stderr, "%s:%d:" M "\n", __FILE__, __LINE__, ##__VA_ARGS__);

#include <vector>
#include <stdlib.h>

using std::vector;

// Helper template for matrix
// Uses alias template define in c++11
template<typename T>
using matrix = vector<vector<T> >;

// Helper for initializing matrix
#define matrix(N, T, R, C) matrix<T> N(R, vector<T>(C))

// Class implements the expectation-maximization algorithm
// https://en.wikipedia.org/wiki/Expectation%E2%80%93maximization_algorithm
// The algorithm iterates over the results of applying the
// Viterbi algorithm for hidden Markov models
// 
// Exmaple usage:
// EM em;
// em.ParseObservations("observations.txt");
// em.ParseTransition("transition.txt");
// em.ParseSensory("sensory.txt");
// em.ParseOriginal("original.txt");
// em.CalculateEM(2);
//
// Files observations.txt, transition.txt, sensory.txt and original.txt
// describe a hidden Markov model. The hidden Markov model process by
// this class contains 3 states B, L, M and 2 observations H or T. The 
// class will output the discovered transition and sensory proabilities
// along with the accuracy.
//
// observations.txt:
// HTTHTTTHHHHTTHTTTTT
// 
// transition.txt:
// 0.45 0.52 0.25
// 0.35 0.3 0.13
// 0.2 0.18 0.62
//
// sensory.txt:
// 0.5
// 0.85
// 0.1
//
// original.txt:
// BBMMMMLLLLLBBBMMMMM 
class EM {
 public:
  // Constructor initializes matrices 
  EM();

  // Parses file containing observation data
  // See class comments for file format
  int ParseObservations(const char *file);

  // Parses file containing transition data
  // See class comments for file format
  int ParseTransition(const char *file);

  // Parses file containing sensory data
  // See class comments for file format
  int ParseSensory(const char *file); 

  // Parses file containing original data
  // See class comments for file format
  int ParseOriginal(const char *file);

  // Applies EM algorithm to results of a Viterbi algorithm on 
  // hidden Markov model
  int CalculateEM(int iterations);

 private:
  // Helper function to print matrix
  template<typename T> 
  void PrintMatrix(const matrix<T> m);

  // Helper function to print state matrix
  void PrintStateMatrix(const matrix<int> m);

  // Helper function to print observation matrix
  void PrintObservationMatrix(const matrix<int> m);

  // Undoes log base 2 function to matrix values
  void UndoLog2Matrix(matrix<double> *matrix); 

  // Calculates accuracy give most likely state sequence
  double CalculateClassifierAccuracy(const matrix<int> state_seq); 

  // Populates most likely state sequence given Viterbi and backtrace matrices
  void PopulateLikelyStateSequence(const matrix<double> vit, const matrix<int> back_trace, matrix<int> *state_seq);

  // Populates Viterbi and backtrace matrices
  void PopulateViterbiMatrix(matrix<double> *vit, matrix<int> *back_trace);

  // Updates transition matrix given current states 
  void UpdateTransitionMatrix(const matrix<int> states);

  // Updates sensory matrix given current states and observations
  void UpdateSensoryMatrix(const matrix<int> states);

  // Writes states to output.txt file
  void WriteStates(const matrix<int> states);

  matrix<int> observations_;
  matrix<double> transition_;
  matrix<double> sensory_;
  matrix<int> original_;
};

#endif // PROJECT2_EM_H_
