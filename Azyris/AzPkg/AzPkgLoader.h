#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdint>
#include <filesystem>

#include <AzPkgStructure.h>

namespace Az
{

    class AzPkgLoader
    {
    public:
        AzPkgLoader() = default;
        AzPkgLoader(const std::string &pkgPath);
        bool LoadPackage(const std::string &pkgPath);

        // Extract a file by virtual path to memory
        std::vector<char> ExtractToMemory(const std::string &virtualPath);

        // Optional: extract to disk
        bool ExtractToDisk(const std::string &virtualPath, const std::string &outputPath);

        void PrintInfo();

    private:
        PkgHeader m_Header{};
        std::vector<PkgFileEntry> m_FileTable;
        std::string m_PackageFilePath;
    };

}