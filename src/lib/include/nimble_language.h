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

#ifndef NIMBLE_LANGUAGE_H_
#define NIMBLE_LANGUAGE_H_

namespace NIMBLE {

	#define TOK_INVALID INVALID_TYPE(tok_t)

	#define TOKSUB_INVALID INVALID_TYPE(toksub_t)

	typedef uint32_t tok_t, toksub_t;

	typedef enum {
		BASE_BINARY = 2,
		BASE_DECIMAL = 10,
		BASE_HEXIDECIMAL = 16,
		BASE_OCTAL = 8,
	} base_t;

	enum {
		SYMBOL_CLOSE_PARENTHESIS = 0,
		SYMBOL_MODE,
		SYMBOL_MODIFIER,
		SYMBOL_OPEN_PARENTHESIS,
		SYMBOL_PIPE,
		SYMBOL_REDIRECT_IN,
		SYMBOL_REDIRECT_OUT,
		SYMBOL_REDIRECT_OUT_APPEND,
		SYMBOL_REDIRECT_OUT_ERR,
		SYMBOL_REDIRECT_OUT_ERR_APPEND,
		SYMBOL_REDIRECT_OUT_ERR_OVERWRITE,
		SYMBOL_REDIRECT_OUT_OVERWRITE,
		SYMBOL_SEPERATOR,
	};

	#define SYMBOL_MAX SYMBOL_SEPERATOR

	enum {
		TOKEN_BEGIN = 0,
		TOKEN_END,
		TOKEN_IDENTIFIER,
		TOKEN_IMMEDIATE,
		TOKEN_LITERAL,
		TOKEN_SYMBOL,
	};

	#define TOKEN_MAX TOKEN_SYMBOL

	typedef class _nimble_language {

		public:

			static double as_value(
				__in const std::string &text,
				__in base_t base
				);

			static toksub_t subtype(
				__in const std::string &text,
				__in tok_t type
				);

			static std::string subtype_as_string(
				__in tok_t type,
				__in toksub_t subtype
				);

			static std::string type_as_string(
				__in tok_t type
				);

		protected:

			static double _as_value(
				__in char ch,
				__in int position,
				__in base_t base
				);

	} nimble_language, *nimble_language_ptr;
}

#endif // NIMBLE_LANGUAGE_H_
