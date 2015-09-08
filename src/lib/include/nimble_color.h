/**
 * libnimble
 * Copyright (C) 2015 David Jolly
 * ----------------------
 *
 * libnimble is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libnimble is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NIMBLE_COLOR_T_
#define NIMBLE_COLOR_T_

#include <ostream>

namespace NIMBLE {

	typedef enum {
		COL_BACK_BLACK = 40,
		COL_BACK_BLUE = 44,
		COL_BACK_CYAN = 46,
		COL_BACK_DARK_GREY = 100,
		COL_BACK_DEFAULT = 49,
		COL_BACK_GREEN = 42,
		COL_BACK_LIGHT_GREY = 47,
		COL_BACK_LIGHT_BLUE = 104,
		COL_BACK_LIGHT_CYAN = 106,
		COL_BACK_LIGHT_GREEN = 102,
		COL_BACK_LIGHT_MAGENTA = 105,
		COL_BACK_LIGHT_RED = 101,
		COL_BACK_LIGHT_YELLOW = 103,
		COL_BACK_MAGENTA = 45,
		COL_BACK_RED = 41,
		COL_BACK_YELLOW = 43,
		COL_BACK_WHITE = 107,
		COL_FORE_BLACK = 30,
		COL_FORE_BLUE = 34,
		COL_FORE_CYAN = 36,
		COL_FORE_DARK_GREY = 90,
		COL_FORE_DEFAULT = 39,
		COL_FORE_GREEN = 32,
		COL_FORE_LIGHT_BLUE = 94,
		COL_FORE_LIGHT_CYAN = 96,
		COL_FORE_LIGHT_GREEN = 92,
		COL_FORE_LIGHT_GREY = 37,
		COL_FORE_LIGHT_MAGENTA = 95,
		COL_FORE_LIGHT_RED = 91,
		COL_FORE_LIGHT_YELLOW = 93,
		COL_FORE_MAGENTA = 35,
		COL_FORE_RED = 31,
		COL_FORE_YELLOW = 33,
		COL_FORE_WHITE = 97,
		COL_FORM_BOLD = 1,
		COL_FORM_BOLD_RESET = 21,
		COL_FORM_BLINK = 5,
		COL_FORM_BLINK_RESET = 25,
		COL_FORM_DIM = 2,
		COL_FORM_DIM_RESET = 22,
		COL_FORM_HIDE = 8,
		COL_FORM_HIDE_RESET = 28,
		COL_FORM_INVERT = 7,
		COL_FORM_INVERT_RESET = 27,
		COL_FORM_RESET = 0,
		COL_FORM_UNDERLINED = 4,
		COL_FORM_UNDERLINED_RESET = 24,
	} nimble_col_t;

	#define CLEAR_TERM_ATTRIB(_STR_) {\
		if(nimble_color::is_supported()) { \
			nimble_color::clear(_STR_); \
		} \
		}

	#define SET_TERM_ATTRIB(_STR_, _COUNT_, ...) {\
		if(nimble_color::is_supported()) { \
			nimble_color::set(_STR_, _COUNT_, __VA_ARGS__); \
		} \
		}	

	typedef class _nimble_color {

		public:

			static void clear(
				__in std::ostream &stream
				);

			static bool is_supported(void);

			static void set(
				__in std::ostream &stream,
				__in size_t count,
				...
				);

	} nimble_color, *nimble_color_ptr;
}

#endif // NIMBLE_COLOR_T_
