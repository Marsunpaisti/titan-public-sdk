# Equipment-provided runes: cache evidence

Checked 2026-10-10. Scope: a possible Titan C++ `RuneElementEquipment` catalog;
no feature implementation.

## Answer

**Elemental staff membership is available in cache enums.** It is not necessary
to infer it from item names or copy a third-party hardcoded list. The current
rune-count script uses four `item -> boolean` enums, then explicit checks for
charged tomes. A complete catalog therefore combines enum data with script
semantics; there is not one demonstrated universal item-to-elements field.[1], [2]

For the SDK as exposed today, a small checked-in catalog sourced from those
enums and tome checks is practical. Offline generation is possible; runtime
cache-enum access would require additional Titan API support. Do not describe
the mapping as unavailable in the OSRS cache merely because Titan does not
expose it.

## Verified current cache data

Source: `abextm/osrs-cache` commit
`eb4f705223d60ea49586f8fe32e03792001ec41a` (2026-10-07, revision 241).[1], [2]
Decoded config index **2**, enum archive **8**, using the formats implemented by
RuneLite's `FlatStorage` and `EnumLoader`.[3] Each enum below has key type `o`
(item), value type `1` (boolean), default `0`; every listed entry has value `1`.

| Element | Enum ID | Item IDs with true entries |
| --- | --- | --- |
| Air | 988 | 1381, 1397, 1405, 11998, 12000, 20404, 20730, 20733, 20736, 20739, 33253 |
| Water | 989 | 1383, 1395, 1403, 6562, 6563, 11787, 11789, 12795, 12796, 20730, 20733, 21006, 30634, 33253 |
| Earth | 996 | 1385, 1399, 1407, 3053, 21198, 3054, 21200, 6562, 6563, 20736, 20739, 33253 |
| Fire | 997 | 1387, 1393, 1401, 3053, 21198, 3054, 21200, 11787, 11789, 12795, 12796, 11998, 12000, 30634, 33253 |

These are four membership sets, not a single enum returning a rune ID. Invert
the sets to obtain `item ID -> elements`. The entries cover basic, battle and
mystic staves; combination staves; ornamented lava/steam variants; Kodai wand
(21006); twinflame staff (30634); and shadowflame quadrant (33253, all four).
The air set also contains `BR_AIR_STAFF` (20404). Names are corroborated by the
repository's [generated item IDs](../../titan/generated/gamevals/item_id.h).
Presence in cache data alone does not establish availability in every game mode.

## Script rules beyond the enum sets

Decoded current script index **12**, script **19** (`magic_runecount`). It has
1,176 instructions and three integer arguments. Its elemental branches perform
`ENUM` lookups against 988/989/996/997 and return `2147483647` when matched.
Separate off-hand comparisons return the same sentinel for the following IDs.[2]

| Element | Accepted charged tome | Uncharged counterpart, not accepted by that check |
| --- | --- | --- |
| Fire | 20714 | 20716 |
| Water | 25574 | 25576 |
| Earth | 30064 | 30066 |

The current script also checks `MAP_MEMBERS` and `OC_MEMBERS`, clearing
members-only weapon/off-hand candidates in the normal F2P path before applying
the equipment checks. **Catalog membership and effectiveness on the current
world are different questions.** A live provider calculation needs world and
item-membership restrictions; matching an ID alone is insufficient.[2]

An older, readable decompilation corroborates the enum calls and F2P filtering,
but its 2021 revision predates the earth tome and newer staff entries. It must
not be used as the current equipment list.[4]

**Do not interpret the script's maximum-integer sentinel as proof of genuinely
unlimited resources in every branch.** Current script 19 also returns it for
charged Bryophyta's staff (22370) in its nature-rune branch and for game-mode
conditions. The script is a spell-interface availability calculation, not an
equipment-only classification API. Tome classification here concerns rune
availability; this investigation did not verify spell-specific page consumption
or damage-boost rules.[2]

## Membership flags for quantity aggregation

Verified all **23 rune IDs and 35 equipment-provider IDs** currently listed in
[`rune_element.h`](../../titan/rune_element.h) and
[`rune_element_equipment.h`](../../titan/rune_element_equipment.h) against the
same pinned revision-241 cache: config index **2**, item archive **10**.[1]
The item definition defaults `members` to `false`; item opcode **16** sets it
to `true`.[5], [9] These are decoded flags, not classifications inferred from
names or expected F2P availability.

| Catalog | `members` | Complete set of matching item IDs |
| --- | --- | --- |
| Runes (11) | `false` | 554, 555, 556, 557, 558, 559, 560, 561, 562, 563, 564 |
| Runes (12) | `true` | 565, 566, 4694, 4695, 4696, 4697, 4698, 4699, 9075, 21880, 28929, 30843 |
| Equipment (5) | `false` | 1381, 1383, 1385, 1387, **20404** |
| Equipment (30) | `true` | 1393, 1395, 1397, 1399, 1401, 1403, 1405, 1407, 3053, 3054, 6562, 6563, 11787, 11789, 11998, 12000, 12795, 12796, 20714, 20730, 20733, 20736, 20739, 21006, 21198, 21200, 25574, 30064, 30634, 33253 |

The rune flags match the expected partition: astral, blood, soul, wrath, all
seven combination runes (including aether), and sunfire are members-only;
the other eleven primitive runes are not.

**Equipment exception: `BR_AIR_STAFF` (20404) is not members-only.** Its item
record has no opcode 16 and its current decoded name is literally `"Null"`.
It remains a true entry in air-provider enum 988. Thus a membership-only guard
must treat it like the four basic elemental staves, without claiming that it
is obtainable or equippable on ordinary F2P worlds.[1]

For an explicit `membersWorld` input, the membership eligibility condition is
`membersWorld || !itemMembers`. Apply it to the **source item**, before crediting
its elements: a members-only combination rune cannot supply even its F2P
elements when `membersWorld` is false. Equipped-slot and other existing
eligibility checks remain separate from this flag.

Verification checked catalog cardinalities, decompressed archive length,
archive chunk boundaries and full consumption of every one of the 58 item
records, failing on unknown opcodes. None of these records had a note, bought,
or placeholder template requiring a membership override through linking.

## Item params, RuneLite, and Titan

- **Item params:** RuneLite's item definition has a generic `params` map, but no
  dedicated rune-provision property was identified. More importantly, the
  inspected elemental branches use enums and explicit tome comparisons, not an
  item-param lookup. This is evidence for the mechanism above, **not proof that
  no related param, enum, or DB table exists anywhere in the cache**.[2], [5]
- **Current RuneLite upstream:** at commit
  `42a6f17a6a2e8e478aa763890ecd0181a59dad38`, its complete tree has no `Rune.java`,
  and a text search of all 1,337 Java files under `runelite-client` found no
  `enum Rune` declaration. The proposed `client/game/Rune.java` URL is absent.
  Consequently, a claim that current upstream uses that hardcoded Rune enum is
  unverified; a fork, plugin, or different revision would need its own source.
  RuneLite's current rune-pouch overlay does use cache enum 982, but that maps
  pouch type numbers to rune items, not equipment to rune elements.[6], [7]
- **Titan's current public surface:** [`ItemDef` and `CacheFacade`](../../titan/client.h)
  expose item identity, name, members flag, note links, actions/sub-operations,
  and several other definition types. They expose neither item params nor enum
  lookup/enumeration. Generated item constants supply IDs, not provision rules.
- **Executing a script is a different capability:** the same file exposes
  `state::script().run` / `runAndGetInt`. This does not expose script bytecode or
  supply a pure item catalog. Current script 19's three-argument signature also
  differs from the old one-argument decompilation. No live Titan invocation or
  runtime calling convention was verified.

## Verification and limits

The current flatcache archives were fetched read-only, base64-decoded,
decompressed and decoded using the linked RuneLite formats. Enum decoding
checked complete buffer consumption; archive splitting checked data boundaries.
Script decoding checked both the instruction count and the bytecode boundary
against its footer.[3], [8] This directly verifies current enum contents and the
reported script comparisons without depending on the older decompiler dump.

No exhaustive parameter/DB-table survey or live server behavior test was run.
Cache scripts are client-side evidence, not the server's authoritative rules.
Any future generated table should record the cache revision; a static table
will otherwise miss later equipment variants.

## Sources

[1]: https://github.com/abextm/osrs-cache/blob/eb4f705223d60ea49586f8fe32e03792001ec41a/2.flatcache
[2]: https://github.com/abextm/osrs-cache/blob/eb4f705223d60ea49586f8fe32e03792001ec41a/12.flatcache
[3]: https://github.com/runelite/runelite/blob/42a6f17a6a2e8e478aa763890ecd0181a59dad38/cache/src/main/java/net/runelite/cache/definitions/loaders/EnumLoader.java
[4]: https://github.com/RuneStar/cs2-scripts/blob/7da6c1fbab51b7528ff13b15a532a44208bf75d6/scripts/%5Bproc%2Cmagic_runecount%5D.cs2
[5]: https://github.com/runelite/runelite/blob/42a6f17a6a2e8e478aa763890ecd0181a59dad38/cache/src/main/java/net/runelite/cache/definitions/ItemDefinition.java
[6]: https://api.github.com/repos/runelite/runelite/git/trees/42a6f17a6a2e8e478aa763890ecd0181a59dad38?recursive=1
[7]: https://github.com/runelite/runelite/blob/42a6f17a6a2e8e478aa763890ecd0181a59dad38/runelite-client/src/main/java/net/runelite/client/plugins/runepouch/RunepouchOverlay.java
[8]: https://github.com/runelite/runelite/blob/42a6f17a6a2e8e478aa763890ecd0181a59dad38/cache/src/main/java/net/runelite/cache/definitions/loaders/ScriptLoader.java
[9]: https://github.com/runelite/runelite/blob/42a6f17a6a2e8e478aa763890ecd0181a59dad38/cache/src/main/java/net/runelite/cache/definitions/loaders/ItemLoader.java

Additional decoding reference:
[FlatStorage](https://github.com/runelite/runelite/blob/42a6f17a6a2e8e478aa763890ecd0181a59dad38/cache/src/main/java/net/runelite/cache/fs/flat/FlatStorage.java).
