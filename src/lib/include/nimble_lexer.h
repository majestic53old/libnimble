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

#ifndef NIMBLE_LEXER_H_
#define NIMBLE_LEXER_H_

#include <vector>

namespace NIMBLE {

	namespace LANGUAGE {

		#define POS_INVALID (INVALID_TYPE(size_t))

		typedef enum {
			CHAR_CLASS_ALPHA = 0,
			CHAR_CLASS_DIGIT,
			CHAR_CLASS_END,
			CHAR_CLASS_SPACE,
			CHAR_CLASS_SYMBOL,
		} char_cls_t;

		#define CHAR_CLASS_MAX CHAR_CLASS_SYMBOL

		typedef class _nimble_lexer_base {

			public:

				_nimble_lexer_base(
					__in_opt const std::string &input = std::string(),
					__in_opt bool is_file = false
					);

				_nimble_lexer_base(
					__in const _nimble_lexer_base &other
					);

				virtual ~_nimble_lexer_base(void);

				_nimble_lexer_base &operator=(
					__in const _nimble_lexer_base &other
					);

				char character(void);

				static std::string character_as_string(
					__in char ch,
					__in_opt bool verbose = false
					);

				char_cls_t character_class(void);

				size_t character_column(void);

				std::string character_exception(
					__in_opt size_t tabs = 0,
					__in_opt bool verbose = false
					);

				static std::string character_exception(
					__in const std::string &line,
					__in_opt const std::string &path = std::string(),
					__in_opt size_t column = 0,
					__in_opt size_t column_offset = 0,
					__in_opt size_t row = 0,
					__in_opt size_t row_offset = 0,
					__in_opt size_t tabs = 0,
					__in_opt bool verbose = false
					);

				static std::string character_exception(
					__in nimble_token_meta &meta,
					__in_opt size_t tabs = 0,
					__in_opt bool verbose = false
					);

				std::string character_line(void);

				size_t character_position(void);

				size_t character_row(void);

				static std::string class_as_string(
					__in char_cls_t cls
					);

				virtual void clear(void);

				virtual size_t discover(void);

				bool has_next_character(void);

				bool has_path(void);

				bool has_previous_character(void);

				bool is_newline(
					__out_opt size_t *length = NULL
					);

				char move_next_character(
					__out_opt char_cls_t *cls = NULL
					);

				char move_previous_character(
					__out_opt char_cls_t *cls = NULL
					);

				std::string path(void);

				virtual void reset(void);

				virtual void set(
					__in_opt const std::string &input = std::string(),
					__in_opt bool is_file = false
					);

				virtual size_t size(void);

				std::string source(void);

				virtual std::string to_string(
					__in_opt bool verbose = false
					);

			protected:

				std::map<size_t, std::pair<size_t, std::string>>::iterator find_line(
					__in size_t row
					);

				bool is_newline(
					__in size_t position,
					__in bool forward,
					__out_opt size_t *length = NULL
					);

				size_t m_char_column;

				std::map<size_t, std::pair<size_t, std::string>> m_char_line;

				size_t m_char_position;

				size_t m_char_row;

				std::string m_path;

				std::string m_source;

			private:

				std::recursive_mutex m_lock;

		} nimble_lexer_base, *nimble_lexer_base_ptr;

		typedef class _nimble_lexer :
				protected nimble_lexer_base {

			public:

				_nimble_lexer(
					__in_opt const std::string &input = std::string(),
					__in_opt bool is_file = false
					);

				_nimble_lexer(
					__in const _nimble_lexer &other
					);

				virtual ~_nimble_lexer(void);

				_nimble_lexer &operator=(
					__in const _nimble_lexer &other
					);

				virtual void clear(void);

				virtual size_t discover(void);

				bool has_next_token(void);

				bool has_previous_token(void);

				nimble_token &move_next_token(void);

				nimble_token &move_previous_token(void);

				virtual void reset(void);

				virtual void set(
					__in_opt const std::string &input = std::string(),
					__in_opt bool is_file = false
					);

				void set(
					__in const _nimble_lexer &other
					);

				virtual size_t size(void);

				virtual std::string to_string(
					__in_opt bool verbose = false
					);

				nimble_token &token(void);

				static std::string token_as_string(
					__in const nimble_uid &uid,
					__in_opt bool verbose = false
					);

				nimble_token &token_begin(void);

				nimble_token &token_end(void);

				std::string token_exception(
					__in_opt size_t tabs = 0,
					__in_opt bool verbose = false
					);

			protected:

				void enumerate_token(
					__inout nimble_token &tok
					);

				void enumerate_token_literal(
					__inout nimble_token &tok
					);

				void enumerate_token_symbol(
					__inout nimble_token &tok
					);

				nimble_token &insert_token(
					__in nimble_tok_t type,
					__in_opt nimble_subtok_t subtype = TOKSUB_INVALID,
					__in size_t position = POS_INVALID
					);

				void skip_whitespace(void);

				static nimble_token_factory_ptr acquire_token(void);

				std::vector<nimble_uid> m_tok_list;

				size_t m_tok_position;

			private:

				std::recursive_mutex m_lock;

		} nimble_lexer, *nimble_lexer_ptr;
	}
}

#endif // NIMBLE_LEXER_H_
