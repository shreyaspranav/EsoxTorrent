#pragma once

// Header files: ----------------------------
#include <assert.h>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <cstdint>
#include <string>
#include <memory>
#include <functional>
#include <cstdio>
// ------------------------------------------

#ifndef PRODUCTION
    // Assertions:
    #define ESOX_ASSERT(COND)                       assert(COND)
    #define ESOX_STATIC_ASSERT(COND, S)             static_assert(COND, S)
    
    // Logging: 
    #define __ESOX_LOG(loggerType, fmt, ...) \
        do { \
            printf("[%s] %s:%d: ", #loggerType, __FILE__, __LINE__); \
            printf(fmt, ##__VA_ARGS__); \
            printf("\n"); \
        } while (0)
    
    // Logger for different scenarios:
    #define ESOX_LOG_ERROR(fmt, ...)   __ESOX_LOG(ERROR, fmt, ##__VA_ARGS__)
    #define ESOX_LOG_WARN(fmt, ...)    __ESOX_LOG(WARNING, fmt, ##__VA_ARGS__)
    #define ESOX_LOG_INFO(fmt, ...)    __ESOX_LOG(INFO, fmt, ##__VA_ARGS__)
#else
    // Logger for different scenarios:
    #define ESOX_LOG_ERROR(fmt, ...)
    #define ESOX_LOG_WARN(fmt, ...) 
    #define ESOX_LOG_INFO(fmt, ...) 
#endif

// 'using' section -------------------------------------------------
using String = std::string;
using Size   = size_t;

template<typename T>
using Scope = std::unique_ptr<T>;

template<typename T>
using Ref = std::shared_ptr<T>;

template<typename A, typename B>
using Pair = std::pair<A, B>;

template<typename T>
using Vector = std::vector<T>;

template<typename K, typename V>
using Map = std::map<K, V>;

template<typename K, typename V>
using UnorderedMap = std::unordered_map<K, V>;
// -------------------------------------------------------------------