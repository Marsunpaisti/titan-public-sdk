/// @file titan/rune_pouch.h
/// @brief Rune-pouch capacity and decoded contents.

#pragma once

#include "client.h"

#include <vector>

namespace titan {

/// Normal pouch capacities; locked variants retain their type and special-mode extra slots are not modeled.
enum class RunePouchType {
    REGULAR, ///< Three usable slots, indexed 0 through 2.
    DIVINE,  ///< Four usable slots, indexed 0 through 3.
};

/// Occupied slots only, retaining zero-based pouch slot indices rather than inventory indices.
/// Each entry holds a decoded rune item ID and positive quantity; sparse slots keep their original index.
/// Stored contents can outlive possession and do not establish that the runes are currently usable.
using RunePouchContents = std::vector<ItemContainerSlot>;

}  // namespace titan
