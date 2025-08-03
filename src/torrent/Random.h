#pragma once
#include "Base.h"

#include <random>

namespace Esox
{
    const char alphaNumericChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    struct RandomState
    {
        std::mt19937 mt;
        std::uniform_int_distribution<uint32_t> alphaNumericIdxDist;
        std::uniform_int_distribution<uint32_t> integerDist;
        RandomState()
        {
            mt = std::mt19937(std::random_device{}());
            alphaNumericIdxDist = std::uniform_int_distribution<uint32_t>(0, 26 * 2 + 9);
            integerDist = std::uniform_int_distribution<uint32_t>(0, UINT32_MAX);
        }
    };
    static RandomState randomState;

    class Random
    {
    public:
        static char AlphaNumeric()
        {
            return alphaNumericChars[randomState.alphaNumericIdxDist(randomState.mt)];
        }

        static uint32_t Integer()
        {
            return randomState.integerDist(randomState.mt);
        }
    };
}
