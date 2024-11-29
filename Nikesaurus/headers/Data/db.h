/*****************************************************************//**
 * \file   db.h
 * \brief  json implementation
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#include "Core/stdafx.h"

 // Define a variant for complex map values
using ComplexValue = std::variant<int, float, std::string, bool>;
using ComplexMap = std::unordered_map<std::string, ComplexValue>;

// Specialization for nlohmann::json to handle ComplexValue
NLOHMANN_JSON_NAMESPACE_BEGIN
template <>
struct adl_serializer<ComplexValue> {
	static void to_json(json& j, const ComplexValue& value) {
		std::visit([&j](auto&& arg) {
			j = arg; // Serialize based on the type of the value
			}, value);
	}

	static void from_json(const json& j, ComplexValue& value) {
		if (j.is_number_integer()) {
			value = j.get<int>();
		}
		else if (j.is_number_float()) {
			value = j.get<float>();
		}
		else if (j.is_string()) {
			value = j.get<std::string>();
		}
		else if (j.is_boolean()) {
			value = j.get<bool>();
		}
		else {
			throw std::runtime_error("Unsupported type in JSON");
		}
	}
};
NLOHMANN_JSON_NAMESPACE_END

const std::string dbPath = R"(data.json)";

class Database {
private:
	Database();

	std::ifstream ifs;


public:
	~Database();

	static Database& getInstance();

	/**
	 * Warning: All data stored in this file will be updated on instance deletion.
	 * 
	 * usage:
	 * Database::getInstance().data[<column name>]...
	 * 
	 * Force update data at anytime with the `forceUpdate` function.
	 */
	nlohmann::json data;

	/**
	 * Update the json object data before calling this function.
	 * 
	 * updates the data.json file with current data in `data`
	 */
	bool forceUpdate();

};
