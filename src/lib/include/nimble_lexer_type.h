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

#ifndef NIMBLE_LEXER_TYPE_H_
#define NIMBLE_LEXER_TYPE_H_

namespace NIMBLE {

	namespace LANGUAGE {

		#define NIMBLE_LEXER_HEADER "Lexer"

		#ifndef NDEBUG
		#define NIMBLE_LEXER_EXCEPTION_HEADER NIMBLE_LEXER_HEADER
		#else
		#define NIMBLE_LEXER_EXCEPTION_HEADER EXCEPTION_HEADER
		#endif // NDEBUG

		enum {
			NIMBLE_LEXER_EXCEPTION_COMPONENT_NOT_READY = 0,
			NIMBLE_LEXER_EXCEPTION_FILE_NOT_FOUND,
			NIMBLE_LEXER_EXCEPTION_INVALID_CHARACTER_POSITION,
			NIMBLE_LEXER_EXCEPTION_INVALID_COMPONENT,
			NIMBLE_LEXER_EXCEPTION_INVALID_TOKEN_POSITION,
			NIMBLE_LEXER_EXCEPTION_NO_NEXT_CHARACTER,
			NIMBLE_LEXER_EXCEPTION_NO_NEXT_TOKEN,
			NIMBLE_LEXER_EXCEPTION_NO_PREVIOUS_CHARACTER,
			NIMBLE_LEXER_EXCEPTION_NO_PREVIOUS_TOKEN,
			NIMBLE_LEXER_EXCEPTION_ROW_NOT_FOUND,
		};

		#define NIMBLE_LEXER_EXCEPTION_MAX NIMBLE_LEXER_EXCEPTION_ROW_NOT_FOUND

		static const std::string NIMBLE_LEXER_EXCEPTION_STR[] = {
			"Token component is not ready",
			"File does not exist",
			"Invalid character position",
			"Invalid token component",
			"Invalid token position",
			"No next character in stream",
			"No next token in stream",
			"No previous character in stream",
			"No previous token in stream",
			"Row does not exist",
			};

		#define NIMBLE_LEXER_EXCEPTION_STRING(_TYPE_) \
			((_TYPE_) > NIMBLE_LEXER_EXCEPTION_MAX ? UNKNOWN_EXCEPTION : \
			CHK_STR(NIMBLE_LEXER_EXCEPTION_STR[_TYPE_]))

		#define THROW_NIMBLE_LEXER_EXCEPTION(_EXCEPT_) \
			THROW_EXCEPTION(NIMBLE_LEXER_EXCEPTION_HEADER, \
			NIMBLE_LEXER_EXCEPTION_STRING(_EXCEPT_))
		#define THROW_NIMBLE_LEXER_EXCEPTION_MESSAGE(_EXCEPT_, _FORMAT_, ...) \
			THROW_EXCEPTION_MESSAGE(NIMBLE_LEXER_EXCEPTION_HEADER, \
			NIMBLE_LEXER_EXCEPTION_STRING(_EXCEPT_), _FORMAT_, __VA_ARGS__)

		class _nimble_lexer_base;
		typedef _nimble_lexer_base nimble_lexer_base, *nimble_lexer_base_ptr;

		class _nimble_lexer;
		typedef _nimble_lexer nimble_lexer, *nimble_lexer_ptr;
	}
}

#endif // NIMBLE_LEXER_TYPE_H_
