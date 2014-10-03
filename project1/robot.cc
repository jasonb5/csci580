#include <iostream>

using namespace std;

class Robot {

};

void print_usage(void);

int main(int argc, char **argv) {
    if (argc < 3 || argv[1] == NULL) {
        print_usage();

        return 1;
    }

    return 0;
}

void print_usage(void) {
    cout << "./robot input.txt obs1 obs2 ..." << endl;
}
