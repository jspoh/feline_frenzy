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

#include "stdafx.h"
#include "db.h"

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
