/// @file titan/utils/rune_pouch.h
/// @brief Rune-pouch possession and contents, independent of world membership.

#pragma once

#include "../generated/gamevals/item_id.h"
#include "../generated/gamevals/varbit_id.h"
#include "../rune_pouch.h"
#include "bank.h"

#include <array>
#include <optional>
#include <span>

namespace titan::utils::RunePouch {

namespace detail {

// TODO: Read cache enum 982 when runtime enum access is available in the SDK.
inline constexpr std::array kRuneIds{
    gamevals::ItemID::AIRRUNE, gamevals::ItemID::WATERRUNE, gamevals::ItemID::EARTHRUNE,
    gamevals::ItemID::FIRERUNE, gamevals::ItemID::MINDRUNE, gamevals::ItemID::CHAOSRUNE,
    gamevals::ItemID::DEATHRUNE, gamevals::ItemID::BLOODRUNE, gamevals::ItemID::COSMICRUNE,
    gamevals::ItemID::NATURERUNE, gamevals::ItemID::LAWRUNE, gamevals::ItemID::BODYRUNE,
    gamevals::ItemID::SOULRUNE, gamevals::ItemID::ASTRALRUNE, gamevals::ItemID::MISTRUNE,
    gamevals::ItemID::MUDRUNE, gamevals::ItemID::DUSTRUNE, gamevals::ItemID::LAVARUNE,
    gamevals::ItemID::STEAMRUNE, gamevals::ItemID::SMOKERUNE, gamevals::ItemID::WRATHRUNE,
    gamevals::ItemID::SUNFIRERUNE, gamevals::ItemID::AETHERRUNE,
};

inline constexpr std::array kTypeVarbits{
    gamevals::VarbitID::RUNE_POUCH_TYPE_1, gamevals::VarbitID::RUNE_POUCH_TYPE_2,
    gamevals::VarbitID::RUNE_POUCH_TYPE_3, gamevals::VarbitID::RUNE_POUCH_TYPE_4,
};
inline constexpr std::array kQuantityVarbits{
    gamevals::VarbitID::RUNE_POUCH_QUANTITY_1, gamevals::VarbitID::RUNE_POUCH_QUANTITY_2,
    gamevals::VarbitID::RUNE_POUCH_QUANTITY_3, gamevals::VarbitID::RUNE_POUCH_QUANTITY_4,
};

}  // namespace detail

/// Divine wins if supplied snapshots contain both types; locked variants retain their capacity.
inline std::optional<RunePouchType> getCarriedRunePouchType(std::span<const ItemContainerSlot> inventoryItems) {
    std::optional<RunePouchType> result;
    for (const auto& item : inventoryItems) {
        if (item.quantity <= 0) continue;
        switch (item.itemId) {
        case gamevals::ItemID::DIVINE_RUNE_POUCH:
        case gamevals::ItemID::DIVINE_RUNE_POUCH_TROUVER:
            return RunePouchType::DIVINE;
        case gamevals::ItemID::BH_RUNE_POUCH:
        case gamevals::ItemID::BH_RUNE_POUCH_TROUVER:
            result = RunePouchType::REGULAR;
            break;
        }
    }
    return result;
}

/// Nullopt means no pouch detected, including when inventory is unavailable.
inline std::optional<RunePouchType> getCarriedRunePouchType() {
    const auto inventory = state::itemContainer(static_cast<int32_t>(InventoryID::INVENTORY));
    return inventory ? getCarriedRunePouchType(inventory->items) : std::nullopt;
}

inline bool accountOwnsRunePouch(std::span<const ItemContainerSlot> inventoryItems,
                                std::span<const ItemContainerSlot> bankItems) {
    return getCarriedRunePouchType(inventoryItems).has_value() || getCarriedRunePouchType(bankItems).has_value();
}

/// False requires known inventory and bank contents; nullopt means ownership is unknown.
/// Remembered bank contents are advisory and scoped to the current account by the host.
inline std::optional<bool> accountOwnsRunePouch() {
    const auto inventory = state::itemContainer(static_cast<int32_t>(InventoryID::INVENTORY));
    if (inventory && getCarriedRunePouchType(inventory->items)) return true;

    // Closed native bank containers can survive a login; only the account-aware cache is safe then.
    if (Bank::isOpen()) {
        if (const auto bank = state::itemContainer(static_cast<int32_t>(InventoryID::BANK))) {
            if (getCarriedRunePouchType(bank->items)) return true;
            return inventory ? std::optional<bool>{false} : std::nullopt;
        }
    }
    const auto bank = state::itemCache().bank();
    if (!bank || !bank->known || bank->loading) return std::nullopt;
    if (getCarriedRunePouchType(bank->items)) return true;
    return inventory ? std::optional<bool>{false} : std::nullopt;
}

/// Stored contents do not prove possession or usability. Empty and unknown rune types are omitted.
/// The SDK's varbit reader has no failure flag, so unreadable values can appear empty.
inline RunePouchContents getRunePouchContents() {
    RunePouchContents result;
    // shortcut: only normal three/four-slot pouches are modeled, extend for special-mode extra slots.
    for (int32_t slot = 0; slot < static_cast<int32_t>(detail::kTypeVarbits.size()); ++slot) {
        const auto type = state::vars().varbit(detail::kTypeVarbits[slot]);
        const auto quantity = state::vars().varbit(detail::kQuantityVarbits[slot]);
        if (type <= 0 || type > static_cast<int32_t>(detail::kRuneIds.size()) || quantity <= 0) continue;
        result.push_back({slot, detail::kRuneIds[type - 1], quantity});
    }
    return result;
}

}  // namespace titan::utils::RunePouch
