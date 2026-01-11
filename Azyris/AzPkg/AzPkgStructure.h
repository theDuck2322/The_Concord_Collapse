#pragma once

#include <stdint.h>

namespace Az
{
    enum class PkgAssetType : uint8_t
    {
        Unknown = 0,
        Model = 1,
        Sound = 2,
    };
    enum class PkgFallback : uint8_t
    {
        None = 0,
        ConvexHull = 1,
        MeshCollider = 2
    };

    struct PkgHeader
    {
        char magic[6];    // "AzPkg"
        uint16_t version; // 1

        uint8_t assetType; // AzPkgAssetType
        uint8_t fallback;  // AzPkgFallback (models only)

        uint16_t fileCount; // how many files are inside
    };

    struct PkgFileEntry
    {
        char path[128];  // virtual path
        uint32_t offset; // byte offset in file
        uint32_t size;   // file size in bytes
    };
}