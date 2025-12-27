#include <Shapes.h>

namespace Az
{

    AABB AABB::FromOBB(const OBB &obb)
    {
        const auto corners = obb.GetCorners();
        AABB aabb;
        aabb.min = aabb.max = corners[0];

        for (const auto &corner : corners)
        {
            aabb.min = glm::min(aabb.min, corner);
            aabb.max = glm::max(aabb.max, corner);
        }

        return aabb;
    }

    glm::vec3 OBB::GetCenter() const
    {
        return glm::vec3(transform[3]);
    }

    std::array<glm::vec3, 8> OBB::GetCorners() const
    {
        std::array<glm::vec3, 8> corners;

        // Transform local corners to world space
        const glm::vec3 center = GetCenter();
        const glm::mat3 rotation(transform); // Extract rotation
        const glm::vec3 right = rotation[0] * halfExtents.x;
        const glm::vec3 up = rotation[1] * halfExtents.y;
        const glm::vec3 forward = rotation[2] * halfExtents.z;

        // Calculate all 8 corners
        corners[0] = center - right - up - forward; // ---
        corners[1] = center + right - up - forward; // +--
        corners[2] = center + right + up - forward; // ++-
        corners[3] = center - right + up - forward; // -+-
        corners[4] = center - right - up + forward; // --+
        corners[5] = center + right - up + forward; // +-+
        corners[6] = center + right + up + forward; // +++
        corners[7] = center - right + up + forward; // -++

        return corners;
    }

    std::array<glm::vec3, 3> OBB::GetAxes() const
    {
        const glm::mat3 rotation(transform);
        return {
            glm::normalize(rotation[0]),
            glm::normalize(rotation[1]),
            glm::normalize(rotation[2])};
    }

    OBB CreateOBBFromAABB(const glm::vec3 &aabbMin, const glm::vec3 &aabbMax, const glm::mat4 &rotation)
    {
        glm::vec3 localCenter = (aabbMin + aabbMax) * 0.5f;
        glm::vec3 halfExtents = (aabbMax - aabbMin) * 0.5f;

        // Translate to center first
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), localCenter);

        // OBB transform = rotation * translation
        // The order matters: scale -> rotation -> translation
        glm::mat4 obbTransform = rotation * translation;

        return {obbTransform, halfExtents};
    }

    bool TestOBBFrustum(const OBB &obb, const Frustum &frustum)
    {
        // Extract OBB axes and center
        const auto obbAxes = obb.GetAxes();
        const glm::vec3 obbCenter = obb.GetCenter();
        const glm::vec3 &halfExtents = obb.halfExtents;

        // Frustum planes
        const std::array<Plane, 6> frustumPlanes = {
            frustum.leftFace, frustum.rightFace,
            frustum.bottomFace, frustum.topFace,
            frustum.nearFace, frustum.farFace};

        // Test 1: OBB against frustum planes
        for (const Plane &plane : frustumPlanes)
        {
            // Compute projection interval radius of OBB onto plane normal
            float r = halfExtents.x * std::abs(glm::dot(plane.normal, obbAxes[0])) +
                      halfExtents.y * std::abs(glm::dot(plane.normal, obbAxes[1])) +
                      halfExtents.z * std::abs(glm::dot(plane.normal, obbAxes[2]));

            // Compute signed distance of OBB center from plane
            float distance = glm::dot(plane.normal, obbCenter) + plane.distance;

            if (distance < -r)
                return false; // OBB is completely outside this plane
        }

        // Optional: Test 2: Frustum edges against OBB faces (more thorough)
        // This catches cases where the frustum intersects the OBB but all OBB corners are outside

        return true;
    }

    bool TestAABBFrustum(const AABB &aabb, const Frustum &frustum)
    {
        // Convert AABB to center-extents form
        glm::vec3 center = (aabb.min + aabb.max) * 0.5f;
        glm::vec3 extents = (aabb.max - aabb.min) * 0.5f;

        // Test against each frustum plane
        const Plane *planes[] = {
            &frustum.leftFace, &frustum.rightFace,
            &frustum.bottomFace, &frustum.topFace,
            &frustum.nearFace, &frustum.farFace};

        for (const Plane *plane : planes)
        {
            // Compute the projection interval radius of b onto L(t) = c + t * p.n
            float r = extents.x * std::abs(plane->normal.x) +
                      extents.y * std::abs(plane->normal.y) +
                      extents.z * std::abs(plane->normal.z);

            // Compute signed distance of box center from plane
            float distance = glm::dot(plane->normal, center) + plane->distance;

            // Intersection fails if center is outside plane by more than r
            if (distance < -r)
                return false; // AABB is completely outside this plane
        }

        return true; // AABB intersects or is inside frustum
    }

    bool TestOBBFrustumHybrid(const OBB &obb, const Frustum &frustum)
    {
        // Fast AABB test first (broad phase)
        AABB aabbApprox = AABB::FromOBB(obb);
        if (!TestAABBFrustum(aabbApprox, frustum))
            return false;

        // If AABB passes, do more accurate OBB test
        return TestOBBFrustum(obb, frustum);
    }

    bool TestOBBFrustumCorners(const OBB &obb, const Frustum &frustum)
    {
        // Get all 8 OBB corners
        const auto corners = obb.GetCorners();

        // Frustum planes
        const std::array<Plane, 6> planes = {
            frustum.leftFace, frustum.rightFace,
            frustum.bottomFace, frustum.topFace,
            frustum.nearFace, frustum.farFace};

        // For each plane, test if all corners are outside
        for (const Plane &plane : planes)
        {
            bool allOutside = true;

            for (const glm::vec3 &corner : corners)
            {
                // Signed distance from corner to plane
                float distance = glm::dot(plane.normal, corner) + plane.distance;

                if (distance >= 0.0f) // Corner is inside or on plane
                {
                    allOutside = false;
                    break;
                }
            }

            // If all corners are outside any plane, OBB is outside frustum
            if (allOutside)
                return false;
        }

        // Could still be intersecting but not containing any corners
        // Add edge-edge tests here if needed for 100% accuracy

        return true;
    }
}