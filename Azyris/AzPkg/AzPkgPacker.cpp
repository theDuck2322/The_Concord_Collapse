#include <AzPkgPacker.h>

namespace Az
{
    bool AzPkgPacker::PackAsset(const std::string &renderObj, const std::string &collisionObj, const std::string &outputName)
    {
        // ------------------------------------------------------------
        // 1. Load input files
        // ------------------------------------------------------------

        std::string DrawableModelPath = "Model/" + renderObj;
        std::string CollisionModelPath = "Model/" + collisionObj;

        std::vector<char>
            renderModel = m_ReadBinaryFile(DrawableModelPath);

        std::vector<char> collisionModel = m_ReadBinaryFile(CollisionModelPath); // may be empty

        if (renderModel.empty())
        {
            std::cout << "RenderObj is required!" << std::endl;
            return 1;
        }

        bool hasCollisionModel = !collisionModel.empty();

        // ------------------------------------------------------------
        // 2. Create output package
        // ------------------------------------------------------------

        std::ofstream outFile("output/" + outputName + ".AzPkg", std::ios::binary);

        if (!outFile)
        {
            std::cout << "Failed to create AzPkg file" << std::endl;
            return 1;
        }

        // ------------------------------------------------------------
        // 3. Write header (placeholder values)
        // ------------------------------------------------------------

        Az::PkgHeader header{};
        std::memcpy(header.magic, "AzPkg", 5);

        header.version = 1;
        header.assetType = (uint8_t)Az::PkgAssetType::Model;
        header.fallback = (uint8_t)Az::PkgFallback::ConvexHull;

        header.fileCount = hasCollisionModel ? 2 : 1;

        outFile.write(reinterpret_cast<char *>(&header), sizeof(header));

        // ------------------------------------------------------------
        // 4. Reserve space for file table
        // ------------------------------------------------------------

        std::vector<Az::PkgFileEntry> fileTable(header.fileCount);

        std::streampos fileTablePosition = outFile.tellp();

        outFile.write(
            reinterpret_cast<char *>(fileTable.data()),
            sizeof(Az::PkgFileEntry) * fileTable.size());

        // ------------------------------------------------------------
        // 5. Write files and fill table
        // ------------------------------------------------------------

        uint32_t currentIndex = 0;

        auto WriteFileToPackage =
            [&](const char *virtualPath, const std::vector<char> &data)
        {
            Az::PkgFileEntry &entry = fileTable[currentIndex];

            std::strncpy(entry.path, virtualPath, sizeof(entry.path) - 1);
            entry.path[sizeof(entry.path) - 1] = '\0';

            entry.offset = (uint32_t)outFile.tellp();
            entry.size = (uint32_t)data.size();

            outFile.write(data.data(), data.size());

            currentIndex++;
        };

        WriteFileToPackage(DrawableModelPath.c_str(), renderModel);

        if (hasCollisionModel)
            WriteFileToPackage(CollisionModelPath.c_str(), collisionModel);

        // ------------------------------------------------------------
        // 6. Rewrite file table with correct offsets
        // ------------------------------------------------------------

        outFile.seekp(fileTablePosition);

        outFile.write(
            reinterpret_cast<char *>(fileTable.data()),
            sizeof(Az::PkgFileEntry) * fileTable.size());

        outFile.close();

        std::cout << "AzPkg created successfully!" << std::endl;
        return false;
    }

    std::vector<char> AzPkgPacker::m_ReadBinaryFile(const std::string &path)
    {
        std::ifstream file(path, std::ios::binary);

        if (!file)
        {
            std::cout << "Failed to open: " + path << std::endl;
            return {};
        }
        return std::vector<char>(std::istreambuf_iterator<char>(file),
                                 std::istreambuf_iterator<char>());
    }
}