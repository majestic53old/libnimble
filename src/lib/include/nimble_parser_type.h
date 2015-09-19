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

#ifndef NIMBLE_PARSER_TYPE_H_
#define NIMBLE_PARSER_TYPE_H_

namespace NIMBLE {

	namespace LANGUAGE {

		#define NIMBLE_PARSER_HEADER "Parser"

		#ifndef NDEBUG
		#define NIMBLE_PARSER_EXCEPTION_HEADER NIMBLE_PARSER_HEADER
		#else
		#define NIMBLE_PARSER_EXCEPTION_HEADER EXCEPTION_HEADER
		#endif // NDEBUG

		enum {
			NIMBLE_PARSER_EXCEPTION_INVALID_STATEMENT_POSITION = 0,
			NIMBLE_PARSER_EXCEPTION_NO_NEXT_STATEMENT,
			NIMBLE_PARSER_EXCEPTION_NO_PREVIOUS_STATEMENT,
		};

		#define NIMBLE_PARSER_EXCEPTION_MAX NIMBLE_PARSER_EXCEPTION_NO_PREVIOUS_STATEMENT

		static const std::string NIMBLE_PARSER_EXCEPTION_STR[] = {
			"Invalid statement position",
			"No next statement is stream",
			"No previous statement in stream",
			};

		#define NIMBLE_PARSER_EXCEPTION_STRING(_TYPE_) \
			((_TYPE_) > NIMBLE_PARSER_EXCEPTION_MAX ? UNKNOWN_EXCEPTION : \
			CHK_STR(NIMBLE_PARSER_EXCEPTION_STR[_TYPE_]))

		#define THROW_NIMBLE_PARSER_EXCEPTION(_EXCEPT_) \
			THROW_EXCEPTION(NIMBLE_PARSER_EXCEPTION_HEADER, \
			NIMBLE_PARSER_EXCEPTION_STRING(_EXCEPT_))
		#define THROW_NIMBLE_PARSER_EXCEPTION_MESSAGE(_EXCEPT_, _FORMAT_, ...) \
			THROW_EXCEPTION_MESSAGE(NIMBLE_PARSER_EXCEPTION_HEADER, \
			NIMBLE_PARSER_EXCEPTION_STRING(_EXCEPT_), _FORMAT_, __VA_ARGS__)

		class _nimble_parser;
		typedef _nimble_parser nimble_parser, *nimble_parser_ptr;
	}
}

#endif // NIMBLE_PARSER_TYPE_H_
