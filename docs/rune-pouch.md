# Rune-pouch utilities

Include `<titan/utils/rune_pouch.h>` for possession and contents, or
`<titan/utils/runes.h>` for element amounts including pouch supplies.

```cpp
auto contents = titan::utils::RunePouch::getRunePouchContents();
auto carriedType = titan::utils::RunePouch::getCarriedRunePouchType();
auto ownsPouch = titan::utils::RunePouch::accountOwnsRunePouch();

// optional<bool>: test its value, not just whether the optional is engaged.
if (ownsPouch == true) {
    // Inventory or known bank contents contain a pouch.
}

auto pouchAmounts = titan::utils::Runes::getAvailableRuneElementAmountsFromCarriedRunePouch();
auto totalAmounts = titan::utils::Runes::getCarriedRuneElementAmounts();
```

## Contracts

- `RunePouchType` distinguishes `REGULAR` (three slots) and `DIVINE` (four).
  Locked variants have the same capacity. If supplied inventory data contains
  both types, divine wins; stored contents are counted only once.
- `RunePouchContents` is a vector of `ItemContainerSlot`, containing occupied
  slots with their original zero-based pouch indices, decoded rune item IDs,
  and positive quantities. Contents do not establish possession. All four
  stored slots are read even without a carried pouch or on an F2P world.
- `accountOwnsRunePouch()` returns true on a positive observation, false only
  when inventory and bank are known and neither contains a pouch, and nullopt
  otherwise. A readable open bank takes precedence over the account-aware host
  cache. Remembered contents are advisory; an unknown or loading cache cannot
  establish absence. Placeholders and zero quantities do not establish ownership.
- `getCarriedRunePouchType()` returns nullopt when no pouch is detected, including
  unavailable inventory. Possession and ownership do not require membership.
- Rune-element availability requires a members world and a carried pouch.
  Regular pouches ignore stored slot four. Combination runes contribute to each
  represented element; finite supplies are added before equipment's infinite
  amounts override them.

Supplied-item overloads accept `std::span<const ItemContainerSlot>`. Ownership's
two-span overload returns a plain bool because both snapshots are supplied.
The pouch-amount overload accepts `(inventoryItems, pouchContents, membersWorld)`.
The combined overload accepts `(inventoryItems, equippedItems, pouchContents,
membersWorld)`. The older three-argument combined overload has no pouch snapshot
and continues to count only loose inventory runes and equipped providers.

## Encoding and limits

The type values 1–23 follow the Java reference's rune ordering, cross-checked
against [RuneLite's pinned rune-image ordering and enum-982 usage][1]. Rune
items and the eight slot varbits use the SDK's generated named constants.
The SDK has no runtime cache-enum lookup, so decoding uses a static table.
Zero, unknown type values, and nonpositive quantities are omitted.

The SDK's varbit reader has no success flag: unreadable contents may appear
empty, including through the optional live amount getters. These reads are
sequential, not an atomic inventory/world/varbit snapshot. The three/four-slot
model does not include RuneLite's additional fifth and sixth slot varbits.
No live game-client execution has been verified.

[1]: https://github.com/runelite/runelite/blob/42a6f17a6a2e8e478aa763890ecd0181a59dad38/runelite-client/src/main/java/net/runelite/client/plugins/runepouch/RunepouchOverlay.java
