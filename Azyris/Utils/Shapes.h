#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

namespace Az
{

    struct OBB
    {
        glm::mat4 transform;
        glm::vec3 halfExtents;

        // Helper methods
        glm::vec3 GetCenter() const;
        // Get the 8 corner points of the OBB
        std::array<glm::vec3, 8> GetCorners() const;
        std::array<glm::vec3, 3> GetAxes() const;
    };

    struct AABB
    {
        glm::vec3 min{0.0f};
        glm::vec3 max{0.0f};

        static AABB FromOBB(const OBB &obb);
    };

    struct Plane
    {
        Plane() = default;
        Plane(const glm::vec3 &n, float d) : normal(n), distance(d) {}

        glm::vec3 normal = {0, 1, 0};
        float distance = 0.f;
    };

    struct Frustum
    {
        Plane topFace;
        Plane bottomFace;

        Plane rightFace;
        Plane leftFace;

        Plane farFace;
        Plane nearFace;

        std::array<glm::vec3, 8> corners;
    };

    OBB CreateOBBFromAABB(const glm::vec3 &aabbMin, const glm::vec3 &aabbMax, const glm::mat4 &rotation);

    // OBB-Frustum intersection test (Separating Axis Theorem - SAT)
    bool TestOBBFrustum(const OBB &obb, const Frustum &frustum);

    // AABB-Frustum intersection test (faster but less accurate)
    bool TestAABBFrustum(const AABB &aabb, const Frustum &frustum);

    // Combined approach: AABB test first (fast rejection), then OBB test if needed
    bool TestOBBFrustumHybrid(const OBB &obb, const Frustum &frustum);

    // Enhanced OBB-Frustum test with 8 corners (alternative approach)
    bool TestOBBFrustumCorners(const OBB &obb, const Frustum &frustum);
}