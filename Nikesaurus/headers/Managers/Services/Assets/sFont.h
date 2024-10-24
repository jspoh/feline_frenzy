/*****************************************************************//**
 * \file   sysFont.h
 * \brief	Font render manager
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   October 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef FONT_SYSTEM_HPP
#define FONT_SYSTEM_HPP

#include "Components/cRender.h"

namespace NIKESAURUS {
	namespace Font {

		//Abstract font lib interface
		class IFontLib {
		private:
		public:
			IFontLib() = default;
			virtual ~IFontLib() = default;
		};

		#ifdef NIKE_BUILD_DLL //!EXPOSE TO ENGINE ONLY

		//Font Service
		class NIKFontLib : public IFontLib {
		private:
			//Free type lib
			FT_Library ft_lib;

			//Generate texture from glyphs for rendering
			Render::Font generateGlyphsTex(FT_Face& font_face);

		public:
			//Default constructor
			NIKFontLib();

			//Load free type font
			Render::Font generateFont(std::string const& file_path, Vector2f const& pixel_sizes = { 0.0f, 48.0f });

			//Default destructor
			~NIKFontLib();
		};

		#endif //!EXPOSE TO ENGINE ONLY

		//Font Service
		class Service {
		private:
			std::shared_ptr<IFontLib> font_lib;
		public:
			Service();

			//Get font lib
			std::shared_ptr<IFontLib> getFontLib() const;
		};
	}
}

#endif //!FONT_SYSTEM_HPP