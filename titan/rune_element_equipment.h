/// @file titan/rune_element_equipment.h
/// @brief Equipment rune-provider catalog, independent of live equipment and world state.

#pragma once

#include "equipment_slot.h"
#include "rune_element.h"

#include <array>
#include <cstdint>
#include <span>

namespace titan {
namespace RuneElementEquipmentInfo {

namespace detail {
using RuneElementInfo::detail::kProvidedElements;
}

struct EquipmentDefinition {
    int32_t itemId;
    EquipmentSlot slot;
    std::span<const RuneElement> elements;
    bool membersOnly = true;
};

// TODO: Read weapon mappings once from cache enums 988/989/996/997; the SDK lacks runtime enum access.
// shortcut: revision-241 weapon data can miss new equipment, refresh until runtime enum access is available.
inline constexpr std::array kEquipment = {
    EquipmentDefinition{gamevals::ItemID::STAFF_OF_AIR, EquipmentSlot::WEAPON, detail::kProvidedElements<RuneElement::AIR>, false},
    EquipmentDefinition{gamevals::ItemID::AIR_BATTLESTAFF, EquipmentSlot::WEAPON, detail::kProvidedElements<RuneElement::AIR>},
    EquipmentDefinition{gamevals::ItemID::MYSTIC_AIR_STAFF, EquipmentSlot::WEAPON, detail::kProvidedElements<RuneElement::AIR>},
    EquipmentDefinition{gamevals::ItemID::BR_AIR_STAFF, EquipmentSlot::WEAPON, detail::kProvidedElements<RuneElement::AIR>, false},
    EquipmentDefinition{gamevals::ItemID::STAFF_OF_WATER, EquipmentSlot::WEAPON, detail::kProvidedElements<RuneElement::WATER>, false},
    EquipmentDefinition{gamevals::ItemID::WATER_BATTLESTAFF, EquipmentSlot::WEAPON, detail::kProvidedElements<RuneElement::WATER>},
    EquipmentDefinition{gamevals::ItemID::MYSTIC_WATER_STAFF, EquipmentSlot::WEAPON, detail::kProvidedElements<RuneElement::WATER>},
    EquipmentDefinition{gamevals::ItemID::KODAI_WAND, EquipmentSlot::WEAPON, detail::kProvidedElements<RuneElement::WATER>},
    EquipmentDefinition{gamevals::ItemID::STAFF_OF_EARTH, EquipmentSlot::WEAPON, detail::kProvidedElements<RuneElement::EARTH>, false},
    EquipmentDefinition{gamevals::ItemID::EARTH_BATTLESTAFF, EquipmentSlot::WEAPON, detail::kProvidedElements<RuneElement::EARTH>},
    EquipmentDefinition{gamevals::ItemID::MYSTIC_EARTH_STAFF, EquipmentSlot::WEAPON, detail::kProvidedElements<RuneElement::EARTH>},
    EquipmentDefinition{gamevals::ItemID::STAFF_OF_FIRE, EquipmentSlot::WEAPON, detail::kProvidedElements<RuneElement::FIRE>, false},
    EquipmentDefinition{gamevals::ItemID::FIRE_BATTLESTAFF, EquipmentSlot::WEAPON, detail::kProvidedElements<RuneElement::FIRE>},
    EquipmentDefinition{gamevals::ItemID::MYSTIC_FIRE_STAFF, EquipmentSlot::WEAPON, detail::kProvidedElements<RuneElement::FIRE>},
    EquipmentDefinition{gamevals::ItemID::MIST_BATTLESTAFF, EquipmentSlot::WEAPON,
        detail::kProvidedElements<RuneElement::AIR, RuneElement::WATER>},
    EquipmentDefinition{gamevals::ItemID::MYSTIC_MIST_BATTLESTAFF, EquipmentSlot::WEAPON,
        detail::kProvidedElements<RuneElement::AIR, RuneElement::WATER>},
    EquipmentDefinition{gamevals::ItemID::DUST_BATTLESTAFF, EquipmentSlot::WEAPON,
        detail::kProvidedElements<RuneElement::AIR, RuneElement::EARTH>},
    EquipmentDefinition{gamevals::ItemID::MYSTIC_DUST_BATTLESTAFF, EquipmentSlot::WEAPON,
        detail::kProvidedElements<RuneElement::AIR, RuneElement::EARTH>},
    EquipmentDefinition{gamevals::ItemID::SMOKE_BATTLESTAFF, EquipmentSlot::WEAPON,
        detail::kProvidedElements<RuneElement::AIR, RuneElement::FIRE>},
    EquipmentDefinition{gamevals::ItemID::MYSTIC_SMOKE_BATTLESTAFF, EquipmentSlot::WEAPON,
        detail::kProvidedElements<RuneElement::AIR, RuneElement::FIRE>},
    EquipmentDefinition{gamevals::ItemID::MUD_BATTLESTAFF, EquipmentSlot::WEAPON,
        detail::kProvidedElements<RuneElement::WATER, RuneElement::EARTH>},
    EquipmentDefinition{gamevals::ItemID::MYSTIC_MUD_STAFF, EquipmentSlot::WEAPON,
        detail::kProvidedElements<RuneElement::WATER, RuneElement::EARTH>},
    EquipmentDefinition{gamevals::ItemID::STEAM_BATTLESTAFF, EquipmentSlot::WEAPON,
        detail::kProvidedElements<RuneElement::WATER, RuneElement::FIRE>},
    EquipmentDefinition{gamevals::ItemID::MYSTIC_STEAM_BATTLESTAFF, EquipmentSlot::WEAPON,
        detail::kProvidedElements<RuneElement::WATER, RuneElement::FIRE>},
    EquipmentDefinition{gamevals::ItemID::STEAM_BATTLESTAFF_PRETTY, EquipmentSlot::WEAPON,
        detail::kProvidedElements<RuneElement::WATER, RuneElement::FIRE>},
    EquipmentDefinition{gamevals::ItemID::MYSTIC_STEAM_BATTLESTAFF_PRETTY, EquipmentSlot::WEAPON,
        detail::kProvidedElements<RuneElement::WATER, RuneElement::FIRE>},
    EquipmentDefinition{gamevals::ItemID::TWINFLAME_STAFF, EquipmentSlot::WEAPON,
        detail::kProvidedElements<RuneElement::WATER, RuneElement::FIRE>},
    EquipmentDefinition{gamevals::ItemID::LAVA_BATTLESTAFF, EquipmentSlot::WEAPON,
        detail::kProvidedElements<RuneElement::EARTH, RuneElement::FIRE>},
    EquipmentDefinition{gamevals::ItemID::MYSTIC_LAVA_STAFF, EquipmentSlot::WEAPON,
        detail::kProvidedElements<RuneElement::EARTH, RuneElement::FIRE>},
    EquipmentDefinition{gamevals::ItemID::LAVA_BATTLESTAFF_PRETTY, EquipmentSlot::WEAPON,
        detail::kProvidedElements<RuneElement::EARTH, RuneElement::FIRE>},
    EquipmentDefinition{gamevals::ItemID::MYSTIC_LAVA_STAFF_PRETTY, EquipmentSlot::WEAPON,
        detail::kProvidedElements<RuneElement::EARTH, RuneElement::FIRE>},
    EquipmentDefinition{gamevals::ItemID::SHADOWFLAME_QUADRANT, EquipmentSlot::WEAPON,
        detail::kProvidedElements<RuneElement::AIR, RuneElement::WATER, RuneElement::EARTH, RuneElement::FIRE>},
    // magic_runecount checks charged tomes in the off-hand separately from the weapon enums.
    EquipmentDefinition{gamevals::ItemID::TOME_OF_FIRE, EquipmentSlot::SHIELD, detail::kProvidedElements<RuneElement::FIRE>},
    EquipmentDefinition{gamevals::ItemID::TOME_OF_WATER, EquipmentSlot::SHIELD, detail::kProvidedElements<RuneElement::WATER>},
    EquipmentDefinition{gamevals::ItemID::TOME_OF_EARTH, EquipmentSlot::SHIELD, detail::kProvidedElements<RuneElement::EARTH>},
};

/// Static catalog membership only; callers must check the equipped slot and world restrictions.
/// The view has static lifetime; unknown IDs, including empty tomes, produce an empty view.
inline constexpr std::span<const RuneElement> getRuneElementsProvidedByEquipment(int32_t itemId) {
    for (const auto& equipment : kEquipment) {
        if (equipment.itemId == itemId) return equipment.elements;
    }
    return {};
}

}  // namespace RuneElementEquipmentInfo
}  // namespace titan
