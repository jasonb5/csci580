#include "em.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc != 6) {
    printf("%s <observation file> <transition file> <sensory file> <original> k\n", \
        argv[0]);

    exit(1);
  }

  return 0;
}
