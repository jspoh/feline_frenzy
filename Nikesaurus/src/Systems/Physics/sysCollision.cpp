/*****************************************************************//**
* \file   sysCollision.cpp
* \brief  Implements the collision manager.
*
* \author Min Khant Ko, 2301320, ko.m@digipen.edu (100%)
* \date   September 2024
* All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
*********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Systems/Physics/sysCollision.h"

namespace NIKE {

    void Collision::System::bounceResolution(
        Transform::Transform& transform_a, Physics::Dynamics& dynamics_a, Physics::Collider& collider_a,
        Transform::Transform& transform_b, Physics::Dynamics& dynamics_b, Physics::Collider& collider_b,
        CollisionInfo const& info) {

        // Step 1: Validate and normalize the collision normal
        float normal_magnitude = std::sqrt(info.collision_normal.x * info.collision_normal.x +
            info.collision_normal.y * info.collision_normal.y);

        // Skip if the normal's magnitude is too small (invalid)
        if (normal_magnitude < EPSILON) return;

        // Normalize the collision normal
        Vector2f collision_normal = {
            info.collision_normal.x / normal_magnitude,
            info.collision_normal.y / normal_magnitude
        };

        // Step 2: Reflect the velocity of entity A
        float normal_vel = dynamics_a.velocity.x * collision_normal.x + dynamics_a.velocity.y * collision_normal.y;

        // Reflect velocity only if it's moving toward the collision normal
        if (normal_vel < 0.0f) {
            dynamics_a.velocity.x -= 2.0f * normal_vel * collision_normal.x;
            dynamics_a.velocity.y -= 2.0f * normal_vel * collision_normal.y;

            // Add a small nudge to prevent sticking when velocity becomes too small
            if (std::abs(collision_normal.x) > 0.9f && std::abs(dynamics_a.velocity.x) < EPSILON) {
                dynamics_a.velocity.x = (collision_normal.x > 0 ? EPSILON : -EPSILON);
            }
            if (std::abs(collision_normal.y) > 0.9f && std::abs(dynamics_a.velocity.y) < EPSILON) {
                dynamics_a.velocity.y = (collision_normal.y > 0 ? EPSILON : -EPSILON);
            }
        }

        // Step 3: Reflect the force of entity A (if forces are used)
        float normal_force = dynamics_a.force.x * collision_normal.x + dynamics_a.force.y * collision_normal.y;

        if (normal_force < 0.0f) {
            dynamics_a.force.x -= 2.0f * normal_force * collision_normal.x;
            dynamics_a.force.y -= 2.0f * normal_force * collision_normal.y;
        }

        // Step 4: Clamp velocity components to max speed
        dynamics_a.velocity.x = std::clamp(dynamics_a.velocity.x, -dynamics_a.max_speed, dynamics_a.max_speed);
        dynamics_a.velocity.y = std::clamp(dynamics_a.velocity.y, -dynamics_a.max_speed, dynamics_a.max_speed);

        // Step 5: Adjust position based on MTV
        if (collider_a.resolution != Physics::Resolution::NONE) {
            transform_a.position.x += info.mtv.x;
            transform_a.position.y += info.mtv.y;

            // Step 6: Apply an additional small offset to fully resolve overlap
            const float small_offset = 0.01f; // Tiny offset to move entity fully out of collision bounds
            transform_a.position.x += collision_normal.x * small_offset;
            transform_a.position.y += collision_normal.y * small_offset;
        }
    }



    void Collision::System::setRestitution(float val) {
        restitution = val;
    }

    float Collision::System::getRestitution() const {
        return restitution;
    }

    bool Collision::System::detectAABBRectRect(
        Physics::Dynamics const& dynamics_a, Physics::Collider const& collider_a,
        Physics::Dynamics const& dynamics_b, Physics::Collider const& collider_b,
        CollisionInfo& info) {
        
        // References to components
        AABB aabb_a({
            collider_a.transform.position.x - (collider_a.transform.scale.x * 0.5f),
            collider_a.transform.position.y - (collider_a.transform.scale.y * 0.5f)
        }, {
            collider_a.transform.position.x + (collider_a.transform.scale.x * 0.5f),
            collider_a.transform.position.y + (collider_a.transform.scale.y * 0.5f)
        });

        AABB aabb_b({
            collider_b.transform.position.x - (collider_b.transform.scale.x * 0.5f),
            collider_b.transform.position.y - (collider_b.transform.scale.y * 0.5f)
        }, {
            collider_b.transform.position.x + (collider_b.transform.scale.x * 0.5f),
            collider_b.transform.position.y + (collider_b.transform.scale.y * 0.5f)
        });

        // Get delta time
        const float delta_time = NIKE_ENGINE.getService<Windows::Service>()->getFixedDeltaTime();

        // Step 1: Static collision detection
        if (!(aabb_a.rect_max.x < aabb_b.rect_min.x || aabb_a.rect_min.x > aabb_b.rect_max.x ||
            aabb_a.rect_max.y < aabb_b.rect_min.y || aabb_a.rect_min.y > aabb_b.rect_max.y)) {

            // Calculate overlaps on each axis
            float overlapX = Utility::getMin(
                aabb_a.rect_max.x - aabb_b.rect_min.x, aabb_b.rect_max.x - aabb_a.rect_min.x);
            float overlapY = Utility::getMin(
                aabb_a.rect_max.y - aabb_b.rect_min.y, aabb_b.rect_max.y - aabb_a.rect_min.y);

            // Determine MTV direction and smallest overlap
            Vector2f mtv_dir;
            if (overlapX < overlapY) {
                mtv_dir = { (aabb_a.rect_min.x < aabb_b.rect_min.x ? -1.0f : 1.0f), 0.0f };
            }
            else {
                mtv_dir = { 0.0f, (aabb_a.rect_min.y < aabb_b.rect_min.y ? -1.0f : 1.0f) };
            }

            // Set MTV and collision normal
            info.mtv = {
                mtv_dir.x * (overlapX < overlapY ? overlapX : overlapY),
                mtv_dir.y * (overlapX < overlapY ? overlapX : overlapY)
            };
            info.collision_normal = mtv_dir;

            info.t_first = 0.0f; // Static collision occurs
            return true;
        }

        // Step 2: Dynamic collision detection
        Vector2f vel_rel = dynamics_a.velocity - dynamics_b.velocity;

        // Initialize time of first and last collision along each axis
        Vector2f t_first = { 0.0f, 0.0f };
        Vector2f t_last = { delta_time, delta_time };

        // Check dynamic collision along x-axis
        if (std::abs(vel_rel.x) > EPSILON) {
            if (vel_rel.x > 0) {
                t_first.x = (aabb_a.rect_min.x - aabb_b.rect_max.x) / vel_rel.x;
                t_last.x = (aabb_a.rect_max.x - aabb_b.rect_min.x) / vel_rel.x;
            }
            else {
                t_first.x = (aabb_a.rect_max.x - aabb_b.rect_min.x) / vel_rel.x;
                t_last.x = (aabb_a.rect_min.x - aabb_b.rect_max.x) / vel_rel.x;
            }
        }
        else if (aabb_a.rect_max.x < aabb_b.rect_min.x || aabb_a.rect_min.x > aabb_b.rect_max.x) {
            return false; // No collision along x-axis
        }

        // Check dynamic collision along y-axis
        if (std::abs(vel_rel.y) > EPSILON) {
            if (vel_rel.y > 0) {
                t_first.y = (aabb_a.rect_max.y - aabb_b.rect_min.y) / vel_rel.y;
                t_last.y = (aabb_a.rect_min.y - aabb_b.rect_max.y) / vel_rel.y;
            }
            else {
                t_first.y = (aabb_a.rect_min.y - aabb_b.rect_max.y) / vel_rel.y;
                t_last.y = (aabb_a.rect_max.y - aabb_b.rect_min.y) / vel_rel.y;
            }
        }
        else if (aabb_a.rect_max.y < aabb_b.rect_min.y || aabb_a.rect_min.y > aabb_b.rect_max.y) {
            return false; // No collision along y-axis
        }

        // Verify if collisions occur within the time frame
        float t_first_overall = Utility::getMax(t_first.x, t_first.y);
        float t_last_overall = Utility::getMin(t_last.x, t_last.y);

        if (t_first_overall > t_last_overall || t_first_overall > delta_time) {
            return false; // No collision detected
        }

        // Update collision info
        info.t_first = t_first_overall;
        info.collision_normal = vel_rel.normalize();
        return true;
    }

    // SAT helper functions

    // Helper to retrieve and apply transformations to vertices based on model_id
    std::vector<Vector2f> Collision::System::getRotatedVertices(
        const Physics::Collider& collider, const std::string& model_id)
    {
        // Initialize vertex list based on model type
        std::vector<Vector2f> vertices;

        // quad models
        constexpr std::array<const char*, 4> QUAD_MODELS {
            "square.model",
            "square-texture.model",
            "batched_square.model",
            "batched_texture.model"
        };

        if (model_id == "triangle.model") {
            vertices = {
                Vector2f(-0.5f, -0.5f),
                Vector2f(0.5f, -0.5f),
                Vector2f(0.0f, 0.5f)
            };
        }
        else if (std::find(QUAD_MODELS.begin(), QUAD_MODELS.end(), model_id) != QUAD_MODELS.end()) {
            vertices = {
                Vector2f(0.5f, -0.5f),
                Vector2f(0.5f, 0.5f),
                Vector2f(-0.5f, 0.5f),
                Vector2f(-0.5f, -0.5f)
            };
        }
        //Fallback default to square
        else {
            vertices = {
                Vector2f(0.5f, -0.5f),
                Vector2f(0.5f, 0.5f),
                Vector2f(-0.5f, 0.5f),
                Vector2f(-0.5f, -0.5f)
            };
        }

        // Apply scaling and rotation based on Collider properties
        float angleRad = collider.transform.rotation * static_cast<float>(M_PI) / 180.0f; // Convert degrees to radians
        float cosAngle = cos(angleRad);
        float sinAngle = sin(angleRad);

        Vector2f position = collider.transform.position; // Collider's position is independent of Transform
        Vector2f scale = collider.transform.scale;

        for (Vector2f& vertex : vertices) {
            // Scale the vertex
            vertex.x *= scale.x;
            vertex.y *= scale.y;

            // Rotate the vertex
            float rotatedX = vertex.x * cosAngle - vertex.y * sinAngle;
            float rotatedY = vertex.x * sinAngle + vertex.y * cosAngle;

            // Translate to Collider's position
            vertex.x = position.x + rotatedX;
            vertex.y = position.y + rotatedY;
        }

        return vertices;
    }


    // Helper to retrieve separating axes from two sets of vertices
    std::vector<Vector2f> Collision::System::getSeparatingAxes(
        const std::vector<Vector2f>& verticesA, const std::vector<Vector2f>& verticesB)
    {
        std::vector<Vector2f> axes;

        // Lambda to extract perpendicular axes from edges
        auto addAxesFromEdges = [&](const std::vector<Vector2f>& vertices) {
            for (size_t i = 0; i < vertices.size(); ++i) {
                // Calculate the edge vector
                Vector2f edge = vertices[(i + 1) % vertices.size()] - vertices[i];
                Vector2f axis(-edge.y, edge.x); // Perpendicular to the edge

                // Normalize the axis
                axis = axis.normalize();
                axes.push_back(axis);
            }
            };

        // Extract axes from both shapes
        addAxesFromEdges(verticesA);
        addAxesFromEdges(verticesB);

        return axes;
    }

    // Helper to project vertices onto a given axis and find the min/max projection values
    void Collision::System::projectVerticesOnAxis(
        const std::vector<Vector2f>& vertices, const Vector2f& axis, float& min, float& max)
    {
        // Project the first vertex
        min = max = axis.dot(vertices[0]);

        // Project all vertices onto the axis
        for (const Vector2f& vertex : vertices) {
            float projection = axis.dot(vertex);

            // Update min and max projections
            if (projection < min) min = projection;
            if (projection > max) max = projection;
        }

        // Adjust overlap to favor edges over vertex-only overlaps
        if (std::abs(min - max) < 0.01f) {
            min -= 0.01f; // Slight offset for better edge prioritization
        }
    }


    // Main detect SAT function
    bool Collision::System::detectSATCollision(
        const Physics::Collider& colliderA, const Physics::Collider& colliderB,
        const std::string& model_idA, const std::string& model_idB, CollisionInfo& info)
    {
        // Step 1: Get vertices and separating axes
        std::vector<Vector2f> verticesA = getRotatedVertices(colliderA, model_idA);
        std::vector<Vector2f> verticesB = getRotatedVertices(colliderB, model_idB);
        std::vector<Vector2f> axes = getSeparatingAxes(verticesA, verticesB);

        Vector2f smallestAxis;
        bool collisionDetected = true;
        float minOverlap = FLT_MAX;

        // Step 2: Project shapes onto each axis to find overlap
        for (const Vector2f& axis : axes) {
            float minA, maxA, minB, maxB;
            projectVerticesOnAxis(verticesA, axis, minA, maxA);
            projectVerticesOnAxis(verticesB, axis, minB, maxB);

            // Check for separation
            if (maxA < minB || maxB < minA) {
                collisionDetected = false;
                break;
            }

            // Calculate overlap distance
            float overlap = Utility::getMin(maxA, maxB) - Utility::getMax(minA, minB);

            // Track the smallest overlap
            if (overlap < minOverlap) {
                minOverlap = overlap;
                smallestAxis = axis;
            }
        }

        // Step 3: If collision is detected, assign the MTV and collision normal
        if (collisionDetected) {
            // Calculate consistent MTV direction based on relative position
            Vector2f relativePosition = colliderA.transform.position - colliderB.transform.position;
            float biasFactor = 0.001f;

            if (relativePosition.dot(smallestAxis) < 0) {
                // Flip axis to ensure consistent direction away from the stationary object
                smallestAxis = { -smallestAxis.x, -smallestAxis.y };
            }

            // Adjust MTV and normal
            info.mtv = (smallestAxis * minOverlap) + (smallestAxis * biasFactor);
            info.collision_normal = smallestAxis.normalize();
        }

        return collisionDetected;
    }


    void Collision::System::collisionResolution(
        Entity::Type entity_a, Transform::Transform& transform_a, Physics::Dynamics& dynamics_a, Physics::Collider& collider_a,
        Entity::Type entity_b, Transform::Transform& transform_b, Physics::Dynamics& dynamics_b, Physics::Collider& collider_b,
        CollisionInfo const& info) {

        // Step 1: Handle DESTROY resolution
        // If either entity is set to be destroyed upon collision, mark it for deletion
        if (collider_a.resolution == Physics::Resolution::DESTROY) {
            NIKE_ECS_MANAGER->markEntityForDeletion(entity_a);
            return;
        }
        if (collider_b.resolution == Physics::Resolution::DESTROY) {
            NIKE_ECS_MANAGER->markEntityForDeletion(entity_b);
            return;
        }

        // Step 2: Handle BOUNCE resolution
        // If either entity has BOUNCE resolution, apply bounce logic
        if (collider_a.resolution == Physics::Resolution::BOUNCE || collider_b.resolution == Physics::Resolution::BOUNCE) {
            if (collider_a.resolution == Physics::Resolution::BOUNCE) {
                // Bounce entity A off entity B
                bounceResolution(transform_a, dynamics_a, collider_a, transform_b, dynamics_b, collider_b, info);
            }
            else {
                // Bounce entity B off entity A
                bounceResolution(transform_b, dynamics_b, collider_b, transform_a, dynamics_a, collider_a, info);
            }
            return;
        }

        // Step 3: Skip NONE resolution adjustments
        // If both entities have NONE resolution, no physics adjustments are applied
        if (collider_a.resolution == Physics::Resolution::NONE && collider_b.resolution == Physics::Resolution::NONE) return;

        // Step 4: Handle SLIDE resolution for entity A
        if (collider_a.resolution == Physics::Resolution::SLIDE) {
            // Remove the velocity component along the collision normal (sliding effect)
            dynamics_a.velocity -= dynamics_a.velocity.dot(info.collision_normal) * info.collision_normal;

            // Adjust position based on the minimum translation vector (MTV)
            transform_a.position += info.mtv;
        }

        // Step 5: Handle SLIDE resolution for entity B
        if (collider_b.resolution == Physics::Resolution::SLIDE) {
            // Remove the velocity component along the collision normal (sliding effect)
            dynamics_b.velocity -= dynamics_b.velocity.dot(info.collision_normal) * info.collision_normal;

            // Adjust position based on the minimum translation vector (MTV)
            transform_b.position -= info.mtv;
        }
    }


}
