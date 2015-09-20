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

#ifndef NIMBLE_PARSER_H_
#define NIMBLE_PARSER_H_

#include <vector>

namespace NIMBLE {

	namespace LANGUAGE {

		typedef std::vector<nimble_uid> nimble_statement;

		typedef class _nimble_parser :
				protected nimble_lexer {

			public:

				_nimble_parser(
					__in_opt const std::string &input = std::string(),
					__in_opt bool is_file = false
					);

				_nimble_parser(
					__in const _nimble_parser &other
					);

				virtual ~_nimble_parser(void);

				_nimble_parser &operator=(
					__in const _nimble_parser &other
					);

				virtual void clear(void);

				virtual size_t discover(void);

				bool has_next_statement(void);

				bool has_previous_statement(void);

				nimble_statement &move_next_statement(void);

				nimble_statement &move_previous_statement(void);

				virtual void reset(void);

				virtual void set(
					__in_opt const std::string &input = std::string(),
					__in_opt bool is_file = false
					);

				void set(
					__in const _nimble_parser &other
					);

				virtual size_t size(void);

				nimble_statement &statement(void);

				nimble_statement &statement_begin(void);

				nimble_statement &statement_end(void);

				static std::string statement_as_string(
					__in const nimble_statement &stmt,
					__in_opt bool verbose = false
					);

				std::string statement_exception(
					__in_opt size_t tabs = 0,
					__in_opt bool verbose = false
					);

				size_t statement_position(void);

				virtual std::string to_string(
					__in_opt bool verbose = false
					);

			protected:

				static void _statement_as_string(
					__out std::stringstream &stream,
					__in const nimble_statement &stmt,
					__in_opt size_t position = 0,
					__in_opt size_t tabs = 0,
					__in_opt bool verbose = false
					);

				static nimble_node_factory_ptr acquire_node(void);

				size_t insert_node(
					__in nimble_statement &stmt,
					__in const nimble_uid &token,
					__in_opt size_t parent = PAR_INVALID
					);

				void insert_statement(
					__in const nimble_statement &stmt
					);

				static nimble_token &node_token(
					__in const nimble_uid &uid
					);

				std::vector<nimble_statement> m_stmt_list;

				size_t m_stmt_position;

			private:

				std::recursive_mutex m_lock;

		} nimble_parser, *nimble_parser_ptr;
	}
}

#endif // NIMBLE_PARSER_H_
