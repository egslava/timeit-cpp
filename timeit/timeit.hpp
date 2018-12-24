﻿#pragma once
#include <vector>
#include <chrono>
#include <thread>  // we need it for this_thread::sleep_for to warm up the code
#include <sstream>
#include <iomanip>
#include <assert.h>
#include <iostream>
#include <algorithm>
#include <valarray>  // see _fill_cache
#include <string>

namespace _timeit {
    bool autoprint = true;
    std::size_t cachesize = 1 * 1024;  // number of elements to use to 


    //namespace timeit {
    //#define timeit(code) {     \
        //        for(int i = 0; i < 10; i++){  \
//            code                      \
//        }                             \
//    }
//}

    using timer = std::chrono::high_resolution_clock;
    using IterType = long long;
    using RepType = long;

    template <typename Collection>
    std::string _join(const Collection &collection, const std::string &delimiter = ", ") {
        using namespace std;

        ostringstream out;
        out << collection[0];
        for (const auto &data : collection) {
            out << delimiter;
            out << data;
        }
        return out.str();
    }


    time_t _ns(const timer::duration &duration) {
        using namespace std;
        using namespace chrono;

        return duration_cast<nanoseconds>(duration).count();
    };


    /**
    The purpose of this function is to fullfill a CPU's cache with some bullshit.
    It's supposed to be used with 'cold start' benchmarks.

    In the beginning, I was thinking of implementation on macroses + inline assembler.
    So the plan was to use 'nop' instruction and with help of macroses, replicate it 256*1024 times.
    But, in this case:
    1. The debugger gets _really slow_.
    2. The inline assembler is not supported on x64-systems (in MS VS).
    3. The compile time also increases.

    So, instead, I just create an array of CACHE_SIZE elements and iterate over it.
    */
    void _fill_cache() {
        using namespace std;
        using namespace chrono;

        valarray<int> ints(cachesize);
        ints[rand() % (cachesize)] = 1;     // avoiding optimizing it out
        volatile auto result = ints.sum();  // again, don't optimize out
    }


    namespace _granularity {
        /** A minimum measurable time */
        time_t _granularity() {
            auto start = timer::now();
            auto end = timer::now();
            while (_ns(end - start) <= 0) {
                end = timer::now();
            };
            return _ns(end - start);
        }

        /** Get the minimal one between 50 calls. Just for reliablitity. */
        time_t _min_of_50_times() {
            time_t min_value = _granularity();
            for (int i = 0; i < 49; i++) {
                time_t value = _granularity();
                if (value < min_value) {
                    min_value = value;
                }
            }
            return min_value;
        }

        /** "memoized"/cached lazy-initialized value */
        time_t get() {
            static time_t _cache = -1;

            if (_cache != -1) {
                return _cache;
            }

            // believe it or not, but we have to 'warm up' the program this way, even if we run
            // granularity code about 50 times!!!
            // It seems, that, initially, the app is not in CPU cache and it runs the 
            // next code ~10-15 times slower, so I got the wrong granularity.
            // For some reasons, it's especially noticable in 'debug' configuration (in MSVS).
            // on my machine (Win 8 + MSVS) even 1 nanosecond looks ok, but I leave 1us just in case.
            std::this_thread::sleep_for(std::chrono::microseconds(1));

            _cache = _min_of_50_times();
            return _cache;
        }
    }

    template<typename T>
    time_t time(T code, IterType n_iterations = 0) {

        auto start = timer::now();
        for (IterType i = 0; i < n_iterations; i++) {
            code();
        }
        auto end = timer::now();
        return _ns(end - start);
    }

    /** 
    This function returns the optimal amount of iterations needed to perform benchmarks.
    - We can't run a code just once - the timer doesn't have enough resolution.
    - We can't run too many times (although Python's timeit does it) - because it gets too slow.

    Consider two time scales (in nanoseconds):


    Results from timer                                 0      300     600     900     1200    1500     1800    
    (if we constantly ask the timer about it)          ├───────┼───────┼───────┼───────┼───────┼────────┤
    The distance between two values I call                300     300     300     300     300     300     
    "timer resolution". Here - 300ns
    

    The actual code runtime (6 iterations):            0    200   400   600   800   1000  1200  
                                                       └─────┴─────┴─────┴─────┴─────┴─────┘
                                                         200   200   200   200   200   200

    
    0                               200                                 400                                 600                                 800   
    └────────────────────────────────┴───────────┬───────────────────────┴───────────────────────┬───────────┴───────────────────────────────────┼─────────────────────────────
    0                                           300                                             600                                             600
    */
    template<typename T>
    IterType num_best_iters(T code, IterType iterations) {
        if (iterations <= 0) {
            for (iterations = 1; iterations < 10000000000; iterations *= 10) {
                time_t time_to_perform = time(code, iterations);

                if (time_to_perform >= _granularity::get() * 1000) {
                    break;
                }
            }
        }
        return iterations;
    }


    struct Stats {
        struct DecomposedTime {
            const int ps, ns, us, ms,  // ps=picoseconds, us - microseconds, ms - milliseconds
                _s, mm, hh, dd;

            /** I use long double because I don't really know how people will use this class.
            Probably, some people will run it during several days. Probably, someone will run it during a year.
            We can't count picoseconds during one year in long-long: we need 64.77 bits for that.
            long-double just looses precision, but long-long just overflows and looses the data completely.
            For sure, I don't believe, someone will run this program for a year, but... */
            DecomposedTime(long double nanoseconds):
            ps{ static_cast<int>(fmodl(nanoseconds * 1000.L,  1000.L)) },
                ns{ static_cast<int>(fmodl(nanoseconds , 1000.L)) },
                us{ static_cast<int>(fmodl(nanoseconds / 1000.L, 1000.L)) },
                ms{ static_cast<int>(fmodl(nanoseconds / 1000000.L, 1000.L)) },
                _s{ static_cast<int>(fmodl(nanoseconds / 1000000000.L, 60.L)) },
                mm{ static_cast<int>(fmodl(nanoseconds / (1000000000.L * 60.L), 60.L)) },
                hh{ static_cast<int>(fmodl(nanoseconds / (1000000000.L * 60.L * 60.L), 24.L)) },
                dd{ static_cast<int>(nanoseconds / (1000000000.L * 60.L * 60.L * 24.L)) }
            {
                assert(nanoseconds > 0);
            }

            operator std::string() const {
                using namespace std;

                // != 0 is used instead of > 0, to ensure that I don't have negatives numbers. 
                // That's also the reason why I don't use unsigned types here
                ostringstream out;
                out << fixed;
                if (dd) {
                    out << dd << " days, "; // << hh << 'h';
                                            //return out.str();
                }

                if (hh || mm) {
                    if (hh) {
                        // 2:15:12
                        out << hh
                            << ':'
                            << setfill('0') << setw(2) << mm
                            << ':'
                            << setfill('0') << setw(2) << _s;
                        return out.str();
                    }

                    // 1:00.025
                    out << mm << ':' << setfill('0') << setw(2) << _s << '.' << setfill('0') << setw(3) << ms;
                    return out.str();
                }

                if (_s) return _fmt(out, _s, ms, "s");  // 1.025s
                if (ms) return _fmt(out, ms, us, "ms"); // 12.026ms
                if (us) return _fmt(out, us, ns, "us"); // 139.051us
                if (ns) return _fmt(out, ns, ps, "ns"); // 1.050ns
                out << ps << "ps";  // 570ps
                return out.str();
            }

            friend std::ostream& operator << (std::ostream &out, const DecomposedTime &time) {
                out << static_cast<std::string>(time);
                return out;
            }

        private:
            /** _fmt(out, 1, 23, "ms") -> "1.023ms" */
            static std::string _fmt(std::ostringstream &out, time_t integral, time_t fractional, const std::string &unit) {
                out << integral << '.' << std::setfill('0') << std::setw(3) << fractional << unit;
                return out.str();
            }
        };


        Stats(IterType iterations, RepType repetitions) :
            _n_iterations(iterations),
            _n_repetitions(repetitions)
        {};
        IterType    _n_iterations;
        RepType     _n_repetitions;
        std::vector<long double> _repetitions_ns;

        // output stats
        long double sum = 0;
        long double min = -1;

        long double mean() const noexcept {
            return this->sum / this->_n_repetitions;
        }

        void guard_all_results_are_known() const noexcept {
            assert(this->_n_repetitions == this->_repetitions_ns.size());
        }

        /** Calculates standard deviation (https://en.wikipedia.org/wiki/Standard_deviation) */
        long double std() const noexcept {
            guard_all_results_are_known();

            long double numerator = { 0 };
            long double x = { this->mean() };

            for (const auto &x_i : this->_repetitions_ns) {
                numerator += powl(x_i - x, 2.l);
            }

            // wtf n_repetitions if we can just _repetions_ns.size()? 
            // Answer: because _repetitions_ns.size() gives current amount of repetitions
            // and n_repetitions - planned amount
            return sqrtl(numerator / (_n_repetitions - 1));
        }

        long double max() const noexcept {
            guard_all_results_are_known();
            
            return *max_element(_repetitions_ns.cbegin(), _repetitions_ns.cend());
        }

        Stats& operator << (time_t nanos_per_repetition) {
            long double _ns_per_repetition = static_cast<long double>(nanos_per_repetition) / static_cast<long double>(_n_iterations);
            _repetitions_ns.push_back(_ns_per_repetition);

            sum += _ns_per_repetition;

            if (_ns_per_repetition < min || min < 0) {
                min = _ns_per_repetition;
            }

            return *this;
        }

        operator std::string () const {
            using namespace std;
            
            stringstream s;
            s << fixed << setprecision(2);
            s << "[" << DecomposedTime(min) << ",.., " << DecomposedTime(mean() ) << " " << char(241) << " "<< setw(5) << DecomposedTime(std()) << ",.., " << DecomposedTime(max()) <<  " ]" << " (" << _n_repetitions << " runs, " << _n_iterations << " loops each)";
            return s.str();
        }

        friend std::ostream& operator << (std::ostream &out, const Stats &stats) {
            out << static_cast<std::string>(stats);
            return out;
        }
    };


    template<class T>
    Stats timeit(T code, int repetitions = 30, IterType iterations = 0) {
        using namespace std;

        iterations = num_best_iters(code, iterations);

        Stats stats(iterations, repetitions);

        for (int i = 0; i < repetitions; i++) {
            stats << time(code, iterations);
        }

        if (autoprint) {
            cout << (string)stats << endl;
        }
        return stats;
    }

   
    namespace _tests {
        void case1_granularity() {
            assert(_granularity::get() > 0);
        }

        void case2_formatting() {
            using namespace std;
            Stats::DecomposedTime time(0.001);
            assert(time.ps == 1);
            assert(time.ns == 0);
            assert(time.us == 0);
            assert("1ps" == static_cast<string>(time));

            Stats::DecomposedTime time2(1.5);
            assert(time2.ps == 500);
            assert(time2.ns == 1);
            assert(time2.us == 0);
            assert("1.500ns" == static_cast<string>(time2));

            Stats::DecomposedTime time3(100000.5);
            assert(time3.ps == 500);
            assert(time3.ns == 0);
            assert(time3.us == 100);
            assert("100.000us" == static_cast<string>(time3));

            Stats::DecomposedTime time4(20.5 + 50000 + 2000000);
            assert(time4.ps == 500);
            assert(time4.ns == 20);
            assert(time4.us == 50);
            assert(time4.ms == 2);
            assert("2.050ms" == static_cast<string>(time4));

            constexpr long _s = 1000000000;
            constexpr long ms = 1000000;
            Stats::DecomposedTime time5(20.5 + 50000. + 3.*_s + 450.*ms);
            assert(time5.ps == 500);
            assert(time5.ns == 20);
            assert(time5.us == 50);
            assert(time5.ms == 450);
            assert(time5._s == 3);
            string st{ time5 };
            assert("3.450s" == static_cast<string>(time5));

            constexpr long double mm = _s * 60.L;
            Stats::DecomposedTime time6(20.5 + 50000. + 3.*_s + 450.*ms + 5 * mm);
            assert(time6.ps == 500);
            assert(time6.ns == 20);
            assert(time6.us == 50);
            assert(time6.ms == 450);
            assert(time6._s == 3);
            assert(time6.mm == 5);
            assert("5:03.450" == static_cast<string>(time6));

            constexpr long double hh = mm * 60.L;
            Stats::DecomposedTime time7(20.5 + 50000. + 3.*_s + 450.*ms + 5 * mm + 7 * hh);
            assert(time7.ps == 500);
            assert(time7.ns == 20);
            assert(time7.us == 50);
            assert(time7.ms == 450);
            assert(time7._s == 3);
            assert(time7.mm == 5);
            assert(time7.hh == 7);
            st = static_cast<string>(time7);
            assert("7:05:03" == static_cast<string>(time7));

            constexpr long double days = 24.L * hh;
            Stats::DecomposedTime time8(20.5 + 50000. + 3.*_s + 450.*ms + 5 * mm + 7 * hh + 2.L*days);

            // long double already doesn't have enough precision, and gives me 512 picoseconds instead of 500
            //assert(time8.ps == 500);  
            assert(time8.ns == 20);
            assert(time8.us == 50);
            assert(time8.ms == 450);
            assert(time8._s == 3);
            assert(time8.mm == 5);
            assert(time8.hh == 7);
            assert(time8.dd == 2);
            assert("2 days, 7:05:03" == static_cast<string>(time8));
        }

        int run() {
            case1_granularity();
            case2_formatting();
            return 0;
        }

        int _run_tests = run();
    }
}

using _timeit::timeit;