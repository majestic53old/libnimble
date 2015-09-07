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

#include <cctype>
#include <fstream>
#include "../include/nimble.h"
#include "../include/nimble_lexer_type.h"

namespace NIMBLE {

	namespace LANGUAGE {

		#define CHAR_NEWLINE_LEN 1
		#define CHAR_NEWLINE_LONG_LEN 2
		#define CHAR_UNPRINTABLE CHAR_SPACE
		#define SENTINEL_LEXER_BASE 1

		static const std::string CHAR_CLASS_STR[] = {
			"ALPHA", "DIGIT", "END", "SPACE", "SYMBOL",
			};

		#define CHAR_CLASS_STRING(_TYPE_) \
			((_TYPE_) > CHAR_CLASS_MAX ? UNKNOWN : \
			CHK_STR(CHAR_CLASS_STR[_TYPE_]))

		_nimble_lexer_base::_nimble_lexer_base(
			__in_opt const std::string &input,
			__in_opt bool is_file
			) :
				m_char_column(0),
				m_char_position(0),
				m_char_row(0)
		{
			nimble_lexer_base::set(input, is_file);
		}

		_nimble_lexer_base::_nimble_lexer_base(
			__in const _nimble_lexer_base &other
			) :
				m_char_column(other.m_char_column),
				m_char_line(other.m_char_line),
				m_char_position(other.m_char_position),
				m_char_row(other.m_char_row),
				m_path(other.m_path),
				m_source(other.m_source)
		{
			return;
		}

		_nimble_lexer_base::~_nimble_lexer_base(void)
		{
			return;
		}

		_nimble_lexer_base &
		_nimble_lexer_base::operator=(
			__in const _nimble_lexer_base &other
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			if(this != &other) {
				m_char_column = other.m_char_column;
				m_char_line = other.m_char_line;
				m_char_position = other.m_char_position;
				m_char_row = other.m_char_row;
				m_path = other.m_path;
				m_source = other.m_source;
			}

			return *this;
		}

		std::string 
		_nimble_lexer_base::as_string(
			__in char ch,
			__in_opt bool verbose
			)
		{
			std::stringstream result;

			if(std::isspace(ch)) {

				switch(ch) {
					case CHAR_CARAGE_RETURN:
						result << "\\r";
						break;
					case CHAR_LINE_FEED:
						result << "\\n";
						break;
					case CHAR_SPACE:
						result << CHAR_SPACE;
						break;
					case CHAR_TAB:
						result << "\\t";
						break;
					default:
						result << "\\x" << VAL_AS_HEX(uint8_t, ch);
						break;
				}
			} else if(ch == CHAR_END_OF_FILE) {
				result << "\\0";
			} else {
				result << (std::isprint(ch) ? ch : CHAR_UNPRINTABLE);
			}

			return CHK_STR(result.str());
		}

		char 
		_nimble_lexer_base::character(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			if(m_char_position >= m_source.size()) {
				THROW_NIMBLE_LEXER_EXCEPTION_MESSAGE(NIMBLE_LEXER_EXCEPTION_INVALID_CHARACTER_POSITION,
					"%lu", m_char_position);
			}

			return m_source.at(m_char_position);
		}

		char_cls_t 
		_nimble_lexer_base::character_class(void)
		{
			char ch;
			char_cls_t result = CHAR_CLASS_SYMBOL;

			SERIALIZE_CALL_RECUR(m_lock);

			ch = character();
			if(ch == CHAR_END_OF_FILE) {
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
		_nimble_lexer_base::character_exception(
			__in_opt size_t tabs,
			__in_opt bool verbose
			)
		{						
			std::string line;
			size_t iter, off = 0;
			std::stringstream result;
			std::string::iterator ch_iter;

			SERIALIZE_CALL_RECUR(m_lock);

			for(iter = 0; iter < tabs; ++iter) {
				result << CHAR_TAB;
			}

			line = CHK_STR(character_line());
			for(ch_iter = line.begin(), iter = 0; ch_iter != line.end(); ++ch_iter, ++iter) {

				if(iter < m_char_column) {

					switch(*ch_iter) {
						case  CHAR_CARAGE_RETURN:
						case CHAR_LINE_FEED:
						case CHAR_TAB:
							++off;
							break;
					}
				}

				result << nimble_lexer_base::as_string(*ch_iter, verbose);
			}

			if(result.str().back() != CHAR_LINE_FEED) {
				result << std::endl;
			}

			for(iter = 0; iter < tabs; ++iter) {
				result << CHAR_TAB;
			}

			for(iter = 0; iter < (m_char_column + off); ++iter) {
				result << CHAR_SPACE;
			}

			result << "^";

			if(verbose) {
				result << std::endl;

				for(iter = 0; iter <= tabs; ++iter) {
					result << CHAR_TAB;
				}

				result << "(";

				if(has_path()) {
					result << CHK_STR(m_path) << ":";
				}

				result << m_char_row << ":" << m_char_column << ")";
			}

			return CHK_STR(result.str());
		}

		std::string 
		_nimble_lexer_base::character_line(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return find_line(m_char_row)->second.second;
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
			return CHAR_CLASS_STRING(cls);
		}

		void 
		_nimble_lexer_base::clear(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			m_char_column = 0;
			m_char_line.clear();
			m_char_position = 0;
			m_char_row = 0;
			m_path.clear();
			m_source.clear();
		}

		size_t 
		_nimble_lexer_base::discover(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			nimble_lexer_base::reset();

			return nimble_lexer_base::size();
		}

		std::map<size_t, std::pair<size_t, std::string>>::iterator 
		_nimble_lexer_base::find_line(
			__in size_t row
			)
		{
			std::map<size_t, std::pair<size_t, std::string>>::iterator result;

			SERIALIZE_CALL_RECUR(m_lock);

			result = m_char_line.find(row);
			if(result == m_char_line.end()) {
				THROW_NIMBLE_LEXER_EXCEPTION_MESSAGE(NIMBLE_LEXER_EXCEPTION_ROW_NOT_FOUND,
					"%lu", row);
			}

			return result;
		}

		bool 
		_nimble_lexer_base::has_next_character(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return ((m_char_position < m_source.size()) 
				&& (character_class() != CHAR_CLASS_END));
		}

		bool 
		_nimble_lexer_base::has_path(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return !m_path.empty();
		}

		bool 
		_nimble_lexer_base::has_previous_character(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return (m_char_position > 0);
		}

		bool 
		_nimble_lexer_base::is_newline(
			__in size_t position,
			__in bool forward,
			__out_opt size_t *length
			)
		{			
			size_t len = 0;
			char ch, cmp0, cmp1;
			bool result = false, valid;

			SERIALIZE_CALL_RECUR(m_lock);

			valid = (forward ? (position < m_source.size()) : true);
			if(valid) {
				ch = m_source.at(position);
				cmp0 = (forward ? CHAR_CARAGE_RETURN : CHAR_LINE_FEED);
				cmp1 = (forward ? CHAR_LINE_FEED : CHAR_CARAGE_RETURN);

				if(ch == cmp0) {
					position = (forward ? (position + 1) : (position - 1));
					valid = (forward ? (position < m_source.size()) 
						: (position > 0));

					if(valid) {

						ch = m_source.at(position);
						if(ch == cmp1) {
							len = CHAR_NEWLINE_LONG_LEN;
							result = true;
						} else if(!forward) {
							len = CHAR_NEWLINE_LEN;
							result = true;
						}
					}
				} else if(ch == CHAR_LINE_FEED) {
					len = CHAR_NEWLINE_LEN;
					result = true;
				}
			}

			if(length) {
				*length = len;
			}

			return result;
		}

		char 
		_nimble_lexer_base::move_next_character(
			__out_opt char_cls_t *cls
			)
		{
			char ch;
			size_t col, len;
			std::string line;

			SERIALIZE_CALL_RECUR(m_lock);

			if(!has_next_character()) {
				THROW_NIMBLE_LEXER_EXCEPTION_MESSAGE(NIMBLE_LEXER_EXCEPTION_NO_NEXT_CHARACTER,
					"%lu", m_char_position);
			}

			ch = character();
			if(is_newline(m_char_position, true, &len)) {

				if(len == CHAR_NEWLINE_LEN) {
					m_char_column = 0;
					++m_char_row;
					
					col = (m_char_position + 1);
					for(;; ++col) {
						ch = m_source.at(col);
						line += ch;
				
						if((ch == CHAR_END_OF_FILE)
								|| (is_newline(col, true, &len)
								&& (len == CHAR_NEWLINE_LEN))) {
							break;
						}
					}

					m_char_line.insert(std::pair<size_t, std::pair<size_t, std::string>>(
						m_char_row, std::pair<size_t, std::string>(col - m_char_position - 1, 
						line)));
				} else {
					++m_char_column;
				}
			} else {
				++m_char_column;
			}

			++m_char_position;

			if(cls) {
				*cls = character_class();
			}

			return character();
		}

		char 
		_nimble_lexer_base::move_previous_character(
			__out_opt char_cls_t *cls
			)
		{
			char ch;
			size_t len;

			SERIALIZE_CALL_RECUR(m_lock);

			if(!has_previous_character()) {
				THROW_NIMBLE_LEXER_EXCEPTION_MESSAGE(NIMBLE_LEXER_EXCEPTION_NO_PREVIOUS_CHARACTER,
					"%lu", m_char_position);
			}

			--m_char_position;

			ch = character();
			if(is_newline(m_char_position, false, &len)) {
				--m_char_row;
				m_char_column = find_line(m_char_row)->second.first;
			} else {
				--m_char_column;
			}

			if(cls) {
				*cls = character_class();
			}

			return ch;
		}

		std::string 
		_nimble_lexer_base::path(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return m_path;
		}

		void 
		_nimble_lexer_base::reset(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			m_char_column = 0;
			m_char_position = 0;
			m_char_row = 0;
		}

		void 
		_nimble_lexer_base::set(
			__in_opt const std::string &input,
			__in_opt bool is_file
			)
		{
			char ch;
			size_t line_len;
			int len, col = 0;
			std::string line;

			SERIALIZE_CALL_RECUR(m_lock);

			nimble_lexer_base::clear();

			if(is_file) {

				std::ifstream file(input.c_str(), std::ios::in);
				if(!file) {
					THROW_NIMBLE_LEXER_EXCEPTION_MESSAGE(NIMBLE_LEXER_EXCEPTION_FILE_NOT_FOUND,
						"%s", CHK_STR(input));
				}

				file.seekg(0, std::ios::end);
				len = file.tellg();
				file.seekg(0, std::ios::beg);
				m_source.resize(len, CHAR_END_OF_FILE);
				file.read((char *) &m_source[0], len);
				file.close();
				m_path = input;
			} else {
				m_source = input;
			}

			m_source += CHAR_END_OF_FILE;

			for(;; ++col) {
				ch = m_source.at(col);
				line += ch;
				
				if((ch == CHAR_END_OF_FILE)
						|| (is_newline(col, true, &line_len)
						&& (line_len == CHAR_NEWLINE_LEN))) {
					break;
				}
			}

			m_char_line.insert(std::pair<size_t, std::pair<size_t, std::string>>(m_char_row, 
				std::pair<size_t, std::string>(col, line)));
		}

		size_t 
		_nimble_lexer_base::size(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return (m_source.size() - SENTINEL_LEXER_BASE);
		}

		std::string 
		_nimble_lexer_base::source(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return m_source;
		}

		std::string 
		_nimble_lexer_base::to_string(
			__in_opt bool verbose
			)
		{
			char ch;
			std::string line;
			std::stringstream result;
			std::string::iterator iter;

			SERIALIZE_CALL_RECUR(m_lock);

			if(verbose) {
				result << "(" << m_char_position << "/" << size() << ") ";
			}

			ch = character();
			result << "[" << CHAR_CLASS_STRING(character_class()) << "] \'"
				<< nimble_lexer_base::as_string(ch, verbose) 
				<< "\' (" << VAL_AS_HEX(uint8_t, ch) << ") \'";

			line = CHK_STR(character_line());
			for(iter = line.begin(); iter != line.end(); ++iter) {
				result << nimble_lexer_base::as_string(*iter, verbose);
			}

			result << "\' [" << m_char_row << ", " << m_char_column << "]";

			return CHK_STR(result.str());
		}
	}
}
