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
`[85.931ns,.., 87.701ns ± 2.594ns,.., 96.642ns ] (30 runs, 10000 loops each)` 

The standard output has the next format: [min,.., mean(average) ± standard_deviation,.., max ] (NUM_RUNS runs, NUM_LOOPS loops each).

You can also access the results and disable the default output.:
```cpp
_timeit::autoprint = false;
long double exact = 0, approx = 0;

_timeit::Stats results1 = timeit([&] { exact += distance(10., 100.); }),
_timeit::Stats results2 = timeit([&] { approx += approx_distance(10., 100.); });

cout << "Accuracy: " << (1. - abs((exact - approx) / exact)) * 100. << "%" << endl;
cout << "Performance: " << fixed << setprecision(2) << (results1.fast / results2.fast)*100. << "%" << endl;
```

```
Accuracy: 97.8629%
Performance: 204.77%
```

Please, have a look at [the source file](example2_manual_output/example2_manual_output.cpp) for the exact source code.

# Explanation
The `timeit` module runs the code in lambda several times. These times are called **loops**. Loops are grouped in **runs**. 

## Loops and runs
So, for instance, in the example above we need to measure the execution time of `pow(1,2);` code. 
Because it's a very fast code, the timer doesn't have enough resolution to measure such a piece of code. 
So, the idea is to run this code, for instance, 1000 times and to measure the total time. Let's suppose it's 1.5ms. Then, we need to divide this time by 1000.
Thus, in this example one run consists of 1000 iterations. In the end of one run we have time of this run: 1.5ms / 1000iterations = 1.5us (1.5 microseconds).

If you try to run such measurements several times, you'd notice that the result is almost always different: 1.5us, 1.4us, 1.6s, 1.5, us and so on. 
So, probably, the next thing you'd like to do, is to run the measurements several times, to get some picture about 'the real value'.
So, the next evolution of a benchmarking - is to make several runs instead of you and to represent the result nicely. 
The way it's done in the original Python's `timeit` is to use the format: mean ± [std.dev](https://en.wikipedia.org/wiki/Standard_deviation).

For instance, for the example above the output would be: 1.5us ± 70ns. But, for sure, we also need to include the information about the way we got this info. 
Thus the original `timeit` would print something like that: `1.5us ± 70ns per loop (mean ± std. dev. of 4 runs, 1000 loops each)`

## Differences with the original timeit
TODO: How EXACTLY many iterations?
TODO: the output format

# Features
1. Checks the granularity/resolution of the used timer and guesses the number of iterations based on that. So, it shouldn't use too much or too small amount of time.
2. The precision down to nanoseconds.
3. Should work well in a years-scale as well (though it hasn't been tested so heavily).
4. There are some unit-tests and they run during the start of the program. You have more chances to be warned about possible mistakes if they are (the project is tiny and young :).

# Roadmap
1. Find the best amount of repetitions
2. Rewrite the main function in macroses to be sure, that in the debug mode it also inlines the code under benchmarking.

# Open problems
1. How to avoid optimizing out the benchmarking loop in the release mode?
2. How to take into an account the cost of an empty loop (since it's optimized out)?