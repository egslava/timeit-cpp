// example2_manual_output.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../timeit/timeit.hpp"
#include <cmath>
#include <algorithm>
#include <iomanip>

double distance(double x, double y) noexcept {
    return sqrt(x*x + y * y);
}

double approx_distance(double x, double y) noexcept {
    using namespace std;

    double ax = abs(x);
    double ay = abs(y);
    return 1007. / 1024.*max(ax, ay) + 441. / 1024.*min(ax, ay);
}

void main() {
    using namespace std;

    _timeit::autoprint = false;
    long double exact = 0, approx = 0;
    _timeit::Stats results1 = timeit([&] { exact += distance(10., 100.); }),
        results2 = timeit([&] { approx += approx_distance(10., 100.); });

    cout << "Accuracy: " << (1. - abs((exact - approx) / exact)) * 100. << "%" << endl;
    cout << "Performance: " << fixed << setprecision(2) << (results1.min / results2.min)*100. << "%" << endl;
    getchar();
}