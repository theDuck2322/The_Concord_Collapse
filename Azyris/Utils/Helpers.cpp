#include <Helpers.h>
#include <filesystem>
#include <Mesh.h>

#define AZ_PI 3.14159265359

namespace Az
{
    glm::mat4 BtQuatToGlmMat4(const btQuaternion &q)
    {
        // Convert Bullet quaternion to GLM quaternion
        glm::quat glmQuat(q.getW(), q.getX(), q.getY(), q.getZ());

        // Convert to 4x4 rotation matrix
        return glm::mat4_cast(glmQuat);
    }

    float intervalNormalizer(int x, int xMin, int xMax, int yMin, int yMax)
    {
        float xf = static_cast<float>(x);
        float xMinf = static_cast<float>(xMin);
        float xMaxf = static_cast<float>(xMax);

        float y = yMin + (yMax - yMin) * ((xf - xMinf) / (xMaxf - xMinf));
        return y;
    }

    float normalizeAxis(int x)
    {
        if (x >= 0)
            return float(x) / 32767.0f;
        else
            return float(x) / 32768.0f;
    }

    float normalize(float min, float max, float value)
    {
        return (value - min) / (max - min);
    }

    float deNormalize(float min, float max, float value)
    {
        return (value * (max - min)) - min;
    }

    float getRandomFloat(float min, float max)
    {
        if (min > max)
            std::swap(min, max);

        float num = static_cast<float>(rand()) / RAND_MAX * (max - min) + min;

        return num;
    }

    int getRandomInt(int min, int max)
    {
        if (min > max)
            std::swap(min, max);

        int num = rand() % (max - min + 1) + min;

        return num;
    }

    int getRandomInt(int max)
    {
        int num = rand() % max;
        return num;
    }

    void SetWorkingDir()
    {
        std::filesystem::path currentDir = std::filesystem::current_path();
        std::filesystem::path parentDir = currentDir.parent_path();
        std::string folderName = currentDir.filename().string();

        std::cout << "Current Folder: " << folderName << std::endl;

        if (folderName == "Debug")
        {
            currentDir = parentDir.parent_path();
            std::filesystem::current_path(currentDir);

            std::cout << "Changed working directory to: " << std::filesystem::current_path() << std::endl;
        }

        else if (folderName == "Release")
        {
            currentDir = parentDir.parent_path();
            std::filesystem::current_path(currentDir);

            std::cout << "Changed working directory to: " << std::filesystem::current_path() << std::endl;
        }
    }

    float DistanceBetweenPoints(const glm::vec3 &p1, const glm::vec3 &p2)
    {
        return glm::distance(p1, p2);
    }

    glm::vec3 rotatePoint(glm::vec3 point, glm::vec3 ref, float rad)
    {
        glm::mat4 rotationMatrix = glm::rotate(rad, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around Z-axis

        glm::vec4 translatedPoint = glm::vec4(point - ref, 1.0f);  // Move point relative to ref
        glm::vec4 rotatedPoint = rotationMatrix * translatedPoint; // Apply rotation
        return glm::vec3(rotatedPoint) + ref;                      // Move point back
    }

    glm::mat4 Convert_aiMat4(const aiMatrix4x4 &m)
    {
        glm::mat4 r;
        r[0][0] = m.a1;
        r[1][0] = m.a2;
        r[2][0] = m.a3;
        r[3][0] = m.a4;
        r[0][1] = m.b1;
        r[1][1] = m.b2;
        r[2][1] = m.b3;
        r[3][1] = m.b4;
        r[0][2] = m.c1;
        r[1][2] = m.c2;
        r[2][2] = m.c3;
        r[3][2] = m.c4;
        r[0][3] = m.d1;
        r[1][3] = m.d2;
        r[2][3] = m.d3;
        r[3][3] = m.d4;

        return r;
    }

    btVector3 ConvertGLMVec3(const glm::vec3 &vec)
    {
        return btVector3(vec.x, vec.y, vec.z);
    }
    glm::vec3 ConvertBTVec3(const btVector3 &vec)
    {
        return glm::vec3(vec.getX(), vec.getY(), vec.getZ());
    }
    glm::quat ConvertBTQuat(const btQuaternion &qt)
    {
        return glm::quat(qt.getW(), qt.getX(), qt.getY(), qt.getZ());
    }

}