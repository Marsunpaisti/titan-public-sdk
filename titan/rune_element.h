/// @file titan/rune_element.h
/// @brief Rune elements and their item catalog, independent of live game state.

#pragma once

#include "generated/gamevals/item_id.h"

#include <array>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>

namespace titan {

enum class RuneElement : int32_t {
    AIR,
    WATER,
    EARTH,
    FIRE,
    MIND,
    BODY,
    COSMIC,
    CHAOS,
    NATURE,
    LAW,
    DEATH,
    ASTRAL,
    BLOOD,
    SOUL,
    WRATH,
};

namespace RuneElementInfo {

namespace detail {

// Explicit static arrays avoid MSVC's runtime initialization issues with nested constexpr initializer_lists.
template <RuneElement... Elements>
inline constexpr std::array<RuneElement, sizeof...(Elements)> kProvidedElements{Elements...};

template <int32_t... ItemIds>
inline constexpr std::array<int32_t, sizeof...(ItemIds)> kRuneIds{ItemIds...};

}  // namespace detail

struct RuneDefinition {
    int32_t itemId;
    std::span<const RuneElement> elements;
    bool membersOnly = false;
};

struct ElementDefinition {
    RuneElement element;
    int32_t primitiveRuneId;
    std::span<const int32_t> combinationRuneIds;
    // Single-element substitutes, such as sunfire, are not combination runes.
    std::span<const int32_t> variantRuneIds;
};

inline constexpr std::array<RuneDefinition, 23> kRunes = {{
    {gamevals::ItemID::AIRRUNE,     detail::kProvidedElements<RuneElement::AIR>},
    {gamevals::ItemID::WATERRUNE,   detail::kProvidedElements<RuneElement::WATER>},
    {gamevals::ItemID::EARTHRUNE,   detail::kProvidedElements<RuneElement::EARTH>},
    {gamevals::ItemID::FIRERUNE,    detail::kProvidedElements<RuneElement::FIRE>},
    {gamevals::ItemID::MINDRUNE,    detail::kProvidedElements<RuneElement::MIND>},
    {gamevals::ItemID::BODYRUNE,    detail::kProvidedElements<RuneElement::BODY>},
    {gamevals::ItemID::COSMICRUNE,  detail::kProvidedElements<RuneElement::COSMIC>},
    {gamevals::ItemID::CHAOSRUNE,   detail::kProvidedElements<RuneElement::CHAOS>},
    {gamevals::ItemID::NATURERUNE,  detail::kProvidedElements<RuneElement::NATURE>},
    {gamevals::ItemID::LAWRUNE,     detail::kProvidedElements<RuneElement::LAW>},
    {gamevals::ItemID::DEATHRUNE,   detail::kProvidedElements<RuneElement::DEATH>},
    {gamevals::ItemID::ASTRALRUNE,  detail::kProvidedElements<RuneElement::ASTRAL>, true},
    {gamevals::ItemID::BLOODRUNE,   detail::kProvidedElements<RuneElement::BLOOD>, true},
    {gamevals::ItemID::SOULRUNE,    detail::kProvidedElements<RuneElement::SOUL>, true},
    {gamevals::ItemID::WRATHRUNE,   detail::kProvidedElements<RuneElement::WRATH>, true},
    {gamevals::ItemID::MISTRUNE,    detail::kProvidedElements<RuneElement::AIR, RuneElement::WATER>, true},
    {gamevals::ItemID::DUSTRUNE,    detail::kProvidedElements<RuneElement::AIR, RuneElement::EARTH>, true},
    {gamevals::ItemID::SMOKERUNE,   detail::kProvidedElements<RuneElement::AIR, RuneElement::FIRE>, true},
    {gamevals::ItemID::MUDRUNE,     detail::kProvidedElements<RuneElement::WATER, RuneElement::EARTH>, true},
    {gamevals::ItemID::STEAMRUNE,   detail::kProvidedElements<RuneElement::WATER, RuneElement::FIRE>, true},
    {gamevals::ItemID::LAVARUNE,    detail::kProvidedElements<RuneElement::EARTH, RuneElement::FIRE>, true},
    {gamevals::ItemID::AETHERRUNE,  detail::kProvidedElements<RuneElement::COSMIC, RuneElement::SOUL>, true},
    {gamevals::ItemID::SUNFIRERUNE, detail::kProvidedElements<RuneElement::FIRE>, true},
}};

inline constexpr std::array kElements = {
    ElementDefinition{RuneElement::AIR, gamevals::ItemID::AIRRUNE,
        detail::kRuneIds<gamevals::ItemID::MISTRUNE, gamevals::ItemID::DUSTRUNE, gamevals::ItemID::SMOKERUNE>, {}},
    ElementDefinition{RuneElement::WATER, gamevals::ItemID::WATERRUNE,
        detail::kRuneIds<gamevals::ItemID::MISTRUNE, gamevals::ItemID::MUDRUNE, gamevals::ItemID::STEAMRUNE>, {}},
    ElementDefinition{RuneElement::EARTH, gamevals::ItemID::EARTHRUNE,
        detail::kRuneIds<gamevals::ItemID::DUSTRUNE, gamevals::ItemID::MUDRUNE, gamevals::ItemID::LAVARUNE>, {}},
    ElementDefinition{RuneElement::FIRE, gamevals::ItemID::FIRERUNE,
        detail::kRuneIds<gamevals::ItemID::SMOKERUNE, gamevals::ItemID::STEAMRUNE, gamevals::ItemID::LAVARUNE>,
        detail::kRuneIds<gamevals::ItemID::SUNFIRERUNE>},
    ElementDefinition{RuneElement::MIND, gamevals::ItemID::MINDRUNE, {}, {}},
    ElementDefinition{RuneElement::BODY, gamevals::ItemID::BODYRUNE, {}, {}},
    ElementDefinition{RuneElement::COSMIC, gamevals::ItemID::COSMICRUNE, detail::kRuneIds<gamevals::ItemID::AETHERRUNE>, {}},
    ElementDefinition{RuneElement::CHAOS, gamevals::ItemID::CHAOSRUNE, {}, {}},
    ElementDefinition{RuneElement::NATURE, gamevals::ItemID::NATURERUNE, {}, {}},
    ElementDefinition{RuneElement::LAW, gamevals::ItemID::LAWRUNE, {}, {}},
    ElementDefinition{RuneElement::DEATH, gamevals::ItemID::DEATHRUNE, {}, {}},
    ElementDefinition{RuneElement::ASTRAL, gamevals::ItemID::ASTRALRUNE, {}, {}},
    ElementDefinition{RuneElement::BLOOD, gamevals::ItemID::BLOODRUNE, {}, {}},
    ElementDefinition{RuneElement::SOUL, gamevals::ItemID::SOULRUNE, detail::kRuneIds<gamevals::ItemID::AETHERRUNE>, {}},
    ElementDefinition{RuneElement::WRATH, gamevals::ItemID::WRATHRUNE, {}, {}},
};

/// The view refers to static catalog data; unknown item IDs produce an empty view.
inline constexpr std::span<const RuneElement> getRuneElementsProvidedByRune(int32_t itemId) {
    for (const auto& rune : kRunes) {
        if (rune.itemId == itemId) return rune.elements;
    }
    return {};
}

inline constexpr bool isRune(int32_t itemId) {
    return !getRuneElementsProvidedByRune(itemId).empty();
}

/// Invalid element values have no primary rune.
inline constexpr std::optional<int32_t> getPrimaryRuneForElement(RuneElement element) {
    for (const auto& entry : kElements) {
        if (entry.element == element) return entry.primitiveRuneId;
    }
    return std::nullopt;
}

/// Primary rune first, then combinations and variants; invalid elements produce an empty list.
inline constexpr std::vector<int32_t> getAllRunesProvidingElement(RuneElement element) {
    for (const auto& entry : kElements) {
        if (entry.element != element) continue;
        std::vector<int32_t> result{entry.primitiveRuneId};
        result.insert(result.end(), entry.combinationRuneIds.begin(), entry.combinationRuneIds.end());
        result.insert(result.end(), entry.variantRuneIds.begin(), entry.variantRuneIds.end());
        return result;
    }
    return {};
}

}  // namespace RuneElementInfo
}  // namespace titan
