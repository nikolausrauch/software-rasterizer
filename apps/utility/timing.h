#pragma once

#include <chrono>
#include <iostream>

namespace profile
{

typedef std::conditional<std::chrono::high_resolution_clock::is_steady, std::chrono::high_resolution_clock, std::chrono::steady_clock >::type  HighResClock;
typedef HighResClock::time_point    TimePoint;
typedef HighResClock::duration      Time;


typedef std::chrono::nanoseconds    NanoSeconds;
typedef std::chrono::microseconds   MicroSeconds;
typedef std::chrono::milliseconds   MilliSeconds;
typedef std::chrono::seconds        Seconds;

template <typename T>
double time_cast(const Time& time)
{
    return std::chrono::duration_cast<T>(time).count();
}

template<>
double time_cast<NanoSeconds>(const Time& t)
{
    return std::chrono::duration_cast<NanoSeconds>(t).count();
}

template<>
double time_cast<MicroSeconds>(const Time& t)
{
    return std::chrono::duration_cast<MicroSeconds>(t).count();
}

template<>
double time_cast<MilliSeconds>(const Time& t)
{
    return std::chrono::duration<double, std::ratio<1, 1000>>(t).count();
}

template<>
double time_cast<Seconds>(const Time& t)
{
    return std::chrono::duration<double, std::ratio<1, 1000000>>(t).count();
}

}

#define TIME_MS(func) { auto start = profile::HighResClock::now(); func; auto end = profile::HighResClock::now(); std::cout << profile::time_cast<profile::MilliSeconds>(end - start) << std::endl; }
#define TIME_NS(func) { auto start = profile::HighResClock::now(); func; auto end = profile::HighResClock::now(); std::cout << profile::time_cast<profile::NanoSeconds>(end - start) << std::endl; }
