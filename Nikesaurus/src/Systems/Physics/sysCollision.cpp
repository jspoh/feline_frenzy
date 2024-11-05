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

        // Calculate the reflection vector for angular bounce
        Vector2f impulse = info.collision_normal.operator*(impulse_magnitude);

        if (collider_a.resolution == Physics::Resolution::NONE) {
            // Transform back outside of collision
            transform_b.position += info.mtv;

            // Apply impluse to velocity
            dynamics_b.velocity -= impulse;
        }
        else if (collider_b.resolution == Physics::Resolution::NONE) {
            // Transform back outside of collision
            transform_a.position += info.mtv;

            // Apply impluse to velocity
            dynamics_a.velocity += impulse;
        }
        else {
            // Transform back outside of collision
            transform_a.position += info.mtv.operator*(0.5f);
            transform_b.position -= info.mtv.operator*(0.5f);

            // Apply impulse to velocity based on mass
            dynamics_a.velocity += impulse.operator*(dynamics_b.mass / (dynamics_a.mass + dynamics_b.mass));
            dynamics_b.velocity -= impulse.operator*(dynamics_a.mass / (dynamics_a.mass + dynamics_b.mass));
        }
    }

    void Collision::System::setRestitution(float val) {
        restitution = val;
    }

    float Collision::System::getRestitution() const {
        return restitution;
    }

    bool Collision::System::detectAABBRectRect(
        Transform::Transform const& transform_a, Physics::Dynamics const& dynamics_a,
        Transform::Transform const& transform_b, Physics::Dynamics const& dynamics_b,
        CollisionInfo& info) {
        
        // References to components
        AABB aabb_a({ transform_a.position.x - (transform_a.scale.x * 0.5f), transform_a.position.y - (transform_a.scale.y * 0.5f) }, { transform_a.position.x + (transform_a.scale.x * 0.5f), transform_a.position.y + (transform_a.scale.y * 0.5f) });
        AABB aabb_b({ transform_b.position.x - (transform_b.scale.x * 0.5f), transform_b.position.y - (transform_b.scale.y * 0.5f) }, { transform_b.position.x + (transform_b.scale.x * 0.5f), transform_b.position.y + (transform_b.scale.y * 0.5f) });

        // Get delta time & set epsilon
        const float deltaTime = NIKE_ENGINE.getService<Windows::Service>()->getDeltaTime();
        const float epsilon = 0.0001f;

        // Step 1: Static collision detection
        if (!(aabb_a.rect_max.x < aabb_b.rect_min.x || aabb_a.rect_min.x > aabb_b.rect_max.x ||
            aabb_a.rect_max.y < aabb_b.rect_min.y || aabb_a.rect_min.y > aabb_b.rect_max.y)) {

            // Calculate overlaps on each axis
            float overlapX = Utility::getMin(aabb_a.rect_max.x - aabb_b.rect_min.x, aabb_b.rect_max.x - aabb_a.rect_min.x);
            float overlapY = Utility::getMin(aabb_a.rect_max.y - aabb_b.rect_min.y, aabb_b.rect_max.y - aabb_a.rect_min.y);

            // Determine MTV direction and smallest overlap
            Vector2f mtv_dir;
            if (overlapX < overlapY) {
                mtv_dir = { (aabb_a.rect_min.x < aabb_b.rect_min.x ? -1.0f : 1.0f), 0.0f };
            }
            else {
                mtv_dir = { 0.0f, (aabb_a.rect_min.y < aabb_b.rect_min.y ? -1.0f : 1.0f) };
            }

            // Set MTV and collision normal
            info.mtv = { mtv_dir.x * (overlapX < overlapY ? overlapX : overlapY), mtv_dir.y * (overlapX < overlapY ? overlapX : overlapY) };
            info.collision_normal = mtv_dir;

            info.t_first = 0.0f; //Static collision occurs
            return true;
        }

        // Step 2: Dynamic collision detection
        Vector2 velRel = { dynamics_a.velocity.x - dynamics_b.velocity.x, dynamics_a.velocity.y - dynamics_b.velocity.y };

        // Initialize time of first and last collision along each axis
        Vector2 tFirst = { 0.0f, 0.0f };
        Vector2 tLast = { deltaTime, deltaTime };

        // Step 3: Check dynamic collision on x-axis
        if (abs(velRel.x) > epsilon) {
            if (velRel.x > 0) {
                tFirst.x = (aabb_a.rect_min.x - aabb_b.rect_max.x) / velRel.x;
                tLast.x = (aabb_a.rect_max.x - aabb_b.rect_min.x) / velRel.x;
            }
            else {
                tFirst.x = (aabb_a.rect_max.x - aabb_b.rect_min.x) / velRel.x;
                tLast.x = (aabb_a.rect_min.x - aabb_b.rect_max.x) / velRel.x;
            }
        }
        else if (aabb_a.rect_max.x < aabb_b.rect_min.x || aabb_a.rect_min.x > aabb_b.rect_max.x) {
            return false; // No collision on the x-axis if there's no relative movement and no static collision
        }

        // Step 4: Check dynamic collision on y-axis
        if (abs(velRel.y) > epsilon) {
            if (velRel.y > 0) {
                tFirst.y = (aabb_a.rect_max.y - aabb_b.rect_min.y) / velRel.y;
                tLast.y = (aabb_a.rect_min.y - aabb_b.rect_max.y) / velRel.y;
            }
            else {
                tFirst.y = (aabb_a.rect_min.y - aabb_b.rect_max.y) / velRel.y;
                tLast.y = (aabb_a.rect_max.y - aabb_b.rect_min.y) / velRel.y;
            }
        }
        else if (aabb_a.rect_max.y < aabb_b.rect_min.y || aabb_a.rect_min.y > aabb_b.rect_max.y) {
            return false; // No collision on the y-axis if there's no relative movement and no static collision
        }

        // Step 5: Check if collisions occur within the time frame
        float tFirstOverall = Utility::getMax(tFirst.x, tFirst.y);
        float tLastOverall = Utility::getMin(tLast.x, tLast.y);

        if (tFirstOverall > tLastOverall || tFirstOverall > deltaTime) {
            return false; // No collision detected
        }

        // Update the time of first collision
        info.t_first = tFirstOverall;

        // Determine MTV and collision normal based on earliest axis of collision
        float smallestOverlap;
        Vector2f mtv_dir;
        if (abs(aabb_a.rect_min.y - aabb_b.rect_max.y)) {
            mtv_dir = { 1.0f, 0.0f };
            smallestOverlap = abs(aabb_a.rect_min.x - aabb_b.rect_max.x);
        }
        else {
            mtv_dir = { 0.0f, 1.0f };
            smallestOverlap = abs(aabb_a.rect_min.y - aabb_b.rect_max.y);
        }

        info.mtv = { mtv_dir.x * smallestOverlap, mtv_dir.y * smallestOverlap };
        info.collision_normal = mtv_dir;

        return true; // Collision detected
    }

    // SAT helper functions

    // Helper to retrieve and apply transformations to vertices based on model_id
    std::vector<Vector2f> Collision::System::getRotatedVertices(
        const Transform::Transform& transform, const std::string& model_id)
    {
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

        // Apply scaling and rotation
        float angleRad = transform.rotation * ((float)M_PI / 180.0f);  // Ensure degrees to radians

        float cosAngle = cos(angleRad);
        float sinAngle = sin(angleRad);
        Vector2f position = transform.position;
        Vector2f scale = transform.scale;

        for (Vector2f& vertex : vertices) {
            vertex.x *= scale.x;
            vertex.y *= scale.y;

            float rotatedX = vertex.x * cosAngle - vertex.y * sinAngle;
            float rotatedY = vertex.x * sinAngle + vertex.y * cosAngle;

            vertex.x = position.x + rotatedX;
            vertex.y = position.y + rotatedY;
        }
        return vertices;
    }


    std::vector<Vector2f> Collision::System::getSeparatingAxes(const std::vector<Vector2f>& verticesA, const std::vector<Vector2f>& verticesB)
    {
        std::vector<Vector2f> axes;

        auto addAxesFromEdges = [&](const std::vector<Vector2f>& vertices) {
            for (size_t i = 0; i < vertices.size(); ++i) {
                // Get the edge vector between consecutive vertices
                Vector2f edge = vertices[(i + 1) % vertices.size()] - vertices[i];
                Vector2f axis(-edge.y, edge.x);  // Perpendicular to the edge

                axis = axis.normalize();
                axes.push_back(axis);
            }
            };

        addAxesFromEdges(verticesA);
        addAxesFromEdges(verticesB);

        return axes;
    }


    void Collision::System::projectVerticesOnAxis(const std::vector<Vector2f>& vertices, const Vector2f& axis, float& min, float& max)
    {
        // Project vertices and find the min and max projections on the given axis
        min = max = axis.dot(vertices[0]);

        for (const Vector2f& vertex : vertices) {
            float projection = axis.dot(vertex);
            if (projection < min) min = projection;
            if (projection > max) max = projection;
        }

        // Adjust the overlap to give priority to edge alignments rather than vertices alone
        if (abs(min - max) < 0.01f) {
            min -= 0.01f;  // Slightly offset min to favor edge over vertex-only overlaps
        }
    }

    // Main detect SAT function
    bool Collision::System::detectSATCollision(
        const Transform::Transform& transformA, const Transform::Transform& transformB,
        const std::string& model_idA, const std::string& model_idB, CollisionInfo& info)
    {
        // Step 1: Get vertices and separating axes
        std::vector<Vector2f> verticesA = getRotatedVertices(transformA, model_idA);
        std::vector<Vector2f> verticesB = getRotatedVertices(transformB, model_idB);
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
            Vector2f relativePosition = transformA.position - transformB.position;
            float biasFactor = 0.001f;

            if (relativePosition.dot(smallestAxis) < 0) {
                // Vector2f currently no "flip" operator...
                smallestAxis = { -smallestAxis.x, -smallestAxis.y };  // Flip axis to ensure consistent direction away from the stationary object
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

        // Bounce Resolution
        if (collider_a.resolution == Physics::Resolution::BOUNCE || collider_b.resolution == Physics::Resolution::BOUNCE) {
            bounceResolution(transform_a, dynamics_a, collider_a, transform_b, dynamics_b, collider_b, info);
            return;
        }

        // Slide To Slide Resolution
        if (collider_a.resolution == Physics::Resolution::SLIDE && collider_b.resolution == Physics::Resolution::SLIDE) {
            transform_a.position += info.mtv * 0.5f;
            transform_b.position -= info.mtv * 0.5f;
            return;
        }

        // Resolution::NONE currently makes movement object "bounce"
        switch (collider_a.resolution) {
        case Physics::Resolution::NONE:
            break;
        case Physics::Resolution::SLIDE:
            transform_a.position += info.mtv;
            break;
        default:
            break;
        }
        
        switch (collider_b.resolution) {
        case Physics::Resolution::NONE:
            break;
        case Physics::Resolution::SLIDE:
            transform_b.position -= info.mtv;
            break;
        default:
            break;
        }
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
