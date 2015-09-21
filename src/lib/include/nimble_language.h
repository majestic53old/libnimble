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

	#define CHAR_CARAGE_RETURN '\r'
	#define CHAR_COMMENT '#'
	#define CHAR_DIRECTORY_MARKER '.'
	#define CHAR_DIRECTORY_SEPERATOR_BACKWARD '\\'
	#define CHAR_DIRECTORY_SEPERATOR_FOREWORD '/'
	#define CHAR_END_OF_FILE '\0'
	#define CHAR_FILL '~'
	#define CHAR_LINE_FEED '\n'
	#define CHAR_LITERAL_STRING_DELIMITER '\"'
	#define CHAR_SPACE ' '
	#define CHAR_TAB '\t'

	#define TOK_INVALID INVALID_TYPE(nimble_tok_t)
	#define TOKSUB_INVALID INVALID_TYPE(nimble_subtok_t)

	typedef uint32_t nimble_tok_t, nimble_subtok_t;

	typedef enum {
		BASE_BINARY = 2,
		BASE_DECIMAL = 10,
		BASE_HEXIDECIMAL = 16,
		BASE_OCTAL = 8,
	} base_t;

	enum {
		SYMBOL_ASSIGNMENT = 0,
		SYMBOL_CLOSE_PARENTHESIS,
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
		SYMBOL_UNARY_NEGATION,
	};

	#define SYMBOL_MAX SYMBOL_SEPERATOR

	enum {
		TOKEN_ARGUMENT = 0,
		TOKEN_ASSIGNMENT,
		TOKEN_BEGIN,
		TOKEN_CALL,
		TOKEN_CALL_LIST,
		TOKEN_COMMAND,
		TOKEN_END,
		TOKEN_IMMEDIATE,
		TOKEN_LITERAL,
		TOKEN_STATEMENT,
		TOKEN_SYMBOL,
	};

	#define TOKEN_MAX TOKEN_SYMBOL

	typedef class _nimble_token_meta {

		public:

			_nimble_token_meta(
				__in_opt const std::string &line = std::string(),
				__in_opt const std::string &path = std::string(),
				__in_opt size_t column = 0,
				__in_opt size_t column_off = 0,
				__in_opt size_t row = 0,
				__in_opt size_t row_off = 0
				);

			_nimble_token_meta(
				__in const _nimble_token_meta &other
				);

			virtual ~_nimble_token_meta(void);

			_nimble_token_meta &operator=(
				__in const _nimble_token_meta &other
				);

			static std::string as_string(
				__in const _nimble_token_meta &meta,
				__in_opt size_t tabs = 0,
				__in_opt bool verbose = false
				);

			size_t &column(void);

			size_t &column_offset(void);

			std::string &line(void);

			std::string &path(void);

			size_t &row(void);

			size_t &row_offset(void);

			virtual std::string to_string(
				__in_opt size_t tabs = 0,
				__in_opt bool verbose = false
				);

		protected:

			size_t m_column;

			size_t m_column_offset;

			std::string m_line;

			std::string m_path;

			size_t m_row;

			size_t m_row_offset;

		private:

			std::recursive_mutex m_lock;

	} nimble_token_meta, *nimble_token_meta_ptr;

	typedef class _nimble_language {

		public:

			static double as_value(
				__in const std::string &text,
				__in base_t base,
				__in_opt const nimble_token_meta &meta = nimble_token_meta()
				);

			static bool is_control_token(
				__in nimble_tok_t type
				);

			static bool is_symbol(
				__in const std::string &text
				);

			static nimble_subtok_t subtype(
				__in const std::string &text,
				__in nimble_tok_t type
				);

			static std::string subtype_as_string(
				__in nimble_tok_t type,
				__in nimble_subtok_t subtype
				);

			static std::string type_as_string(
				__in nimble_tok_t type
				);

		protected:

			static double _as_value(
				__in char ch,
				__in int position,
				__in base_t base,
				__in_opt const nimble_token_meta &meta = nimble_token_meta()
				);

	} nimble_language, *nimble_language_ptr;
}

#endif // NIMBLE_LANGUAGE_H_
