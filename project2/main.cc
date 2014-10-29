#define DEBUG

#include "em.h"


int main(int argc, char **argv) {
  EM em;

  if (em.ParseObservations(argv[1])) {
    error("Failed to parse observations");

    exit(1);
  }

  if (em.ParseTransition(argv[2])) {
    error("Failed to parse transition");

    exit(1);
  }

  if (em.ParseSensory(argv[3])) {
    error("Failed to parse sensory");

    exit(1);
  }

  if (em.ParseOriginal(argv[4])) {
    error("Failed to parse original");

    exit(1);
  }

  if (em.CalculateEM(atoi(argv[5]))) {
    error("Failed to calculate em");

    exit(1);
  }

  return 0;
}
