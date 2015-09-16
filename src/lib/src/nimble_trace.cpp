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
#include "../include/nimble.h"

namespace NIMBLE {

	enum {
		NIMBLE_TRACE_EXCEPTION_EMPTY = 0,
		NIMBLE_TRACE_EXCEPTION_MALFORMED,
	};

	#define NIMBLE_TRACE_EXCEPTION_MAX NIMBLE_TRACE_EXCEPTION_MALFORMED

	static const std::string NIMBLE_TRACE_EXCEPTION_STR[] = {
		"Empty exception",
		"Malformed exception",
		};

	#define NIMBLE_TRACE_EXCEPTION_STRING(_TYPE_) \
		((_TYPE_) > NIMBLE_TRACE_EXCEPTION_MAX ? UNKNOWN : \
		CHK_STR(NIMBLE_TRACE_EXCEPTION_STR[_TYPE_]))

	static const std::string TRACE_LEVEL_STR[] = {
		std::string(), "Error", "Warning", "Information", "Verbose",
		};

	#define NIMBLE_TRACE_LEVEL_STRING(_TYPE_) \
		((_TYPE_) > NIMBLE_LEVEL_MAX ? UNKNOWN : \
		CHK_STR(TRACE_LEVEL_STR[_TYPE_]))

	void 
	_nimble_trace::generate(
		__in nimble_lvl_t level,
		__in const std::string &message,
		__in const std::string &source,
		__in size_t line,
		__in const char *format,
		...
		)
	{
		std::stringstream result;

		if((TLEVEL > TRACE_NONE)
				&& (TLEVEL >= level)) {

			// TODO

			switch(level) {
				case TRACE_ERROR:
					SET_TERM_ATTRIB(std::cerr, 1, COL_FORE_RED);
					std::cerr << CHK_STR(result.str()) << std::endl;
					CLEAR_TERM_ATTRIB(std::cerr);
					break;
				case TRACE_WARNING:
					SET_TERM_ATTRIB(std::cerr, 1, COL_FORE_YELLOW);
					std::cerr << CHK_STR(result.str()) << std::endl;
					CLEAR_TERM_ATTRIB(std::cerr);
					break;
				default:
					std::cout << CHK_STR(result.str()) << std::endl;
					break;
			}

			
		}
	}
}
