#include <AzPkgLoader.h>

namespace Az
{
    AzPkgLoader::AzPkgLoader(const std::string &pkgPath)
    {
        LoadPackage(pkgPath);
    }

    bool AzPkgLoader::LoadPackage(const std::string &pkgPath)
    {
        std::ifstream file(pkgPath, std::ios::binary);
        if (!file)
        {
            std::cout << "Failed to open package: " + pkgPath << std::endl;
            return 1;
        }

        // Read header
        file.read(reinterpret_cast<char *>(&m_Header), sizeof(m_Header));
        if (std::strncmp(m_Header.magic, "AzPkg", 5) != 0)
        {
            std::cout << "Invalid AzPkg magic!" << std::endl;
            return 1;
        }

        // Read file table
        m_FileTable.resize(m_Header.fileCount);
        file.read(reinterpret_cast<char *>(m_FileTable.data()),
                  sizeof(PkgFileEntry) * m_FileTable.size());

        // Store package file stream for extraction
        m_PackageFilePath = pkgPath;
        return false;
    }

    std::vector<char> AzPkgLoader::ExtractToMemory(const std::string &virtualPath)
    {
        for (const auto &entry : m_FileTable)
        {
            if (virtualPath == entry.path)
            {
                std::ifstream file(m_PackageFilePath, std::ios::binary);
                file.seekg(entry.offset);
                std::vector<char> buffer(entry.size);
                file.read(buffer.data(), entry.size);
                return buffer;
            }
        }
        return {}; // not found
    }
    bool AzPkgLoader::ExtractToDisk(const std::string &virtualPath, const std::string &outputPath)
    {
        auto data = ExtractToMemory(virtualPath);
        if (data.empty())
        {
            std::cout << "File not found in package: " + virtualPath << std::endl;
            return 1;
        }

        std::ofstream out(outputPath, std::ios::binary);
        out.write(data.data(), data.size());
        return 0;
    }
    void AzPkgLoader::PrintInfo()
    {
        std::cout << "AzPkg Version: " << m_Header.version << "\n";
        std::cout << "Asset Type: " << (int)m_Header.assetType << "\n";
        std::cout << "Fallback: " << (int)m_Header.fallback << "\n";
        std::cout << "File Count: " << m_Header.fileCount << "\n";
        std::cout << "Files:\n";
        for (const auto &entry : m_FileTable)
            std::cout << "  " << entry.path
                      << " (" << entry.size << " bytes)\n";
    }
}
