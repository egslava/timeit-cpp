# timeit-cpp
Python's `timeit` module, rewritten C++11. Thus, it's a simple, header-only benchmarking library for quick experiments. Probably, not only :)

# Usage
Download [timeit.hpp](timeit/timeit.hpp).
The minimal [example](example1_simple/example1_simple.cpp) is:
```cpp
#include "timeit.hpp"

void main() {
    timeit([] { 
    	pow(1, 2);
    });
}
```
`min: 85.780ns, mean: 87.365ns (3 runs, 10000 loops each)` 

You can also access the results and disable the default output.:
```cpp
_timeit::autoprint = false;
long double exact = 0, approx = 0;

_timeit::Stats results1 = timeit([&] { exact += distance(10., 100.); }),
_timeit::Stats results2 = timeit([&] { approx += approx_distance(10., 100.); });

cout << "Accuracy: " << (1. - abs((exact - approx) / exact)) * 100. << "%" << endl;
cout << "Performance: " << fixed << setprecision(2) << (results1.fast / results2.fast)*100. << "%" << endl;
```
<<<<<<< HEAD

=======
Accuracy: 97.8629%
Performance: 204.77%
>>>>>>> 24b70002aa0aeab01965784e2d4a5ee46c0d69cf
```
Accuracy: 97.8629%
Performance: 204.77%
```

Please, have a look at [the source file](example2_manual_output/example2_manual_output.cpp) for the exact source code.

Please, have a look at [the source file](example2_manual_output/example2_manual_output.cpp) for the exact source code.

# Features
1. Checks the granularity/resolution of the used timer and guesses the number of iterations based on that. So, it shouldn't use too much or too small amount of time.
2. The precision down to nanoseconds.
3. Should work well in a years-scale as well (though it hasn't been tested so heavily).
4. There are some unit-tests and they run during the start of the program. You have more chances to be warned about possible mistakes if they are (the project is tiny and young :).

# Roadmap
1. Find the best amount of repetitions
2. Show standard deviation as well
3. Rewrite the main function in macroses to be sure, that in the debug mode it also inlines the code under benchmarking.

# Open problems
1. How to avoid optimizing out the benchmarking loop in the release mode?
2. How to take into an account the cost of an empty loop (since it's optimized out)?