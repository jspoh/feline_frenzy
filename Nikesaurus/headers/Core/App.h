#pragma once

#include "Core.h"

namespace NIKE {

	class NIKESAURUS_API Application
	{
	public:
		Application() = default;
		virtual ~Application() = default;
		void run();
	};
}

