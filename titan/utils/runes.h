/// @file titan/utils/runes.h
/// @brief Element quantities supplied by inventory runes and equipped rune providers.

#pragma once

#include "../client.h"
#include "../inventory_id.h"
#include "../rune_element.h"
#include "../rune_element_equipment.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <map>
#include <optional>
#include <span>

namespace titan::utils::Runes {

inline constexpr int64_t kInfiniteRuneAmount = (std::numeric_limits<int64_t>::max)();

/// Missing elements have zero supply. Combination-rune totals overlap and are not independent budgets.
using RuneElementAmounts = std::map<RuneElement, int64_t>;

inline RuneElementAmounts getAvailableRuneElementAmountsFromInventoryRunes(
        std::span<const ItemContainerSlot> inventoryItems, bool membersWorld) {
    RuneElementAmounts result;
    for (const auto& item : inventoryItems) {
        if (item.quantity <= 0) continue;
        for (const auto& rune : RuneElementInfo::kRunes) {
            if (rune.itemId != item.itemId || (rune.membersOnly && !membersWorld)) continue;
            for (auto element : rune.elements) {
                auto& amount = result[element];
                // Finite stacks must never overflow or become the infinity sentinel.
                amount += (std::min)(int64_t{item.quantity}, kInfiniteRuneAmount - 1 - amount);
            }
            break;
        }
    }
    return result;
}

inline RuneElementAmounts getAvailableRuneElementAmountsFromEquippedItems(
        std::span<const ItemContainerSlot> equippedItems, bool membersWorld) {
    RuneElementAmounts result;
    for (const auto& item : equippedItems) {
        if (item.quantity <= 0) continue;
        for (const auto& equipment : RuneElementEquipmentInfo::kEquipment) {
            if (equipment.itemId != item.itemId || item.slot != static_cast<int32_t>(equipment.slot)
                || (equipment.membersOnly && !membersWorld)) continue;
            for (auto element : equipment.elements) result[element] = kInfiniteRuneAmount;
            break;
        }
    }
    return result;
}

/// Rune-pouch contents are excluded; only loose inventory runes and equipped providers contribute.
inline RuneElementAmounts getCarriedRuneElementAmounts(
        std::span<const ItemContainerSlot> inventoryItems,
        std::span<const ItemContainerSlot> equippedItems, bool membersWorld) {
    auto result = getAvailableRuneElementAmountsFromInventoryRunes(inventoryItems, membersWorld);
    for (const auto& [element, amount] : getAvailableRuneElementAmountsFromEquippedItems(equippedItems, membersWorld)) {
        result[element] = amount;
    }
    return result;
}

namespace detail {

inline std::optional<bool> currentWorldIsMembers() {
    const auto currentWorld = state::world::current();
    if (!currentWorld) return std::nullopt;
    for (const auto& world : state::world::list()) {
        if (world.id == *currentWorld) return world.isMembers();
    }
    return std::nullopt;
}

}  // namespace detail

/// Live overloads return nullopt when world membership or a required container is unavailable.
inline std::optional<RuneElementAmounts> getAvailableRuneElementAmountsFromInventoryRunes() {
    const auto membersWorld = detail::currentWorldIsMembers();
    if (!membersWorld) return std::nullopt;
    const auto inventory = state::itemContainer(static_cast<int32_t>(InventoryID::INVENTORY));
    if (!inventory) return std::nullopt;
    return getAvailableRuneElementAmountsFromInventoryRunes(inventory->items, *membersWorld);
}

inline std::optional<RuneElementAmounts> getAvailableRuneElementAmountsFromEquippedItems() {
    const auto membersWorld = detail::currentWorldIsMembers();
    if (!membersWorld) return std::nullopt;
    const auto equipment = state::itemContainer(static_cast<int32_t>(InventoryID::EQUIPMENT));
    if (!equipment) return std::nullopt;
    return getAvailableRuneElementAmountsFromEquippedItems(equipment->items, *membersWorld);
}

inline std::optional<RuneElementAmounts> getCarriedRuneElementAmounts() {
    const auto membersWorld = detail::currentWorldIsMembers();
    if (!membersWorld) return std::nullopt;
    const auto inventory = state::itemContainer(static_cast<int32_t>(InventoryID::INVENTORY));
    const auto equipment = state::itemContainer(static_cast<int32_t>(InventoryID::EQUIPMENT));
    if (!inventory || !equipment) return std::nullopt;
    return getCarriedRuneElementAmounts(inventory->items, equipment->items, *membersWorld);
}

}  // namespace titan::utils::Runes
