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

/// Element-supply snapshots, not independent spell budgets or mutable consumption counters.
/// Supplied-item overloads do not read the SDK; pass each occupied source slot once.
/// membersWorld describes the world, not the account's subscription status.
namespace titan::utils::Runes {

/// Sentinel for equipment-provided infinite supply; ordinary available >= required comparisons work.
/// Arithmetic must preserve infinity explicitly: adding can overflow and subtracting loses the sentinel.
inline constexpr int64_t kInfiniteRuneAmount = (std::numeric_limits<int64_t>::max)();

/// Missing elements have zero supply. Combination-rune totals overlap and are not independent budgets.
/// Owned map of positive finite quantities or kInfiniteRuneAmount; finite sums saturate at one below infinity.
using RuneElementAmounts = std::map<RuneElement, int64_t>;

/// Counts loose rune stacks, crediting the full stack quantity to each represented element.
/// Ignores unknown items, nonpositive quantities, providers, and pouch contents; inventory slot indices are unused.
/// When membersWorld is false, members-only source runes are excluded even if their elements are F2P-usable.
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

/// Returns kInfiniteRuneAmount for each element supplied by eligible equipped providers.
/// Item slot indices must match EquipmentSlot values and the provider's required slot; quantity must be positive.
/// Unknown IDs and empty tomes are ignored; members-only providers are excluded when membersWorld is false.
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

/// Counts supplied pouch contents only when inventory contains a supported pouch and membersWorld is true.
/// pouchContents must use decoded rune item IDs and original zero-based pouch slot indices, not inventory indices.
/// Regular pouches accept slots 0-2, divine 0-3; divine wins if both types are carried and contents count only once.
/// Out-of-range slots, unknown rune IDs, and nonpositive quantities are ignored; combinations credit each element.
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

/// Combines supplied loose runes and usable pouch contents, then overrides elements supplied by equipment with infinity.
/// Equipment entries use EquipmentSlot indices; pouch entries use original zero-based pouch slot indices.
/// membersWorld filters member-only sources, including the entire pouch; finite totals saturate below infinity.
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
/// Performs no SDK reads; membersWorld filters source eligibility and equipped providers override finite totals.
inline RuneElementAmounts getCarriedRuneElementAmounts(
        std::span<const ItemContainerSlot> inventoryItems,
        std::span<const ItemContainerSlot> equippedItems, bool membersWorld) {
    return getCarriedRuneElementAmounts(inventoryItems, equippedItems, {}, membersWorld);
}

namespace detail {

// A missing current-world list entry is unknown, not evidence of an F2P world.
inline std::optional<bool> currentWorldIsMembers() {
    const auto currentWorld = state::world::current();
    if (!currentWorld) return std::nullopt;
    for (const auto& world : state::world::list()) {
        if (world.id == *currentWorld) return world.isMembers();
    }
    return std::nullopt;
}

}  // namespace detail

/// Counts loose runes using the SDK's current inventory and world membership; pouch supplies are excluded.
/// Returns nullopt when world membership or inventory is unavailable; a readable inventory with no eligible runes is empty.
inline std::optional<RuneElementAmounts> getAvailableRuneElementAmountsFromInventoryRunes() {
    const auto membersWorld = detail::currentWorldIsMembers();
    if (!membersWorld) return std::nullopt;
    const auto inventory = state::itemContainer(static_cast<int32_t>(InventoryID::INVENTORY));
    if (!inventory) return std::nullopt;
    return getAvailableRuneElementAmountsFromInventoryRunes(inventory->items, *membersWorld);
}

/// Resolves current equipment and world membership through the SDK, returning infinite amounts for eligible providers.
/// Returns nullopt when world membership or equipment is unavailable; no eligible providers produces an empty map.
inline std::optional<RuneElementAmounts> getAvailableRuneElementAmountsFromEquippedItems() {
    const auto membersWorld = detail::currentWorldIsMembers();
    if (!membersWorld) return std::nullopt;
    const auto equipment = state::itemContainer(static_cast<int32_t>(InventoryID::EQUIPMENT));
    if (!equipment) return std::nullopt;
    return getAvailableRuneElementAmountsFromEquippedItems(equipment->items, *membersWorld);
}

/// Resolves current inventory, pouch varbits, and world membership through the SDK.
/// A known F2P world returns an empty map without requiring inventory; a readable inventory without a pouch is also empty.
/// Returns nullopt for unknown world membership, or unavailable inventory on a members world.
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
/// Reads current inventory, equipment, world membership, and eligible pouch contents through the SDK.
/// Returns nullopt if membership, inventory, or equipment is unavailable; known absence of supplies gives an empty map.
/// Eligible equipment overrides finite totals with infinity. Sequential SDK reads are not an atomic snapshot.
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
