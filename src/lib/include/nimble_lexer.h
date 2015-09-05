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

#include <map>

namespace NIMBLE {

	namespace LANGUAGE {

		#define CHAR_END '\0'
		#define CHAR_NEWLINE '\n'

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

				char_cls_t character_class(void);

				size_t character_column(void);

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

				std::map<size_t, std::pair<std::string, size_t>>::iterator find_line(
					__in size_t row
					);

				size_t m_char_column;

				std::map<size_t, std::pair<std::string, size_t>> m_char_line;

				size_t m_char_position;

				size_t m_char_row;

				std::string m_path;

				std::string m_source;

			private:

				std::recursive_mutex m_lock;

		} nimble_lexer_base, *nimble_lexer_base_ptr;
	}
}

#endif // NIMBLE_LEXER_H_
