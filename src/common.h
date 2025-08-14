#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <exception>

#include <stdlib.h>

#include <vector>
#include <list>

#include <memory.h>

typedef unsigned char byte;

#define ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))

#define Con_Printf printf

template <class T> inline void ClearPointersVector(T &vec)
{
    for (auto &it : vec)
        delete it;

    vec.clear();
    vec.shrink_to_fit();
}

template <class T> inline void FreeVector(T &vec)
{
    vec.clear();
    vec.shrink_to_fit();
}

template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

#ifdef _WIN32
    #define BREAKPOINT() __debugbreak()
#elif defined(__linux__) || defined(__APPLE__)
    #if defined(__GNUC__) || defined(__clang__)
        #define BREAKPOINT() __builtin_trap()
    #else
        #include <signal.h>
        #define BREAKPOINT() raise(SIGTRAP)
    #endif
#else
    #error "Unsupported platform"
#endif