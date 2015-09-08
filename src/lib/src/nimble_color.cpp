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

#include <cstdarg>
#include <cstdlib>
#include <term.h>
#include "../include/nimble.h"

namespace NIMBLE {

	#define CHAR_ATTRIB_SEP ';'
	#define TERM_MAX_COL ((char *) "Co")

	void 
	_nimble_color::clear(
		__in std::ostream &stream
		)
	{
		SET_TERM_ATTRIB(stream, 1, COL_FORM_RESET);
	}

	bool 
	_nimble_color::is_supported(void)
	{
		int max;
		bool result = true;

		max = tgetnum(TERM_MAX_COL);
		if(max != INVALID(int)) {
			result = (max >= 8);
		}

		return result;
	}

	void 
	_nimble_color::set(
		__in std::ostream &stream,
		__in size_t count,
		...
		)
	{
		va_list args;
		size_t iter = 0;

		if(count) {
			stream << "\e[";
			va_start(args, count);

			for(; iter < count; ++iter) {
				stream << va_arg(args, int);

				if(iter < (count - 1)) {
					stream << CHAR_ATTRIB_SEP;
				}
			}

			va_end(args);
			stream << "m";
		}
	}
}
