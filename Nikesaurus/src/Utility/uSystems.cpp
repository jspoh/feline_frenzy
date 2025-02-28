/*****************************************************************//**
 * \file   uSystems.cpp
 * \brief	Utility systems
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu(100%)
 * \date   October 2024
 * All content 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Utility/uSystems.h"

namespace NIKE {
	Utility::Clock::Clock()
		: start{ std::chrono::high_resolution_clock::now() }, duration{ 0.0f }
	{
	}

	float Utility::Clock::getElapsedTime() {
		duration = std::chrono::high_resolution_clock::now() - start;
		return duration.count();
	}

	void Utility::Clock::restartClock() {
		start = std::chrono::high_resolution_clock::now();
	}

	float Utility::randFloat() {
		return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	}

	std::string Utility::convertTypeString(std::string&& str_type) {
		return str_type.substr(str_type.find_first_not_of(':', str_type.find_first_of(':')), str_type.size() - str_type.find_first_not_of(':', str_type.find_first_of(':')));
	}

	std::vector<Vector2f> Utility::convertTransformToVert(Transform::Transform const& e_transform) {

		//Calculate angle in radians
		float angle_rad = e_transform.rotation * ((float)M_PI / 180.0f);
		float s = sin(angle_rad);
		float c = cos(angle_rad);

		//Calculate half scale
		Vector2f half_scale = { e_transform.scale.x / 2.0f, e_transform.scale.y / 2.0f };

		//Get corners before rotation
		std::vector<Vector2f> corners = {
			{ -half_scale.x, half_scale.y },
			{  half_scale.x, half_scale.y },
			{  half_scale.x, -half_scale.y },
			{ -half_scale.x, -half_scale.y }
		};

		//Rotate vertices
		for (int i = 0; i < corners.size(); ++i) {
			float rotated_x = corners[i].x * c - corners[i].y * s + e_transform.position.x;
			float rotated_y = corners[i].x * s + corners[i].y * c + e_transform.position.y;
			corners[i] = { rotated_x, rotated_y };
		}

		return corners;
	}

	bool Utility::isCursorInTransform(Vector2f const& mouse_pos, Transform::Transform const& transform) {
		//Convert transform to vertices
		auto corners = Utility::convertTransformToVert(transform);

		// Ray-casting algorithm to check if point is inside polygon
		int intersectCount = 0;
		for (size_t i = 0; i < corners.size(); i++) {
			Vector2f v1 = corners[i];
			Vector2f v2 = corners[(i + 1) % corners.size()];  // Wrap to the first vertex for the last edge

			// Check if the ray intersects the edge
			bool isEdgeCrossing = ((v1.y > mouse_pos.y) != (v2.y > mouse_pos.y));
			if (isEdgeCrossing) {
				float intersectionX = v1.x + (mouse_pos.y - v1.y) * (v2.x - v1.x) / (v2.y - v1.y);
				if (mouse_pos.x < intersectionX) {
					intersectCount++;
				}
			}
		}

		// If intersect count is odd, the point is inside
		return (intersectCount % 2) == 1;
	}

	std::string Utility::extractFileName(const std::string& file_path)
	{
		size_t last_slash = file_path.find_last_of("/\\");
		size_t last_dot = file_path.find_last_of(".");
		return file_path.substr(last_slash + 1, last_dot - last_slash - 1);
	}
	std::string Utility::makeRelativePath(const std::string& base_path, const std::string& target_path)
	{
		return std::filesystem::relative(target_path, std::filesystem::path(base_path).parent_path()).string();
	}
}