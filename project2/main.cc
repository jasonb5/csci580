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

  return 0;
}
