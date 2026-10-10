/// @file titan/utils/runes.h
/// @brief Element quantities supplied by inventory runes, carried pouches, and equipped providers.

#pragma once

#include "../client.h"
#include "../inventory_id.h"
#include "../rune_element.h"
#include "../rune_element_equipment.h"
#include "rune_pouch.h"

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

inline RuneElementAmounts getAvailableRuneElementAmountsFromCarriedRunePouch(
        std::span<const ItemContainerSlot> inventoryItems,
        std::span<const ItemContainerSlot> pouchContents, bool membersWorld) {
    if (!membersWorld) return {};
    const auto type = RunePouch::getCarriedRunePouchType(inventoryItems);
    if (!type) return {};
    const int32_t slotCount = *type == RunePouchType::DIVINE ? 4 : 3;
    RunePouchContents available;
    for (const auto& item : pouchContents) {
        if (item.slot >= 0 && item.slot < slotCount) available.push_back(item);
    }
    return getAvailableRuneElementAmountsFromInventoryRunes(available, membersWorld);
}

inline RuneElementAmounts getCarriedRuneElementAmounts(
        std::span<const ItemContainerSlot> inventoryItems,
        std::span<const ItemContainerSlot> equippedItems,
        std::span<const ItemContainerSlot> pouchContents, bool membersWorld) {
    auto result = getAvailableRuneElementAmountsFromInventoryRunes(inventoryItems, membersWorld);
    for (const auto& [element, amount] : getAvailableRuneElementAmountsFromCarriedRunePouch(inventoryItems, pouchContents, membersWorld)) {
        auto& total = result[element];
        total += (std::min)(amount, kInfiniteRuneAmount - 1 - total);
    }
    for (const auto& [element, amount] : getAvailableRuneElementAmountsFromEquippedItems(equippedItems, membersWorld)) {
        result[element] = amount;
    }
    return result;
}

/// Without a supplied pouch snapshot, only loose inventory runes and equipped providers contribute.
inline RuneElementAmounts getCarriedRuneElementAmounts(
        std::span<const ItemContainerSlot> inventoryItems,
        std::span<const ItemContainerSlot> equippedItems, bool membersWorld) {
    return getCarriedRuneElementAmounts(inventoryItems, equippedItems, {}, membersWorld);
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

/// Pouch contents inherit the varbit reader's inability to distinguish unreadable values from zero.
inline std::optional<RuneElementAmounts> getAvailableRuneElementAmountsFromCarriedRunePouch() {
    const auto membersWorld = detail::currentWorldIsMembers();
    if (!membersWorld) return std::nullopt;
    if (!*membersWorld) return RuneElementAmounts{};
    const auto inventory = state::itemContainer(static_cast<int32_t>(InventoryID::INVENTORY));
    if (!inventory) return std::nullopt;
    if (!RunePouch::getCarriedRunePouchType(inventory->items)) return RuneElementAmounts{};
    return getAvailableRuneElementAmountsFromCarriedRunePouch(inventory->items, RunePouch::getRunePouchContents(), true);
}

/// Includes carried pouch contents; unreadable pouch varbits may appear empty.
inline std::optional<RuneElementAmounts> getCarriedRuneElementAmounts() {
    const auto membersWorld = detail::currentWorldIsMembers();
    if (!membersWorld) return std::nullopt;
    const auto inventory = state::itemContainer(static_cast<int32_t>(InventoryID::INVENTORY));
    const auto equipment = state::itemContainer(static_cast<int32_t>(InventoryID::EQUIPMENT));
    if (!inventory || !equipment) return std::nullopt;
    const auto pouch = *membersWorld && RunePouch::getCarriedRunePouchType(inventory->items)
        ? RunePouch::getRunePouchContents() : RunePouchContents{};
    return getCarriedRuneElementAmounts(inventory->items, equipment->items, pouch, *membersWorld);
}

}  // namespace titan::utils::Runes
