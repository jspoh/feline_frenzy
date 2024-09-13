/* Start Header ************************************************************************/
/*!
\file db.cpp
\author Poh Jing Seng, jingseng.poh, 2301363
\par jingseng.poh\@digipen.edu
\date 28 feb 2024
\brief json implementation
/*
Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/


#pragma once

#include "stdafx.h"


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
