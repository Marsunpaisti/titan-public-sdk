#pragma once

#include <cstdlib>
#include <iostream>

// Exit normally on failure so Windows does not launch its assertion/debugger dialogs.
#define CHECK(condition) do { \
    if (!(condition)) { \
        std::cerr << __FILE__ << ':' << __LINE__ << ": failed: " #condition << '\n'; \
        std::exit(EXIT_FAILURE); \
    } \
} while (false)
