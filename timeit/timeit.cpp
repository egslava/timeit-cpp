﻿// timeit.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "timeit.hpp"
#include <iostream>

int main()
{
    using namespace std;

    cout << timeit::timeit([] {
        volatile register double a = pow(1, 2);
        volatile register double b = pow(3, 4);
        volatile register double c = pow(5, 6);
        volatile register double d = pow(7, 8);
        volatile register double e = pow(9, 10);
        volatile register double f = pow(11, 12);
        volatile register double g = pow(13, 14);
        volatile register double h = pow(15, 16);
        volatile register double i = pow(17, 18);
    });

	return 0;
}