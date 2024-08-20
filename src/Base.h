#pragma once

#include <assert.h>
#include <iostream>

#define ENOX_ASSERT(COND)						assert(COND)
#define ENOX_STATIC_ASSERT(COND, S)				static_assert(COND, S)

#define ENOX_LOG_ERROR(...)						std::cout << __VA_ARGS__ << "\n"
