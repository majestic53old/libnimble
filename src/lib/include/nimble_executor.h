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

#ifndef NIMBLE_EXECUTOR_H_
#define NIMBLE_EXECUTOR_H_

namespace NIMBLE {

	namespace LANGUAGE {

		typedef class _nimble_executor :
				protected nimble_parser {

			public:

				_nimble_executor(
					__in_opt const std::string &input = std::string(),
					__in_opt bool is_file = false
					);

				_nimble_executor(
					__in const _nimble_executor &other
					);

				virtual ~_nimble_executor(void);

				_nimble_executor &operator=(
					__in const _nimble_executor &other
					);

				virtual void clear(void);

				virtual int evaluate(
					__inout_opt void *environment = NULL
					);

				virtual void set(
					__in_opt const std::string &input,
					__in_opt bool is_file = false
					);

				void set(
					__in const _nimble_executor &other
					);

			protected:

				size_t evaluate_statement(
					__out int &status,
					__in const nimble_statement &stmt,
					__in_opt size_t parent = PAR_INVALID,
					__inout_opt void *environment = NULL
					);

				std::string evaluate_statement_argument(
					__in const nimble_statement &stmt,
					__in_opt size_t parent = PAR_INVALID,
					__inout_opt void *environment = NULL
					);

				size_t evaluate_statement_assignment(
					__out int &status,
					__in const nimble_statement &stmt,
					__in_opt size_t parent = PAR_INVALID,
					__inout_opt void *environment = NULL
					);

				size_t evaluate_statement_command(
					__out int &status,
					__in const nimble_statement &stmt,
					__in_opt size_t parent = PAR_INVALID,
					__inout_opt void *environment = NULL
					);

				std::string evaluate_statement_literal(
					__in const nimble_statement &stmt,
					__in_opt size_t parent = PAR_INVALID,
					__inout_opt void *environment = NULL
					);

			private:

				std::recursive_mutex m_lock;

		} nimble_executor, *nimble_executor_ptr;
	}
}

#endif // NIMBLE_EXECUTOR_H_
