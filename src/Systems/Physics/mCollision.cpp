/*****************************************************************//**
* \file   mCollision.cpp
* \brief  Implements the collision manager.
*
* \author Min Khant Ko
* \date   September 2024
*********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Systems/Physics/sysCollision.h"
#include "../headers/Systems/sysInput.h"

Collision::Manager::Manager() {
    /*colliders.resize(100);  // Pre-allocate 100 colliders
    for (auto& collider : colliders) {
        collider.active = false;  // Mark all colliders as unused at start
    }*/
}

Collision::Manager::~Manager() {
    /*colliders.clear();  // Clear the vector collection to free memory*/
}

/*
void Collision::Manager::registerCollider(const Vector2& position, const Vector2& size, float radius, const Vector2& velocity) {
    for (auto& collider : colliders) {
        if (!collider.active) {
            collider.active = true;
            collider.position = position;
            collider.size = size;
            collider.radius = radius;
            collider.velocity = velocity;
            collider.rect_min = { position.x - (size.x * 0.5f), position.y - (size.y * 0.5f) };
            collider.rect_max = { position.x + (size.x * 0.5f), position.y + (size.y * 0.5f) };
            return;
        }d
    }
    cout << "Error: No available colliders to register!" << endl;
}

void Collision::Manager::registerCollider(const Transform::Transform& transformT, const Transform::Velocity& transformV) {
    for (auto& collider : colliders) {
        if (!collider.active) {
            collider.active = true;
            collider.position = transformT.position;
            collider.size = transformT.scale;
            collider.radius = 0.0f;
            collider.velocity = transformV.velocity;
            collider.rect_min = { transformT.position.x - (transformT.scale.x * 0.5f), transformT.position.y - (transformT.scale.y * 0.5f) };
            collider.rect_max = { transformT.position.x + (transformT.scale.x * 0.5f), transformT.position.y + (transformT.scale.y * 0.5f) };
            return;
        }
    }
    cout << "Error: No available colliders to register!" << endl;
}

void Collision::Manager::unregisterCollider(const Vector2& position) {
    for (auto& collider : colliders) {
        //if (collider.active && collider.position == position) {  // Find the collider based on position // Note Vector2 does not have defined/overloaded == or != operators currently
        if (collider.active && (collider.position.x == position.x && collider.position.y == position.y)) {
            collider.active = false;
            return;
        }
    }
}

bool Collision::Manager::detectCollision(const Collider& a, const Collider& b) {
    // Placeholder for collision time
    float timeOfCollision = 0.0f;

    if (a.radius > 0.0f && b.radius > 0.0f) {
        return circleCollisionCheck(a.position, a.radius, b.position, b.radius);
    }
    else if (aabbRectRectCheck(a.velocity, b.velocity, a.rect_min, a.rect_max, b.rect_min, b.rect_max, timeOfCollision)) {
        return true;
    }
    else {
        return satCheck(a.position, a.size, b.position, b.size);
    }
}

bool Collision::Manager::detectCollision(const Transform::Transform& aT, const Transform::Velocity& aV, const Transform::Transform& bT, const Transform::Velocity& bV) {

    Collider* colliderA = nullptr;
    Collider* colliderB = nullptr;

    for (auto& collider : colliders) {
        if (collider.position.x == aT.position.x && collider.position.y == aT.position.y) {
            colliderA = &collider;  // Point to the matching collider for A
            collider.active = true;
        }
        else if (collider.position.x == bT.position.x && collider.position.y == bT.position.y) {
            colliderB = &collider;  // Point to the matching collider for B
            collider.active = true;
        }
    }

    // Colliders for A and B don't exist yet then register
    if (colliderA == nullptr) {
        registerCollider(aT, aV);
        for (auto& collider : colliders) {
            if (collider.position.x == aT.position.x && collider.position.y == aT.position.y) {
                colliderA = &collider;
                break;
            }
        }
    }
    if (colliderB == nullptr) {
        registerCollider(bT, bV);
        for (auto& collider : colliders) {
            if (collider.position.x == bT.position.x && collider.position.y == bT.position.y) {
                colliderB = &collider;
                break;
            }
        }
    }

    if (colliderA && colliderB) {
        return detectCollision(*colliderA, *colliderB);  // Note to dereference pointers to pass the colliders
    }

    return false;
}
*/

// Set collider component
bool Collision::Manager::setColliderComp(Entity::Type& cEntity) {
    // Check that entity has Transform component
    if (!NIKEEngine.checkEntityComponent<Transform::Transform>(cEntity)) {
        cout << "Entity's Transform not set" << endl;
        return false;
    }

    // Reference to Transform component
    Transform::Transform& eTransform = NIKEEngine.getEntityComponent<Transform::Transform>(cEntity);

    // Reference to Velocity component
    //Transform::Velocity& eVelocity = NIKEEngine.getEntityComponent<Transform::Velocity>(cEntity);

    Vector2 Rect_Min = { eTransform.position.x - (eTransform.scale.x * 0.5f), eTransform.position.y - (eTransform.scale.y * 0.5f) };
    Vector2 Rect_Max = { eTransform.position.x + (eTransform.scale.x * 0.5f), eTransform.position.y + (eTransform.scale.y * 0.5f) };

    // cCollider.h
    NIKEEngine.addEntityComponentObj<Collision::Collider>(cEntity, { true, 0.0f, Rect_Min, Rect_Max, false, false, false, false, {0.0f, 0.0f}, {0.0f, 0.0f} });

    return true;
}

template <typename T>
T Collision::Manager::getMax(const T a, const T b) const {
    if (a > b) {
        return a;
    }
    else {
        return b;
    }
}

template <typename T>
T Collision::Manager::getMin(const T a, const T b) const {
    if (a < b) {
        return a;
    }
    else {
        return b;
    }
}


bool Collision::Manager::circleCollisionCheck(const Vector2& pos_a, float radius_a, const Vector2& pos_b, float radius_b) const {
    float distX = pos_a.x - pos_b.x;
    float distY = pos_a.y - pos_b.y;
    float distance = std::sqrt(distX * distX + distY * distY);

    return (distance < (radius_a + radius_b));
}

bool Collision::Manager::aabbRectRectCheck(const Vector2& vel_a, const Vector2& vel_b,
    const Vector2& min_a, const Vector2& max_a,
    const Vector2& min_b, const Vector2& max_b,
    float& firstTimeOfCollision) const {
    // Step 1: Static collision detection
    // Check if the two AABBs are overlapping statically
    if (!(max_a.x < min_b.x || min_a.x > max_b.x ||
        max_a.y < min_b.y || min_a.y > max_b.y)) {
        // If there is an overlap in both x and y axes, a static collision occurs
        return true;
    }

    // Step 2: Calculate relative velocity for dynamic collision
    Vector2 velRel = { vel_a.x - vel_b.x, vel_a.y - vel_b.y };

    // Initialize time of first and last collision along each axis
    Vector2 tFirst = { 0.0f, 0.0f };
    Vector2 tLast = { NIKEEngine.accessWindow()->getDeltaTime(), NIKEEngine.accessWindow()->getDeltaTime() }; // Need a way to get time for previous frame
 
    // Step 3: Check dynamic collision on x-axis
    if (velRel.x != 0) {
        if (velRel.x > 0) {
            tFirst.x = (min_a.x - max_b.x) / velRel.x;
            tLast.x = (max_a.x - min_b.x) / velRel.x;
        }
        else {
            tFirst.x = (max_a.x - min_b.x) / velRel.x;
            tLast.x = (min_a.x - max_b.x) / velRel.x;
        }
    }
    else if (max_a.x < min_b.x || min_a.x > max_b.x) {
        return false; // No collision on the x-axis if there's no relative movement and no static collision
    }

    // Step 4: Check dynamic collision on y-axis
    if (velRel.y != 0) {
        if (velRel.y > 0) {
            tFirst.y = (min_a.y - max_b.y) / velRel.y;
            tLast.y = (max_a.y - min_b.y) / velRel.y;
        }
        else {
            tFirst.y = (max_a.y - min_b.y) / velRel.y;
            tLast.y = (min_a.y - max_b.y) / velRel.y;
        }
    }
    else if (max_a.y < min_b.y || min_a.y > max_b.y) {
        return false; // No collision on the y-axis if there's no relative movement and no static collision
    }

    // Step 5: Check if collisions occur within the time frame
    float tFirstOverall = getMax(tFirst.x, tFirst.y);
    float tLastOverall = getMin(tLast.x, tLast.y);

    // Ensure that the time of first collision occurs before the time of last collision
    if (tFirstOverall > tLastOverall || tFirstOverall > NIKEEngine.accessWindow()->getDeltaTime()) { // Need a way to get time for previous frame
        return false; // No collision detected
    }

    // If we have a valid time of collision, update the output parameter
    firstTimeOfCollision = tFirstOverall;

    return true; // Collision detected
}

bool Collision::Manager::detectAABBRectRect(Entity::Type& cEntityA, Entity::Type& cEntityB, float& firstTimeOfCollision) {
    // Check if both entities have the required components
    if (!NIKEEngine.checkEntityComponent<Transform::Transform>(cEntityA) || !NIKEEngine.checkEntityComponent<Collision::Collider>(cEntityA) ||
        !NIKEEngine.checkEntityComponent<Transform::Transform>(cEntityB) || !NIKEEngine.checkEntityComponent<Collision::Collider>(cEntityB)) {
        cout << "Entity's Transform or Collider not set" << endl;
        return false;
    }

    // References to components
    Transform::Velocity& eVelocityA = NIKEEngine.getEntityComponent<Transform::Velocity>(cEntityA);
    Transform::Velocity& eVelocityB = NIKEEngine.getEntityComponent<Transform::Velocity>(cEntityB);
    Collision::Collider& eColliderA = NIKEEngine.getEntityComponent<Collision::Collider>(cEntityA);
    Collision::Collider& eColliderB = NIKEEngine.getEntityComponent<Collision::Collider>(cEntityB);

    const float deltaTime = NIKEEngine.accessWindow()->getDeltaTime();
    const float epsilon = 0.0001f;

    // Step 1: Static collision detection
    bool staticCollision = !(eColliderA.rect_max.x < eColliderB.rect_min.x || eColliderA.rect_min.x > eColliderB.rect_max.x ||
        eColliderA.rect_max.y < eColliderB.rect_min.y || eColliderA.rect_min.y > eColliderB.rect_max.y);

    if (staticCollision) {
        cout << "Collision detected between EntityA and EntityB (Static collision)" << endl;

        // Calculate overlap in both axes
        float overlapX = getMin(eColliderA.rect_max.x - eColliderB.rect_min.x, eColliderB.rect_max.x - eColliderA.rect_min.x);
        float overlapY = getMin(eColliderA.rect_max.y - eColliderB.rect_min.y, eColliderB.rect_max.y - eColliderA.rect_min.y);

        // Determine side of collision based on smallest overlap
        if (overlapX < overlapY) {
            if (eColliderA.rect_min.x < eColliderB.rect_max.x && eColliderA.rect_max.x > eColliderB.rect_min.x) {
                // Collision primarily on the x-axis
                if (eColliderA.rect_min.x < eColliderB.rect_min.x) {
                    eColliderA.right = true;
                    cout << "EntityA is blocked on the right by EntityB" << endl;
                }
                else {
                    eColliderA.left = true;
                    cout << "EntityA is blocked on the left by EntityB" << endl;
                }
            }
        }
        else {
            if (eColliderA.rect_min.y < eColliderB.rect_max.y && eColliderA.rect_max.y > eColliderB.rect_min.y) {
                // Collision primarily on the y-axis
                if (eColliderA.rect_min.y < eColliderB.rect_min.y) {
                    eColliderA.top = true;
                    cout << "EntityA is blocked on the top by EntityB" << endl;
                }
                else {
                    eColliderA.bottom = true;
                    cout << "EntityA is blocked at the bottom by EntityB" << endl;
                }
            }
        }
        return true;
    }

    // Step 2: Dynamic collision detection
    Vector2 velRel = { eVelocityA.velocity.x - eVelocityB.velocity.x, eVelocityA.velocity.y - eVelocityB.velocity.y };

    // Initialize time of first and last collision along each axis
    Vector2 tFirst = { 0.0f, 0.0f };
    Vector2 tLast = { deltaTime, deltaTime };

    // Step 3: Check dynamic collision on x-axis
    if (abs(velRel.x) > epsilon) {
        if (velRel.x > 0) {
            tFirst.x = (eColliderA.rect_min.x - eColliderB.rect_max.x) / velRel.x;
            tLast.x = (eColliderA.rect_max.x - eColliderB.rect_min.x) / velRel.x;
        }
        else {
            tFirst.x = (eColliderA.rect_max.x - eColliderB.rect_min.x) / velRel.x;
            tLast.x = (eColliderA.rect_min.x - eColliderB.rect_max.x) / velRel.x;
        }
    }
    else if (eColliderA.rect_max.x < eColliderB.rect_min.x || eColliderA.rect_min.x > eColliderB.rect_max.x) {
        return false; // No collision on the x-axis if there's no relative movement and no static collision
    }

    // Step 4: Check dynamic collision on y-axis
    if (abs(velRel.y) > epsilon) {
        if (velRel.y > 0) {
            tFirst.y = (eColliderA.rect_max.y - eColliderB.rect_min.y) / velRel.y;
            tLast.y = (eColliderA.rect_min.y - eColliderB.rect_max.y) / velRel.y;
        }
        else {
            tFirst.y = (eColliderA.rect_min.y - eColliderB.rect_max.y) / velRel.y;
            tLast.y = (eColliderA.rect_max.y - eColliderB.rect_min.y) / velRel.y;
        }
    }
    else if (eColliderA.rect_max.y < eColliderB.rect_min.y || eColliderA.rect_min.y > eColliderB.rect_max.y) {
        return false; // No collision on the y-axis if there's no relative movement and no static collision
    }

    // Step 5: Check if collisions occur within the time frame
    float tFirstOverall = getMax(tFirst.x, tFirst.y);
    float tLastOverall = getMin(tLast.x, tLast.y);

    if (tFirstOverall > tLastOverall || tFirstOverall > deltaTime) {
        return false; // No collision detected
    }

    // Update the time of first collision
    firstTimeOfCollision = tFirstOverall;

    // Step 6: Determine which side of both entities is being blocked based on the primary axis
    cout << "Collision detected between EntityA and EntityB (Dynamic collision)" << endl;

    if (tFirst.x < tFirst.y) {
        // Collision primarily along the x-axis
        if (velRel.x > 0) {
            eColliderA.right = true; // 'a' is blocked on the right side
            cout << "EntityA is blocked on the right side by EntityB" << endl;
        }
        else {
            eColliderA.left = true;  // 'a' is blocked on the left side
            cout << "EntityA is blocked on the left side by EntityB" << endl;
        }
    }
    else {
        // Collision primarily along the y-axis
        if (velRel.y > 0) {
            eColliderA.top = true;    // 'a' is blocked on the top side
            cout << "EntityA is blocked on the top by EntityB" << endl;
        }
        else {
            eColliderA.bottom = true; // 'a' is blocked on the bottom side
            cout << "EntityA is blocked at the bottom by EntityB" << endl;
        }
    }

    return true; // Collision detected
}



bool Collision::Manager::satCheck(const Vector2& pos_a, const Vector2& size_a, const Vector2& pos_b, const Vector2& size_b) const {
    // Calculate the half extents for each box
    Vector2 half_extent_a(size_a.x * 0.5f, size_a.y * 0.5f);
    Vector2 half_extent_b(size_b.x * 0.5f, size_b.y * 0.5f);

    // Calculate the center points of both boxes
    Vector2 center_a = pos_a + half_extent_a;
    Vector2 center_b = pos_b + half_extent_b;

    // Calculate the distance between the centers
    Vector2 dist = center_b - center_a;

    // Calculate overlap on the x-axis
    float overlap_x = half_extent_a.x + half_extent_b.x - std::abs(dist.x);
    if (overlap_x <= 0) return false;  // No overlap on x-axis, so no collision

    // Calculate overlap on the y-axis
    float overlap_y = half_extent_a.y + half_extent_b.y - std::abs(dist.y);
    if (overlap_y <= 0) return false;  // No overlap on y-axis, so no collision

    // If we get here, both axes overlap, so a collision exists
    return true;
}

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

