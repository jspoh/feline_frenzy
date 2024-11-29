/*****************************************************************//**
 * \file   db.cpp
 * \brief  json implementation
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Data/db.h"

using json = nlohmann::json;


namespace {

}


Database::Database() {
	ifs = std::ifstream{ dbPath };

	if (!ifs.is_open()) {
		cerr << "failed to open database file" << "\n";
	}

	this->data = json::parse(ifs);

	ifs.close();
}


Database::~Database() {
	forceUpdate();
}


Database& Database::getInstance() {
	static Database instance;
	return instance;
}


bool Database::forceUpdate() {
	// !TODO: this function should not be called until impl has been shifted to a public dir.
	return false;

	try {
		std::ofstream ofs{ dbPath };
		ofs << data.dump(2);
		ofs.close();
	}
	catch (std::exception& e) {
		cerr << "failed to update file: " << e.what() << "\n";
		return false;
	}
	return true;
}
