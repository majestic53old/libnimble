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
#include "../include/nimble_parser_type.h"

namespace NIMBLE {

	namespace LANGUAGE {

		#define SENTINEL_PARSER 2

		_nimble_parser::_nimble_parser(
			__in_opt const std::string &input,
			__in_opt bool is_file
			)
		{
			TRACE_ENTRY(TRACE_VERBOSE);

			// TODO

			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_parser::_nimble_parser(
			__in const _nimble_parser &other
			)
		{
			TRACE_ENTRY(TRACE_VERBOSE);

			// TODO

			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_parser::~_nimble_parser(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);

			// TODO

			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_parser &
		_nimble_parser::operator=(
			__in const _nimble_parser &other
			)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(this != &other) {

				// TODO

			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", this);
			return *this;
		}

		void 
		_nimble_parser::clear(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			// TODO

			TRACE_EXIT(TRACE_VERBOSE);
		}

		size_t 
		_nimble_parser::discover(void)
		{
			size_t result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			while(has_next_statement()) {
				move_next_statement();
			}

			result = nimble_parser::size();
			nimble_parser::reset();

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		bool 
		_nimble_parser::has_next_statement(void)
		{
			bool result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			// TODO

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", result);
			return result;
		}

		bool 
		_nimble_parser::has_previous_statement(void)
		{
			bool result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = (m_stmt_position > 0);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", result);
			return result;
		}

		nimble_statement &
		_nimble_parser::move_next_statement(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!has_next_statement()) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, pos. %lu", 
					NIMBLE_PARSER_EXCEPTION_STRING(NIMBLE_PARSER_EXCEPTION_NO_NEXT_STATEMENT),
					m_stmt_position);
				THROW_NIMBLE_PARSER_EXCEPTION_MESSAGE(NIMBLE_PARSER_EXCEPTION_NO_NEXT_STATEMENT,
					"%lu", m_stmt_position);
			}

			// TODO

			nimble_statement &stmt = statement();

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", 
				CHK_STR(nimble_parser::statement_as_string(stmt, true)));
			return stmt;
		}

		nimble_statement &
		_nimble_parser::move_previous_statement(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!has_previous_statement()) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, pos. %lu", 
					NIMBLE_PARSER_EXCEPTION_STRING(NIMBLE_PARSER_EXCEPTION_NO_PREVIOUS_STATEMENT),
					m_stmt_position);
				THROW_NIMBLE_PARSER_EXCEPTION_MESSAGE(NIMBLE_PARSER_EXCEPTION_NO_PREVIOUS_STATEMENT,
					"%lu", m_stmt_position);
			}

			// TODO

			nimble_statement &stmt = statement();

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", 
				CHK_STR(nimble_parser::statement_as_string(stmt, true)));
			return stmt;
		}

		void 
		_nimble_parser::reset(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			m_stmt_position = 0;

			TRACE_EXIT(TRACE_VERBOSE);
		}

		void 
		_nimble_parser::set(
			__in_opt const std::string &input,
			__in_opt bool is_file
			)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			// TODO

			TRACE_EXIT(TRACE_VERBOSE);
		}

		void 
		_nimble_parser::set(
			__in const _nimble_parser &other
			)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			// TODO

			TRACE_EXIT(TRACE_VERBOSE);
		}

		size_t 
		_nimble_parser::size(void)
		{
			size_t result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = (!m_stmt_list.empty() ? (m_stmt_list.size() - SENTINEL_PARSER) : 0);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		nimble_statement &
		_nimble_parser::statement(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(m_stmt_position >= m_stmt_list.size()) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, pos. %lu", 
					NIMBLE_PARSER_EXCEPTION_STRING(NIMBLE_PARSER_EXCEPTION_INVALID_STATEMENT_POSITION),
					m_stmt_position);
				THROW_NIMBLE_PARSER_EXCEPTION_MESSAGE(NIMBLE_PARSER_EXCEPTION_INVALID_STATEMENT_POSITION,
					"%lu", m_stmt_position);
			}

			nimble_statement &stmt = m_stmt_list.at(m_stmt_position);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", 
				CHK_STR(nimble_parser::statement_as_string(stmt, true)));
			return stmt;
		}

		std::string 
		_nimble_parser::statement_as_string(
			__in const nimble_statement &statement,
			__in_opt bool verbose
			)
		{
			std::stringstream result;

			TRACE_ENTRY(TRACE_VERBOSE);

			// TODO

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result.str()));
			return CHK_STR(result.str());
		}

		std::string 
		_nimble_parser::statement_exception(
			__in_opt size_t tabs,
			__in_opt bool verbose
			)
		{
			std::stringstream result;

			TRACE_ENTRY(TRACE_VERBOSE);

			// TODO

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result.str()));
			return CHK_STR(result.str());
		}

		size_t 
		_nimble_parser::statement_position(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);
			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", m_stmt_position);
			return m_stmt_position;
		}

		std::string 
		_nimble_parser::to_string(
			__in_opt bool verbose
			)
		{
			std::string result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = nimble_parser::statement_as_string(statement(), verbose);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result));
			return CHK_STR(result);
		}
	}
}
