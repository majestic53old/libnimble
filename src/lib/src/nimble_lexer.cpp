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

#include "../include/nimble.h"
#include "../include/nimble_lexer_type.h"

namespace NIMBLE {

	namespace LANGUAGE {

		static const std::string CHAR_CLASS_STR[] = {
			"ALPHA", "DIGIT", "END", "SPACE", "SYMBOL",
			};

		#define CHAR_CLASS_STRING(_TYPE_) \
			((_TYPE_) > CHAR_CLASS_MAX ? UNKNOWN : \
			CHK_STR(CHAR_CLASS_STR[_TYPE_]))

		_nimble_lexer_base::_nimble_lexer_base(
			__in_opt const std::string &input,
			__in_opt bool is_file
			)
		{
			// TODO
		}

		_nimble_lexer_base::_nimble_lexer_base(
			__in const _nimble_lexer_base &other
			)
		{
			// TODO
		}

		_nimble_lexer_base::~_nimble_lexer_base(void)
		{
			// TODO
		}

		_nimble_lexer_base &
		_nimble_lexer_base::operator=(
			__in const _nimble_lexer_base &other
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			// TODO
		}

		char 
		_nimble_lexer_base::character(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			// TODO
		}

		char_cls_t 
		_nimble_lexer_base::character_class(void)
		{
			char ch;
			char_cls_t result = CHAR_CLASS_SYMBOL;

			SERIALIZE_CALL_RECUR(m_lock);

			ch = character();
			if(ch == CHAR_END) {
				result = CHAR_CLASS_END;
			} else if(std::isalpha(ch)) {
				result = CHAR_CLASS_ALPHA;
			} else if(std::isdigit(ch)) {
				result = CHAR_CLASS_DIGIT;
			} else if(std::isspace(ch)) {
				result = CHAR_CLASS_SPACE;
			}

			return result;
		}

		size_t 
		_nimble_lexer_base::character_column(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return m_char_column;
		}

		std::string 
		_nimble_lexer_base::character_line(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			// TODO
		}

		size_t 
		_nimble_lexer_base::character_position(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return m_char_position;
		}

		size_t 
		_nimble_lexer_base::character_row(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return m_char_row;
		}

		std::string 
		_nimble_lexer_base::class_as_string(
			__in char_cls_t cls
			)
		{
			// TODO
		}

		void 
		_nimble_lexer_base::clear(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			// TODO
		}

		size_t 
		_nimble_lexer_base::discover(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			// TODO
		}

		std::map<size_t, std::pair<std::string, size_t>>::iterator 
		_nimble_lexer_base::find_line(
			__in size_t row
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			// TODO
		}

		bool 
		_nimble_lexer_base::has_next_character(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			// TODO
		}

		bool 
		_nimble_lexer_base::has_path(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			// TODO
		}

		bool 
		_nimble_lexer_base::has_previous_character(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			// TODO
		}

		char 
		_nimble_lexer_base::move_next_character(
			__out_opt char_cls_t *cls
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			// TODO
		}

		char 
		_nimble_lexer_base::move_previous_character(
			__out_opt char_cls_t *cls
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			// TODO
		}

		std::string 
		_nimble_lexer_base::path(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			// TODO
		}

		void 
		_nimble_lexer_base::reset(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			// TODO
		}

		void 
		_nimble_lexer_base::set(
			__in_opt const std::string &input,
			__in_opt bool is_file
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			// TODO
		}

		size_t 
		_nimble_lexer_base::size(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			// TODO
		}

		std::string 
		_nimble_lexer_base::source(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			// TODO
		}

		std::string 
		_nimble_lexer_base::to_string(
			__in_opt bool verbose
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			// TODO
		}
	}
}
