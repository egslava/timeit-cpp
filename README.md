# timeit-cpp
Python's `timeit` module, rewritten C++11. Thus, it's a simple, header-only benchmarking library for quick experiments. Probably, not only :)

# Usage
The minimal example is:
```cpp
#include "timeit.hpp"

void main() {
    timeit([] { 
    	pow(1, 2);
    });
}
```
`min: 86.412ns, mean: 86.753ns(mean +- std.dev.of 3 runs, 10000 loops each)` 

You can also access the results:
```cpp
_timeit::Stats results = timeit([] { pow(2, 4); });

cout << results << endl;
cout << "avg: " << results.avg << endl;
cout << "min: " << results.min_result << endl;
```

```
min: 86.864ns, mean: 102.640ns(mean +- std.dev.of 3 runs, 10000 loops each)

min: 86.864ns, mean: 102.640ns(mean +- std.dev.of 3 runs, 10000 loops each)

avg: 307.921
min: 86.864
```

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