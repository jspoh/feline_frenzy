/*****************************************************************//**
 * \file   System.h
 * \brief
 *
 * \author jings
 * \date   11 September 2024
 *********************************************************************/

#pragma once

#ifndef SYSTEM_HPP
#define SYSTEM_HPP

namespace System {

	class Base {
	private:
		bool b_system_active;
	public:

		//Constructor
		Base() : b_system_active{ true } {}

		//Init System
		virtual void init() {}

		//Pure virtual function to be implemented in inherited class
		virtual void update() = 0;

		//Set System active
		void setSystemActive(bool state) {
			b_system_active = state;
		}

		//Get System active
		bool getSystemActive() const {
			return b_system_active;
		}

		//Destructor
		virtual ~Base() = default;
	};
}

#endif //!SYSTEM_HPP