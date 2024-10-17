/*****************************************************************//**
 * \file   db.h
 * \brief  json implementation
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#include "../headers/Core/stdafx.h"


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
