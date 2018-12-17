#include "stdafx.h"
#include "../timeit/timeit.hpp"

void main(int argc, char** argv) {
    using namespace std;

    timeit([] {
        pow(2, 4);
    });
    getchar();
}