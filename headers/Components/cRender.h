#pragma once

#ifndef C_RENDER_HPP
#define C_RENDER_HPP

#include "../headers/Core/stdafx.h"

namespace Render {

	struct Color {
		Vector3 color;
		float alpha;
	};

	struct Model {
		unsigned int vaoid;
		unsigned int vboid;
		unsigned int eboid;
		unsigned int draw_count;
	};
}

#endif // !C_RENDER_HPP
