/// @file titan/web_walker_areas.h
/// @brief How a web walker request names whole areas for a route to avoid.
///
/// Normal plugins use <titan/web_walker.h>: `WebPathRequest::forbiddenAreas`
/// and `WebWalkOptions::forbiddenAreas` take `titan::WorldArea`s and do the
/// encoding below. This header is the wire convention itself, for walker
/// engines and for callers that fill the fixed ABI arrays by hand.
///
/// `HostApi::webPathSubmit` and `webWalkStart` take the tiles a route must
/// avoid as an array of `WorldPointState`. An area rides in that same array
/// as two consecutive entries:
///
///   { x,     y,      plane, kWebPathForbiddenAreaOrigin }   south-west tile
///   { width, height, plane, kWebPathForbiddenAreaExtent }   east and north
///
/// So areas add no HostApi function, change no payload record and need no
/// host change: a host forwards the array as it always has. Each area takes
/// two of the array's kWebPathMaxForbiddenTiles entries, whatever its size.
///
/// To an engine that honours areas, an area is every one of its tiles: no
/// walking step, transport or teleport may end inside it. A request that
/// starts inside an area can only leave across its edge, and a destination
/// inside one has no route.
///
/// An engine that predates areas reads the two entries as two tiles that
/// forbid nothing useful, so the request stays valid and the area is simply
/// not avoided. A caller that must be safe on such an engine also sends the
/// area's outline as ordinary tiles (a ring of tiles cannot be walked
/// across, though it does not stop a teleport landing inside it);
/// <titan/web_walker.h> does so by default.

#pragma once

#include "detail/abi.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace titan {

/// `worldViewId` tags of the two entries that make one area. No world view
/// has either id.
inline constexpr int32_t kWebPathForbiddenAreaOrigin = -0x41524541;  // "AREA"
inline constexpr int32_t kWebPathForbiddenAreaExtent = -0x41524542;
/// The longest side one encoded area may have. Longer areas are sent as
/// several (appendWebPathForbiddenArea splits them).
inline constexpr int32_t kWebPathForbiddenAreaMaxSide = 1024;

/// One decoded area: `x`,`y` its south-west tile, `width` east, `height` north.
struct WebPathForbiddenArea {
    int32_t x = 0;
    int32_t y = 0;
    int32_t width = 0;
    int32_t height = 0;
    int32_t plane = 0;
};

/// Append one area to a forbidden array, split into pieces no longer than
/// kWebPathForbiddenAreaMaxSide a side. An area with a side below one, or a
/// plane outside 0..3, appends nothing.
inline void appendWebPathForbiddenArea(
        std::vector<TitanPluginSdk::WorldPointState>& out,
        int32_t x, int32_t y, int32_t width, int32_t height, int32_t plane) {
    if (width <= 0 || height <= 0 || plane < 0 || plane >= 4) return;
    for (int64_t dy = 0; dy < height; dy += kWebPathForbiddenAreaMaxSide) {
        const int64_t pieceHeight =
            height - dy < kWebPathForbiddenAreaMaxSide
                ? height - dy : kWebPathForbiddenAreaMaxSide;
        for (int64_t dx = 0; dx < width; dx += kWebPathForbiddenAreaMaxSide) {
            const int64_t pieceWidth =
                width - dx < kWebPathForbiddenAreaMaxSide
                    ? width - dx : kWebPathForbiddenAreaMaxSide;
            const int64_t pieceX = static_cast<int64_t>(x) + dx;
            const int64_t pieceY = static_cast<int64_t>(y) + dy;
            if (pieceX > INT32_MAX || pieceY > INT32_MAX) continue;
            out.push_back({static_cast<int32_t>(pieceX),
                           static_cast<int32_t>(pieceY), plane,
                           kWebPathForbiddenAreaOrigin});
            out.push_back({static_cast<int32_t>(pieceWidth),
                           static_cast<int32_t>(pieceHeight), plane,
                           kWebPathForbiddenAreaExtent});
        }
    }
}

/// Append an area's outline as ordinary tiles, for engines that predate
/// areas. Stops when `out` holds `limit` entries; returns whether the whole
/// outline went in.
inline bool appendWebPathForbiddenOutline(
        std::vector<TitanPluginSdk::WorldPointState>& out,
        int32_t x, int32_t y, int32_t width, int32_t height, int32_t plane,
        std::size_t limit, int32_t worldViewId = 0) {
    if (width <= 0 || height <= 0 || plane < 0 || plane >= 4) return true;
    const int64_t east = static_cast<int64_t>(x) + width - 1;
    const int64_t north = static_cast<int64_t>(y) + height - 1;
    if (east > INT32_MAX || north > INT32_MAX) return true;
    const auto put = [&](int64_t tileX, int64_t tileY) {
        if (out.size() >= limit) return false;
        out.push_back({static_cast<int32_t>(tileX),
                       static_cast<int32_t>(tileY), plane, worldViewId});
        return true;
    };
    for (int64_t tileX = x; tileX <= east; ++tileX) {
        if (!put(tileX, y)) return false;
        if (north != y && !put(tileX, north)) return false;
    }
    for (int64_t tileY = static_cast<int64_t>(y) + 1; tileY < north; ++tileY) {
        if (!put(x, tileY)) return false;
        if (east != x && !put(east, tileY)) return false;
    }
    return true;
}

/// Read a forbidden array as an engine does: `tile(entry)` for each ordinary
/// tile and `area(WebPathForbiddenArea)` for each area. False when an area
/// is malformed (an origin with no extent after it, an extent on its own, a
/// side below one or above kWebPathForbiddenAreaMaxSide, mismatched planes);
/// nothing has then been reported that the caller should keep.
template <typename TileFn, typename AreaFn>
inline bool decodeWebPathForbidden(
        const TitanPluginSdk::WorldPointState* entries, uint32_t count,
        TileFn&& tile, AreaFn&& area) {
    if (count != 0 && !entries) return false;
    for (uint32_t index = 0; index < count; ++index) {
        const auto& entry = entries[index];
        if (entry.worldViewId == kWebPathForbiddenAreaExtent) return false;
        if (entry.worldViewId != kWebPathForbiddenAreaOrigin) {
            tile(entry);
            continue;
        }
        if (index + 1 >= count) return false;
        const auto& extent = entries[++index];
        if (extent.worldViewId != kWebPathForbiddenAreaExtent
            || extent.z != entry.z
            || extent.x <= 0 || extent.x > kWebPathForbiddenAreaMaxSide
            || extent.y <= 0 || extent.y > kWebPathForbiddenAreaMaxSide) {
            return false;
        }
        area(WebPathForbiddenArea{entry.x, entry.y, extent.x, extent.y, entry.z});
    }
    return true;
}

}  // namespace titan
