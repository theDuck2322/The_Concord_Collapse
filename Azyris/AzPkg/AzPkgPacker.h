#pragma once

#include <fstream>
#include <vector>
#include <cstring>
#include <cstdint>
#include <iostream>

#include <AzPkgStructure.h>

namespace Az
{
    class AzPkgPacker
    {
    public:
        static bool PackAsset(const std::string &renderObj,
                              const std::string &collisionObj = "",
                              const std::string &outputName = "output");

    private:
        static std::vector<char> m_ReadBinaryFile(const std::string &path);
    };
}
