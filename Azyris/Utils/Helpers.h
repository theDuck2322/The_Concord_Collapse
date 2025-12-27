#pragma once

#include <iostream>
#include <stdlib.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp> // For glm::rotate
#include <glm/gtc/constants.hpp> // For glm::radians

#include <matrix4x4.h>
#include <Bullet/btBulletDynamicsCommon.h>
#include <Bullet/btBulletCollisionCommon.h>

#define MIN_INT16 -32768
#define MAX_INT16 32767

// if condition is false then it asserts
#define AZ_Assert(cond, msg)                                                   \
    do                                                                         \
    {                                                                          \
        if (!(cond))                                                           \
        {                                                                      \
            std::cerr << "Assertion failed at " << __FILE__ << ":" << __LINE__ \
                      << " in function " << __FUNCTION__ << "\n"               \
                      << "Message: " << (msg) << std::endl;                    \
            std::abort();                                                      \
        }                                                                      \
    } while (false)

namespace Az
{

    glm::mat4 BtQuatToGlmMat4(const btQuaternion &q);

    float intervalNormalizer(int x,
                             int xMin = MIN_INT16,
                             int xMax = MAX_INT16,
                             int yMin = -1,
                             int yMax = 1);

    float normalizeAxis(int x);

    float normalize(float min, float max, float value);

    float deNormalize(float min, float max, float value);

    float getRandomFloat(float min, float max);

    int getRandomInt(int min, int max);

    int getRandomInt(int max);

    void SetWorkingDir();

    float DistanceBetweenPoints(const glm::vec3 &p1, const glm::vec3 &p2);

    glm::vec3 rotatePoint(glm::vec3 point, glm::vec3 ref, float rad);

    glm::mat4 Convert(const aiMatrix4x4 &m);

};