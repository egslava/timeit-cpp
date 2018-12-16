// timeit.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "timeit.hpp"

void main() {
    using namespace std;
    _timeit::Stats results = timeit([] {
        pow(2, 4);
    });

    cout << results << endl;
    cout << "avg: " << results.avg << endl;
    cout << "min: " << results.min_result << endl;
    
}