/*****************************************************************//**
* \file   sysCollision.cpp
* \brief  Implements the collision manager.
*
* \author Min Khant Ko, 2301320, ko.m@digipen.edu (100%)
* \date   September 2024
* All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
*********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Systems/Physics/sysCollision.h"

namespace NIKE {

    void Collision::System::bounceResolution(
        Transform::Transform& transform_a, Physics::Dynamics& dynamics_a, Physics::Collider& collider_a,
        Transform::Transform& transform_b, Physics::Dynamics& dynamics_b, Physics::Collider& collider_b,
        CollisionInfo const& info) {

        // Calculate relative velocity
        Vector2f vel_rel = dynamics_a.velocity - dynamics_b.velocity;
        float normal_vel = vel_rel.dot(info.collision_normal);

        // Check if entities are already moving apart
        if (normal_vel > 0) return;

        // Calculate impulse magnitude based on collision response
        float impulse_magnitude = -(1 + restitution) * normal_vel;
        Vector2f impulse = info.collision_normal * impulse_magnitude;

        if (collider_a.resolution == Physics::Resolution::NONE) {
            // Move collider B back
            collider_b.position += info.mtv;

            // Apply impulse
            dynamics_b.velocity -= impulse;

            // Update Transform B
            transform_b.position = collider_b.position - collider_b.offset;
        }
        else if (collider_b.resolution == Physics::Resolution::NONE) {
            // Move collider A back
            collider_a.position += info.mtv;

            // Apply impulse
            dynamics_a.velocity += impulse;

            // Update Transform A
            transform_a.position = collider_a.position - collider_a.offset;
        }
        else {
            // Split MTV between A and B
            collider_a.position += info.mtv * 0.5f;
            collider_b.position -= info.mtv * 0.5f;

            // Apply impulse based on mass
            dynamics_a.velocity += impulse * (dynamics_b.mass / (dynamics_a.mass + dynamics_b.mass));
            dynamics_b.velocity -= impulse * (dynamics_a.mass / (dynamics_a.mass + dynamics_b.mass));

            // Update Transform positions
            transform_a.position = collider_a.position - collider_a.offset;
            transform_b.position = collider_b.position - collider_b.offset;
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
            collider_a.position.x - (collider_a.size.x * 0.5f),
            collider_a.position.y - (collider_a.size.y * 0.5f)
        }, {
            collider_a.position.x + (collider_a.size.x * 0.5f),
            collider_a.position.y + (collider_a.size.y * 0.5f)
        });

        AABB aabb_b({
            collider_b.position.x - (collider_b.size.x * 0.5f),
            collider_b.position.y - (collider_b.size.y * 0.5f)
        }, {
            collider_b.position.x + (collider_b.size.x * 0.5f),
            collider_b.position.y + (collider_b.size.y * 0.5f)
        });

        // Get delta time
        const float deltaTime = NIKE_ENGINE.getService<Windows::Service>()->getDeltaTime();

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
        Vector2f t_last = { deltaTime, deltaTime };

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

        if (t_first_overall > t_last_overall || t_first_overall > deltaTime) {
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

        if (model_id == "triangle") {
            vertices = {
                Vector2f(-0.5f, -0.5f),
                Vector2f(0.5f, -0.5f),
                Vector2f(0.0f, 0.5f)
            };
        }
        else if (model_id == "square" || model_id == "square-texture") {
            vertices = {
                Vector2f(0.5f, -0.5f),
                Vector2f(0.5f, 0.5f),
                Vector2f(-0.5f, 0.5f),
                Vector2f(-0.5f, -0.5f)
            };
        }

        // Apply scaling and rotation based on Collider properties
        float angleRad = collider.rotation * static_cast<float>(M_PI) / 180.0f; // Convert degrees to radians
        float cosAngle = cos(angleRad);
        float sinAngle = sin(angleRad);

        Vector2f position = collider.position; // Collider's position is independent of Transform
        Vector2f scale = collider.size;

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
            Vector2f relativePosition = colliderA.position - colliderB.position;
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
        Transform::Transform& transform_a, Physics::Dynamics& dynamics_a, Physics::Collider& collider_a,
        Transform::Transform& transform_b, Physics::Dynamics& dynamics_b, Physics::Collider& collider_b,
        CollisionInfo const& info) {

        cout << "collision detected" << endl;

        if (collider_a.is_trigger || collider_b.is_trigger) return;

        if (collider_a.resolution == Physics::Resolution::BOUNCE || collider_b.resolution == Physics::Resolution::BOUNCE) {
            bounceResolution(transform_a, dynamics_a, collider_a, transform_b, dynamics_b, collider_b, info);
            return;
        }

        // Apply MTV to Collider
        collider_a.position += info.mtv * 0.5f;
        collider_b.position -= info.mtv * 0.5f;

        // Update Transform to reflect Collider's new state
        collider_a.syncTransform(transform_a);
        collider_b.syncTransform(transform_b);

    }




    /* Temporary storage for mouse click detection functions
        // Detect if the mouse is inside a rectangular area
        bool Collision::Manager::detectMClickRect(const Vector2& center, float width, float height) {
        // Get the mouse position from Input::Manager
        const Input::Mouse mouse = Input::Manager::getInstance()->getMouse();
        float mouseX = mouse.button_pos.x;
        float mouseY = mouse.button_pos.y;

        // Calculate the boundaries of the rectangle
        float left = center.x - (width * 0.5f);
        float right = center.x + (width * 0.5f);
        float top = center.y - (height * 0.5f);
        float bottom = center.y + (height * 0.5f);

        // Check if the mouse is inside the rectangle
        if (mouseX >= left && mouseX <= right && mouseY >= top && mouseY <= bottom) {

            // Check if the mouse is exactly at the center
            if (mouseX == center.x && mouseY == center.y) {
                cout << "Mouse is exactly at the center of the rectangle." << endl;
            }
            else {
                // Print where the mouse is relative to the center of the rectangle
                if (mouseX < center.x)
                    cout << "Mouse is on the left side of the center." << endl;
                else
                    cout << "Mouse is on the right side of the center." << endl;

                if (mouseY < center.y)
                    cout << "Mouse is above the center." << endl;
                else
                    cout << "Mouse is below the center." << endl;
            }

            return true;
        }

        return false;
    }


    // Detect if the mouse is inside a circular area
    bool Collision::Manager::detectMClickCircle(const Vector2& center, float radius) {

        // Get the mouse position from Input::Manager
        const Input::Mouse mouse = Input::Manager::getInstance()->getMouse();
        float mouseX = mouse.button_pos.x;
        float mouseY = mouse.button_pos.y;

        // Calculate the distance from the mouse to the center of the circle
        float distX = mouseX - center.x;
        float distY = mouseY - center.y;
        float distance = std::sqrt(distX * distX + distY * distY);

        // Check if the mouse is inside the circle
        if (distance <= radius) {
            cout << "Mouse is inside the circle." << endl;
            return true;
        }

        return false;
    }
    */

}
