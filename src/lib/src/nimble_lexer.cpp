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
		#define SENTINEL_LEXER 2
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
			TRACE_ENTRY(TRACE_VERBOSE);

			nimble_lexer_base::set(input, is_file);

			TRACE_EXIT(TRACE_VERBOSE);
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
			TRACE_ENTRY(TRACE_VERBOSE);
			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_lexer_base::~_nimble_lexer_base(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_lexer_base &
		_nimble_lexer_base::operator=(
			__in const _nimble_lexer_base &other
			)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(this != &other) {
				m_char_column = other.m_char_column;
				m_char_line = other.m_char_line;
				m_char_position = other.m_char_position;
				m_char_row = other.m_char_row;
				m_path = other.m_path;
				m_source = other.m_source;
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", this);
			return *this;
		}

		char 
		_nimble_lexer_base::character(void)
		{
			char result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(m_char_position >= m_source.size()) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, pos. %lu", 
					NIMBLE_LEXER_EXCEPTION_STRING(NIMBLE_LEXER_EXCEPTION_INVALID_CHARACTER_POSITION),
					m_char_position);
				THROW_NIMBLE_LEXER_EXCEPTION_MESSAGE(NIMBLE_LEXER_EXCEPTION_INVALID_CHARACTER_POSITION,
					"%lu", m_char_position);
			}

			result = m_source.at(m_char_position);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. \'%c\' (0x%x)", 
				std::isprint(result) ? result : ' ', result);
			return result;
		}

		std::string 
		_nimble_lexer_base::character_as_string(
			__in char ch,
			__in_opt bool verbose
			)
		{
			std::stringstream result;

			TRACE_ENTRY(TRACE_VERBOSE);

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

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result.str()));
			return CHK_STR(result.str());
		}

		char_cls_t 
		_nimble_lexer_base::character_class(void)
		{
			char ch;
			char_cls_t result = CHAR_CLASS_SYMBOL;

			TRACE_ENTRY(TRACE_VERBOSE);
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

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. \'%c\' (0x%x)", 
				std::isprint(result) ? result : ' ', result);
			return result;
		}

		size_t 
		_nimble_lexer_base::character_column(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);
			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", m_char_column);
			return m_char_column;
		}

		std::string 
		_nimble_lexer_base::character_exception(
			__in_opt size_t tabs,
			__in_opt bool verbose
			)
		{
			std::string result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = character_exception(CHK_STR(character_line()), m_path, 
					m_char_column, m_char_column, m_char_row, m_char_row, 
					tabs, verbose);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result));
			return CHK_STR(result);
		}

		std::string 
		_nimble_lexer_base::character_exception(
			__in const std::string &line,
			__in_opt const std::string &path,
			__in_opt size_t column,
			__in_opt size_t column_offset,
			__in_opt size_t row,
			__in_opt size_t row_offset,
			__in_opt size_t tabs,
			__in_opt bool verbose
			)
		{
			size_t iter, off = 0;
			std::stringstream result;
			std::string::const_iterator ch_iter;

			TRACE_ENTRY(TRACE_VERBOSE);

			for(iter = 0; iter < tabs; ++iter) {
				result << CHAR_TAB;
			}

			for(ch_iter = line.begin(), iter = 0; ch_iter != line.end(); ++ch_iter, ++iter) {

				if(iter < column) {

					switch(*ch_iter) {
						case  CHAR_CARAGE_RETURN:
						case CHAR_LINE_FEED:
						case CHAR_TAB:
							++off;
							break;
					}
				}

				result << nimble_lexer_base::character_as_string(*ch_iter, verbose);
			}

			if(result.str().back() != CHAR_LINE_FEED) {
				result << std::endl;
			}

			for(iter = 0; iter < tabs; ++iter) {
				result << CHAR_TAB;
			}

			SET_TERM_ATTRIB(result, 1, COL_FORE_GREEN);

			for(iter = 0; iter < (column + off); ++iter) {

				if(iter != (column + off - 1)) {
					result << CHAR_FILL;
				} else {
					result << CHAR_SPACE;
				}
			}

			result << "^";
			CLEAR_TERM_ATTRIB(result);

			if(verbose) {
				result << std::endl;

				for(iter = 0; iter <= tabs; ++iter) {
					result << CHAR_TAB;
				}

				SET_TERM_ATTRIB(result, 1, COL_FORE_YELLOW);
				result << "(";

				if(!path.empty()) {
					result << CHK_STR(path) << ":";
				}

				result << row_offset << ":" << column_offset << ")";
				CLEAR_TERM_ATTRIB(result);
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result.str()));
			return CHK_STR(result.str());
		}

		std::string 
		_nimble_lexer_base::character_exception(
			__in nimble_token_meta &meta,
			__in_opt size_t tabs,
			__in_opt bool verbose
			)
		{
			std::string result;

			TRACE_ENTRY(TRACE_VERBOSE);

			result = nimble_lexer_base::character_exception(meta.line(), meta.path(), 
					meta.column(), meta.column_offset(), meta.row(), 
					meta.row_offset(), tabs, verbose);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result));
			return CHK_STR(result);
		}

		std::string 
		_nimble_lexer_base::character_line(void)
		{
			std::string result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = find_line(m_char_row)->second.second;

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result));
			return CHK_STR(result);
		}

		size_t 
		_nimble_lexer_base::character_position(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);
			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", m_char_position);
			return m_char_position;
		}

		size_t 
		_nimble_lexer_base::character_row(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);
			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", m_char_row);
			return m_char_row;
		}

		std::string 
		_nimble_lexer_base::class_as_string(
			__in char_cls_t cls
			)
		{
			std::string result;

			TRACE_ENTRY(TRACE_VERBOSE);

			result = CHAR_CLASS_STRING(cls);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result));
			return CHK_STR(result);
		}

		void 
		_nimble_lexer_base::clear(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			m_char_column = 0;
			m_char_line.clear();
			m_char_position = 0;
			m_char_row = 0;
			m_path.clear();
			m_source.clear();
			TRACE_MESSAGE(TRACE_INFORMATION, "%s", "Lexer base cleared");

			TRACE_EXIT(TRACE_VERBOSE);
		}

		size_t 
		_nimble_lexer_base::discover(void)
		{
			size_t result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			nimble_lexer_base::reset();
			result = nimble_lexer_base::size();
			TRACE_MESSAGE(TRACE_INFORMATION, "Lexer base discovered %lu characters",
				result);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		std::map<size_t, std::pair<size_t, std::string>>::iterator 
		_nimble_lexer_base::find_line(
			__in size_t row
			)
		{
			std::map<size_t, std::pair<size_t, std::string>>::iterator result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = m_char_line.find(row);
			if(result == m_char_line.end()) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, row. %lu", 
					NIMBLE_LEXER_EXCEPTION_STRING(NIMBLE_LEXER_EXCEPTION_ROW_NOT_FOUND),
					row);
				THROW_NIMBLE_LEXER_EXCEPTION_MESSAGE(NIMBLE_LEXER_EXCEPTION_ROW_NOT_FOUND,
					"%lu", row);
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", result);
			return result;
		}

		bool 
		_nimble_lexer_base::has_next_character(void)
		{
			bool result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = ((m_char_position < m_source.size()) 
				&& (character_class() != CHAR_CLASS_END));

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", result);
			return result;
		}

		bool 
		_nimble_lexer_base::has_path(void)
		{
			bool result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = !m_path.empty();

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", result);
			return result;
		}

		bool 
		_nimble_lexer_base::has_previous_character(void)
		{
			bool result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = (m_char_position > 0);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", result);
			return result;
		}

		bool 
		_nimble_lexer_base::is_newline(
			__out_opt size_t *length
			)
		{
			bool result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = is_newline(m_char_position, true, length);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", result);
			return result;
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

			TRACE_ENTRY(TRACE_VERBOSE);
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

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", result);
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

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!has_next_character()) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, pos. %lu", 
					NIMBLE_LEXER_EXCEPTION_STRING(NIMBLE_LEXER_EXCEPTION_NO_NEXT_CHARACTER),
					m_char_position);
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

			ch = character();
			TRACE_MESSAGE(TRACE_INFORMATION, 
				"Moved to next character[%lu] -> \'%c\' (0x%x), cls. %s", 
				m_char_position, std::isprint(ch) ? ch : ' ', ch, 
				CHAR_CLASS_STRING(character_class()));

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. \'%c\' (0x%x)", 
				std::isprint(ch) ? ch : ' ', ch);
			return ch;
		}

		char 
		_nimble_lexer_base::move_previous_character(
			__out_opt char_cls_t *cls
			)
		{
			char ch;
			size_t len;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!has_previous_character()) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, pos. %lu", 
					NIMBLE_LEXER_EXCEPTION_STRING(NIMBLE_LEXER_EXCEPTION_NO_PREVIOUS_CHARACTER),
					m_char_position);
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

			TRACE_MESSAGE(TRACE_INFORMATION, 
				"Moved to previous character[%lu] -> \'%c\' (0x%x), cls. %s", 
				m_char_position, std::isprint(ch) ? ch : ' ', ch, 
				CHAR_CLASS_STRING(character_class()));

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. \'%c\' (0x%x)", 
				std::isprint(ch) ? ch : ' ', ch);
			return ch;
		}

		std::string 
		_nimble_lexer_base::path(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);
			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(m_path));
			return CHK_STR(m_path);
		}

		void 
		_nimble_lexer_base::reset(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			m_char_column = 0;
			m_char_position = 0;
			m_char_row = 0;
			TRACE_MESSAGE(TRACE_INFORMATION, "%s", "Lexer base reset");

			TRACE_EXIT(TRACE_VERBOSE);
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

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			nimble_lexer_base::clear();
			TRACE_MESSAGE(TRACE_INFORMATION, "%s", "Lexer base set");

			if(is_file) {
				TRACE_MESSAGE(TRACE_INFORMATION, "Set input as path -> \'%s\'", 
					CHK_STR(input));

				std::ifstream file(input.c_str(), std::ios::in);
				if(!file) {
					TRACE_MESSAGE(TRACE_ERROR, "%s, %s", 
						NIMBLE_LEXER_EXCEPTION_STRING(NIMBLE_LEXER_EXCEPTION_FILE_NOT_FOUND),
						CHK_STR(input));
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
				TRACE_MESSAGE(TRACE_INFORMATION, "Set input -> \'%s\'", CHK_STR(input));
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

			ch = character();
			TRACE_MESSAGE(TRACE_INFORMATION, 
				"Initial character[%lu] -> \'%c\' (0x%x), cls. %s", 
				m_char_position, std::isprint(ch) ? ch : ' ', ch, 
				CHAR_CLASS_STRING(character_class()));

			m_char_line.insert(std::pair<size_t, std::pair<size_t, std::string>>(m_char_row, 
				std::pair<size_t, std::string>(col, line)));

			TRACE_EXIT(TRACE_VERBOSE);
		}

		size_t 
		_nimble_lexer_base::size(void)
		{
			size_t result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = (m_source.size() - SENTINEL_LEXER_BASE);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		std::string 
		_nimble_lexer_base::source(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);
			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(m_source));
			return CHK_STR(m_source);
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

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(verbose) {
				result << "(" << m_char_position << "/" << size() << ") ";
			}

			ch = character();
			result << "[" << CHAR_CLASS_STRING(character_class()) << "] \'"
				<< nimble_lexer_base::character_as_string(ch, verbose) 
				<< "\' (" << VAL_AS_HEX(uint8_t, ch) << ") \'";

			line = CHK_STR(character_line());
			for(iter = line.begin(); iter != line.end(); ++iter) {
				result << nimble_lexer_base::character_as_string(
					*iter, verbose);
			}

			result << "\' [" << m_char_row << ", " << m_char_column << "]";

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result.str()));
			return CHK_STR(result.str());
		}

		_nimble_lexer::_nimble_lexer(
			__in_opt const std::string &input,
			__in_opt bool is_file
			) :
				m_tok_position(0)
		{
			TRACE_ENTRY(TRACE_VERBOSE);

			nimble_lexer::set(input, is_file);

			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_lexer::_nimble_lexer(
			__in const _nimble_lexer &other
			)
		{
			TRACE_ENTRY(TRACE_VERBOSE);

			nimble_lexer::set(other);

			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_lexer::~_nimble_lexer(void)
		{
			std::vector<nimble_uid>::iterator iter;
			nimble_token_factory_ptr fact = nimble_lexer::acquire_token();

			TRACE_ENTRY(TRACE_VERBOSE);

			try {

				if(fact && fact->is_initialized()) {

					for(iter = m_tok_list.begin(); iter != m_tok_list.end(); ++iter) {

						if(fact->contains(*iter)) {
							fact->decrement_reference(*iter);
						}
					}
				}
			} catch(...) { }

			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_lexer &
		_nimble_lexer::operator=(
			__in const _nimble_lexer &other
			)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(this != &other) {
				nimble_lexer::set(other);
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", this);
			return *this;
		}

		nimble_token_factory_ptr 
		_nimble_lexer::acquire_token(void)
		{
			nimble_token_factory_ptr result = NULL;

			TRACE_ENTRY(TRACE_VERBOSE);

			if(!nimble_token_factory::is_allocated()) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_LEXER_EXCEPTION_STRING(NIMBLE_LEXER_EXCEPTION_COMPONENT_NOT_READY));
				THROW_NIMBLE_LEXER_EXCEPTION(NIMBLE_LEXER_EXCEPTION_COMPONENT_NOT_READY);
			}

			result = nimble_token_factory::acquire();
			if(!result) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_LEXER_EXCEPTION_STRING(NIMBLE_LEXER_EXCEPTION_INVALID_COMPONENT));
				THROW_NIMBLE_LEXER_EXCEPTION(NIMBLE_LEXER_EXCEPTION_INVALID_COMPONENT);
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", result);
			return result;
		}

		void 
		_nimble_lexer::clear(void)
		{
			nimble_token_factory_ptr fact = NULL;
			std::vector<nimble_uid>::iterator iter;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			nimble_lexer_base::reset();
			fact = nimble_lexer::acquire_token();

			try {

				if(fact && fact->is_initialized()) {

					for(iter = m_tok_list.begin(); iter != m_tok_list.end(); ++iter) {

						if(fact->contains(*iter)) {
							fact->decrement_reference(*iter);
						}
					}
				}
			} catch(...) { }

			m_tok_list.clear();
			m_tok_position = 0;
			TRACE_MESSAGE(TRACE_INFORMATION, "%s", "Lexer cleared");

			TRACE_EXIT(TRACE_VERBOSE);
		}

		size_t 
		_nimble_lexer::discover(void)
		{
			size_t result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			while(has_next_token()) {
				move_next_token();
			}

			result = nimble_lexer::size();
			nimble_lexer::reset();
			TRACE_MESSAGE(TRACE_INFORMATION, "Lexer discovered %lu tokens",
				result);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		void 
		_nimble_lexer::enumerate_token(
			__inout nimble_token &tok
			)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			tok.column() = character_column();
			tok.path() = path();
			tok.position() = character_position();
			tok.row() = character_row();

			switch(character_class()) {
				case CHAR_CLASS_ALPHA:
				case CHAR_CLASS_DIGIT:
					enumerate_token_literal(tok);
					break;
				case CHAR_CLASS_SYMBOL:
					enumerate_token_symbol(tok);
					break;
				default:
					TRACE_MESSAGE(TRACE_ERROR, "%s, %s", 
						NIMBLE_LEXER_EXCEPTION_STRING(NIMBLE_LEXER_EXCEPTION_EXPECTING_COMMAND),
						CHK_STR(token_exception(0, true)));
					THROW_NIMBLE_LEXER_EXCEPTION_MESSAGE(NIMBLE_LEXER_EXCEPTION_EXPECTING_COMMAND,
						"%s", CHK_STR(token_exception(0, true)));
			}

			TRACE_EXIT(TRACE_VERBOSE);
		}

		void 
		_nimble_lexer::enumerate_token_literal(
			__inout nimble_token &tok
			)
		{
			char ch;
			char_cls_t cls;
			bool delim = false;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			ch = character();
			switch(character_class()) {
				case CHAR_CLASS_ALPHA:
				case CHAR_CLASS_DIGIT:
				case CHAR_CLASS_SYMBOL:
					tok.type() = TOKEN_LITERAL;

					delim = (ch == CHAR_LITERAL_STRING_DELIMITER);
					if(!delim) {
						tok.text() += ch;
					}

					while(has_next_character()) {
						ch = move_next_character();

						if(delim && (ch == CHAR_LITERAL_STRING_DELIMITER)) {

							if(has_next_character()) {
								move_next_character();
							}

							delim = false;
							break;
						}

						cls = character_class();
						if(!delim
								&& ((cls == CHAR_CLASS_END) 
								|| (cls == CHAR_CLASS_SPACE)
								|| ((cls == CHAR_CLASS_SYMBOL)
								&& ((ch != CHAR_DIRECTORY_MARKER)
								&& (ch != CHAR_DIRECTORY_SEPERATOR_BACKWARD)
								&& (ch != CHAR_DIRECTORY_SEPERATOR_FOREWORD))))) {
							break;
						}

						tok.text() += ch;
					}

					if(delim) {
						TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
							NIMBLE_LEXER_EXCEPTION_STRING(NIMBLE_LEXER_EXCEPTION_UNTERMINATED_LITERAL),
							CHK_STR(character_exception(0, true)));
						THROW_NIMBLE_LEXER_EXCEPTION_MESSAGE(NIMBLE_LEXER_EXCEPTION_UNTERMINATED_LITERAL,
							"\n%s", CHK_STR(character_exception(0, true)));
					}
					break;
				default:
					TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
						NIMBLE_LEXER_EXCEPTION_STRING(NIMBLE_LEXER_EXCEPTION_EXPECTING_LITERAL),
						CHK_STR(character_exception(0, true)));
					THROW_NIMBLE_LEXER_EXCEPTION_MESSAGE(NIMBLE_LEXER_EXCEPTION_EXPECTING_LITERAL,
						"\n%s", CHK_STR(character_exception(0, true)));
			}

			TRACE_EXIT(TRACE_VERBOSE);
		}

		void 
		_nimble_lexer::enumerate_token_symbol(
			__inout nimble_token &tok
			)
		{
			char ch;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(character_class() != CHAR_CLASS_SYMBOL) {
				TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
					NIMBLE_LEXER_EXCEPTION_STRING(NIMBLE_LEXER_EXCEPTION_EXPECTING_SYMBOL),
					CHK_STR(character_exception(0, true)));
				THROW_NIMBLE_LEXER_EXCEPTION_MESSAGE(NIMBLE_LEXER_EXCEPTION_EXPECTING_SYMBOL,
					"\n%s", CHK_STR(character_exception(0, true)));
			}

			ch = character();
			switch(ch) {
				case CHAR_DIRECTORY_MARKER:
				case CHAR_DIRECTORY_SEPERATOR_BACKWARD:
				case CHAR_DIRECTORY_SEPERATOR_FOREWORD:
				case CHAR_LITERAL_STRING_DELIMITER:
					enumerate_token_literal(tok);
					break;
				default:
					tok.type() = TOKEN_SYMBOL;
					tok.text() += ch;

					if(nimble_language::is_symbol(tok.text())) {

						while(has_next_character()) {
							ch = move_next_character();

							tok.text() += ch;
							if(!nimble_language::is_symbol(tok.text())) {
								tok.text() = tok.text().substr(0, tok.text().size() - 1);
								break;
							}
						}

						tok.subtype() = nimble_language::subtype(
							tok.text(), tok.type());
					} else {
						tok.text().clear();
						enumerate_token_literal(tok);
					}
					break;
			}

			TRACE_EXIT(TRACE_VERBOSE);
		}

		bool 
		_nimble_lexer::has_next_token(void)
		{
			bool result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = ((m_tok_position < m_tok_list.size()) 
				&& (token().type() != TOKEN_END));

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", result);
			return result;
		}

		bool 
		_nimble_lexer::has_previous_token(void)
		{
			bool result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = (m_tok_position > 0);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", result);
			return result;
		}

		nimble_token &
		_nimble_lexer::insert_token(
			__in tok_t type,
			__in_opt toksub_t subtype,
			__in_opt size_t position
			)
		{
			nimble_uid uid;
			nimble_token_factory_ptr fact = NULL;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			fact = acquire_token();
			uid = fact->generate();
			nimble_token &tok = fact->at(uid);
			tok.type() = type;
			tok.subtype() = subtype;

			if(position == POS_INVALID) {
				position = m_tok_position;
			}

			if(position > m_tok_list.size()) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, pos. %lu", 
					NIMBLE_LEXER_EXCEPTION_STRING(NIMBLE_LEXER_EXCEPTION_INVALID_TOKEN_POSITION),
					position);
				THROW_NIMBLE_LEXER_EXCEPTION_MESSAGE(NIMBLE_LEXER_EXCEPTION_INVALID_TOKEN_POSITION,
					"%lu", position);
			}

			if(position < m_tok_list.size()) {
				m_tok_list.insert(m_tok_list.begin() + position, uid);
			} else {
				m_tok_list.push_back(uid);
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", 
				CHK_STR(nimble_token::as_string(tok, true)));
			return tok;	
		}

		nimble_token &
		_nimble_lexer::move_next_token(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!has_next_token()) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, pos. %lu", 
					NIMBLE_LEXER_EXCEPTION_STRING(NIMBLE_LEXER_EXCEPTION_NO_NEXT_TOKEN),
					m_tok_position);
				THROW_NIMBLE_LEXER_EXCEPTION_MESSAGE(NIMBLE_LEXER_EXCEPTION_NO_NEXT_TOKEN,
					"%lu", m_tok_position);
			}

			skip_whitespace();

			if(has_next_character() 
					&& (m_tok_position <= (m_tok_list.size() - SENTINEL_LEXER))) {
				enumerate_token(insert_token(TOK_INVALID, TOKSUB_INVALID, m_tok_position + 1));
			}

			++m_tok_position;
			nimble_token &tok = token();
			TRACE_MESSAGE(TRACE_INFORMATION, "Moved to next token[%lu] -> %s", m_tok_position, 
				CHK_STR(tok.to_string(true)));

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", 
				CHK_STR(nimble_token::as_string(tok, true)));			
			return tok;
		}

		nimble_token &
		_nimble_lexer::move_previous_token(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!has_previous_token()) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, pos. %lu", 
					NIMBLE_LEXER_EXCEPTION_STRING(NIMBLE_LEXER_EXCEPTION_NO_PREVIOUS_TOKEN),
					m_tok_position);
				THROW_NIMBLE_LEXER_EXCEPTION_MESSAGE(NIMBLE_LEXER_EXCEPTION_NO_PREVIOUS_TOKEN,
					"%lu", m_tok_position);
			}

			--m_tok_position;
			nimble_token &tok = token();
			TRACE_MESSAGE(TRACE_INFORMATION, "Moved to previous token[%lu] -> %s", m_tok_position, 
				CHK_STR(tok.to_string(true)));

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", 
				CHK_STR(nimble_token::as_string(tok, true)));			
			return tok;
		}

		void 
		_nimble_lexer::reset(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			m_tok_position = 0;
			TRACE_MESSAGE(TRACE_INFORMATION, "%s", "Lexer reset");

			TRACE_EXIT(TRACE_VERBOSE);
		}

		void 
		_nimble_lexer::set(
			__in_opt const std::string &input,
			__in_opt bool is_file
			)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			nimble_lexer::clear();
			nimble_lexer_base::set(input, is_file);
			insert_token(TOKEN_BEGIN, TOKSUB_INVALID, 0);
			insert_token(TOKEN_END, TOKSUB_INVALID, 1);
			nimble_lexer::reset();
			TRACE_MESSAGE(TRACE_INFORMATION, "Lexer set input -> \'%s\', file -> 0x%x", 
				CHK_STR(input), is_file);

			TRACE_EXIT(TRACE_VERBOSE);
		}

		void 
		_nimble_lexer::set(
			__in const _nimble_lexer &other
			)
		{
			nimble_token_factory_ptr fact = NULL;
			std::vector<nimble_uid>::iterator iter;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			nimble_lexer::clear();
			nimble_lexer_base::operator=(other);
			m_tok_list = other.m_tok_list;
			m_tok_position = other.m_tok_position;
			fact = nimble_lexer::acquire_token();

			try {

				if(fact && fact->is_initialized()) {

					for(iter = m_tok_list.begin(); iter != m_tok_list.end(); ++iter) {

						if(fact->contains(*iter)) {
							fact->increment_reference(*iter);
						}
					}
				}
			} catch(...) { }

			TRACE_EXIT(TRACE_VERBOSE);
		}

		size_t 
		_nimble_lexer::size(void)
		{
			size_t result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = (m_tok_list.size() - SENTINEL_LEXER);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		void 
		_nimble_lexer::skip_whitespace(void)
		{
			size_t len;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			while(has_next_character()) {

				if(character_class() != CHAR_CLASS_SPACE) {
					break;
				}

				move_next_character();
			}

			if(character() == CHAR_COMMENT) {

				while(has_next_character()) {
					len = 0;

					if(is_newline(&len)) {

						if(len == CHAR_NEWLINE_LONG_LEN) {
							move_next_character();
						}

						move_next_character();
						break;
					}

					move_next_character();
				}
			}

			if(has_next_character() 
					&& ((character() == CHAR_COMMENT) 
					|| (character_class() == CHAR_CLASS_SPACE))) {
				skip_whitespace();
			}

			TRACE_EXIT(TRACE_VERBOSE);
		}

		std::string 
		_nimble_lexer::to_string(
			__in_opt bool verbose
			)
		{
			std::stringstream result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(verbose) {
				result << "(" << m_tok_position << "/" << (m_tok_list.size() - 1) << ") ";
			}

			if(m_tok_position >= m_tok_list.size()) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, pos. %lu", 
					NIMBLE_LEXER_EXCEPTION_STRING(NIMBLE_LEXER_EXCEPTION_INVALID_TOKEN_POSITION),
					m_tok_position);
				THROW_NIMBLE_LEXER_EXCEPTION_MESSAGE(NIMBLE_LEXER_EXCEPTION_INVALID_TOKEN_POSITION,
					"%lu", m_tok_position);
			}

			result << nimble_lexer::token_as_string(m_tok_list.at(m_tok_position), verbose);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result.str()));
			return CHK_STR(result.str());
		}

		nimble_token &
		_nimble_lexer::token(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(m_tok_position >= m_tok_list.size()) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, pos. %lu", 
					NIMBLE_LEXER_EXCEPTION_STRING(NIMBLE_LEXER_EXCEPTION_INVALID_TOKEN_POSITION),
					m_tok_position);
				THROW_NIMBLE_LEXER_EXCEPTION_MESSAGE(NIMBLE_LEXER_EXCEPTION_INVALID_TOKEN_POSITION,
					"%lu", m_tok_position);
			}

			nimble_token &tok = acquire_token()->at(m_tok_list.at(m_tok_position));

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", 
				CHK_STR(nimble_token::as_string(tok, true)));
			return tok;
		}

		std::string 
		_nimble_lexer::token_as_string(
			__in const nimble_uid &uid,
			__in_opt bool verbose
			)
		{
			std::string result;

			TRACE_ENTRY(TRACE_VERBOSE);

			result = nimble_token::as_string(acquire_token()->at(uid), verbose);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result));
			return CHK_STR(result);
		}

		std::string 
		_nimble_lexer::token_exception(
			__in_opt size_t tabs,
			__in_opt bool verbose
			)
		{
			nimble_token tok;
			nimble_token_meta meta;
			std::stringstream result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			tok = token();
			meta = tok.meta();
			result << tok.to_string(verbose) << std::endl
				<< nimble_lexer_base::character_exception(meta, tabs, verbose);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result.str()));
			return CHK_STR(result.str());
		}
	}
}
