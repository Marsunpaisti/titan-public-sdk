/// @file titan/rune_pouch.h
/// @brief Rune-pouch capacity and decoded contents.

#pragma once

#include "client.h"

#include <vector>

namespace titan {

enum class RunePouchType {
    REGULAR,
    DIVINE,
};

/// Occupied slots only, retaining zero-based pouch slot indices rather than inventory indices.
using RunePouchContents = std::vector<ItemContainerSlot>;

}  // namespace titan
