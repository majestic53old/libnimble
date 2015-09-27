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
#include "../include/nimble_executor_type.h"

namespace NIMBLE {

	namespace LANGUAGE {

		#define STMT_ARGUMENT_CHILD_COUNT 1
		#define STMT_ASSIGNMENT_CHILD_COUNT 2
		#define STMT_ASSIGNMENT_CHILD_LEFT 0
		#define STMT_ASSIGNMENT_CHILD_RIGHT 1
		#define STMT_CALL_CHILD_COUNT_MIN 1
		#define STMT_CHILD_COUNT 1
		#define STMT_COMMAND_LIST_CHILD_COUNT 1
		#define STMT_LITERAL_CHILD_COUNT 0

		_nimble_executor::_nimble_executor(
			__in_opt const std::string &input,
			__in_opt bool is_file
			)
		{
			TRACE_ENTRY(TRACE_VERBOSE);

			nimble_executor::set(input, is_file);

			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_executor::_nimble_executor(
			__in const _nimble_executor &other
			)
		{
			TRACE_ENTRY(TRACE_VERBOSE);

			nimble_executor::set(other);

			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_executor::~_nimble_executor(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_executor &
		_nimble_executor::operator=(
			__in const _nimble_executor &other
			)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(this != &other) {
				nimble_executor::set(other);
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", this);
			return *this;
		}

		void 
		_nimble_executor::clear(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			nimble_parser::clear();

			TRACE_EXIT(TRACE_VERBOSE);
		}

		int 
		_nimble_executor::evaluate(
			__inout_opt void *environment
			)
		{
			int result = 0;
			nimble_statement stmt;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			nimble_parser::reset();

			while(has_next_statement()) {
				stmt = statement();

				switch(node_token(stmt.front()).type()) {
					case TOKEN_BEGIN:
					case TOKEN_END:
						break;
					case TOKEN_STATEMENT:
						evaluate_statement(result, stmt, PAR_INVALID, environment);
						break;
					default:
						TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
							NIMBLE_EXECUTOR_EXCEPTION_STRING(NIMBLE_EXECUTOR_EXCEPTION_EXPECTING_STATEMENT),
							CHK_STR(nimble_parser::statement_exception(0, true)));
						THROW_NIMBLE_EXECUTOR_EXCEPTION_MESSAGE(NIMBLE_EXECUTOR_EXCEPTION_EXPECTING_STATEMENT,
							"%s", CHK_STR(nimble_parser::statement_exception(0, true)));
				}

				move_next_statement();
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", result);
			return result;
		}

		size_t 
		_nimble_executor::evaluate_statement(
			__out int &status,
			__in const nimble_statement &stmt,
			__in_opt size_t parent,
			__inout_opt void *environment
			)
		{
			size_t result = parent;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if((result != PAR_INVALID) && (result >= stmt.size())) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, %lu\n%s", 
					NIMBLE_EXECUTOR_EXCEPTION_STRING(NIMBLE_EXECUTOR_EXCEPTION_INVALID_PARENT),
					CHK_STR(nimble_parser::statement_exception(0, true)), result);
				THROW_NIMBLE_EXECUTOR_EXCEPTION_MESSAGE(NIMBLE_EXECUTOR_EXCEPTION_INVALID_PARENT,
					"%s, pos. %lu", CHK_STR(nimble_parser::statement_exception(0, true)), 
					result);
			}

			result = (result != PAR_INVALID) ? result : 0;

			nimble_node nd = node(stmt.at(result));
			if(node_token(nd).type() != TOKEN_STATEMENT) {
				TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
					NIMBLE_EXECUTOR_EXCEPTION_STRING(NIMBLE_EXECUTOR_EXCEPTION_EXPECTING_STATEMENT),
					CHK_STR(nimble_parser::statement_exception(0, true)));
				THROW_NIMBLE_EXECUTOR_EXCEPTION_MESSAGE(NIMBLE_EXECUTOR_EXCEPTION_EXPECTING_STATEMENT,
					"%s", CHK_STR(nimble_parser::statement_exception(0, true)));
			}

			if(nd.children().size() != STMT_CHILD_COUNT) {
				TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
					NIMBLE_EXECUTOR_EXCEPTION_STRING(NIMBLE_EXECUTOR_EXCEPTION_MALFORMED_STATEMENT),
					CHK_STR(nimble_parser::statement_exception(0, true)));
				THROW_NIMBLE_EXECUTOR_EXCEPTION_MESSAGE(NIMBLE_EXECUTOR_EXCEPTION_MALFORMED_STATEMENT,
					"%s", CHK_STR(nimble_parser::statement_exception(0, true)));
			}

			result = nd.children().front();
			if((result != PAR_INVALID) && (result >= stmt.size())) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, %lu\n%s", 
					NIMBLE_EXECUTOR_EXCEPTION_STRING(NIMBLE_EXECUTOR_EXCEPTION_INVALID_CHILD),
					CHK_STR(nimble_parser::statement_exception(0, true)), result);
				THROW_NIMBLE_EXECUTOR_EXCEPTION_MESSAGE(NIMBLE_EXECUTOR_EXCEPTION_INVALID_CHILD,
					"%s, pos. %lu", CHK_STR(nimble_parser::statement_exception(0, true)), 
					result);
			}

			switch(node_token(stmt.at(result)).type()) {
				case TOKEN_ASSIGNMENT:
					result = evaluate_statement_assignment(status, stmt, result, environment);
					break;
				case TOKEN_COMMAND:
					result = evaluate_statement_command(status, stmt, result, environment);
					break;
				default:
					TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
						NIMBLE_EXECUTOR_EXCEPTION_STRING(NIMBLE_EXECUTOR_EXCEPTION_EXPECTING_STATEMENT),
						CHK_STR(nimble_parser::statement_exception(0, true)));
					THROW_NIMBLE_EXECUTOR_EXCEPTION_MESSAGE(NIMBLE_EXECUTOR_EXCEPTION_EXPECTING_STATEMENT,
						"%s", CHK_STR(nimble_parser::statement_exception(0, true)));
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		std::string 
		_nimble_executor::evaluate_statement_argument(
			__in const nimble_statement &stmt,
			__in_opt size_t parent,
			__inout_opt void *environment
			)
		{
			std::string result;
			size_t par = parent;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if((par != PAR_INVALID) && (par >= stmt.size())) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, %lu\n%s", 
					NIMBLE_EXECUTOR_EXCEPTION_STRING(NIMBLE_EXECUTOR_EXCEPTION_INVALID_PARENT),
					CHK_STR(nimble_parser::statement_exception(0, true)), par);
				THROW_NIMBLE_EXECUTOR_EXCEPTION_MESSAGE(NIMBLE_EXECUTOR_EXCEPTION_INVALID_PARENT,
					"%s, pos. %lu", CHK_STR(nimble_parser::statement_exception(0, true)), 
					par);
			}

			par = (par != PAR_INVALID) ? par : 0;

			nimble_node nd = node(stmt.at(par));
			if(node_token(nd).type() != TOKEN_ARGUMENT) {
				TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
					NIMBLE_EXECUTOR_EXCEPTION_STRING(NIMBLE_EXECUTOR_EXCEPTION_EXPECTING_ARGUMENT),
					CHK_STR(nimble_parser::statement_exception(0, true)));
				THROW_NIMBLE_EXECUTOR_EXCEPTION_MESSAGE(NIMBLE_EXECUTOR_EXCEPTION_EXPECTING_ARGUMENT,
					"%s", CHK_STR(nimble_parser::statement_exception(0, true)));
			}

			if(nd.children().size() != STMT_ARGUMENT_CHILD_COUNT) {
				TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
					NIMBLE_EXECUTOR_EXCEPTION_STRING(NIMBLE_EXECUTOR_EXCEPTION_MALFORMED_STATEMENT),
					CHK_STR(nimble_parser::statement_exception(0, true)));
				THROW_NIMBLE_EXECUTOR_EXCEPTION_MESSAGE(NIMBLE_EXECUTOR_EXCEPTION_MALFORMED_STATEMENT,
					"%s", CHK_STR(nimble_parser::statement_exception(0, true)));
			}

			result = evaluate_statement_literal(stmt, nd.children().front(), environment);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result));
			return result;
		}

		size_t 
		_nimble_executor::evaluate_statement_assignment(
			__out int &status,
			__in const nimble_statement &stmt,
			__in_opt size_t parent,
			__inout_opt void *environment
			)
		{
			bool right_arg = false;
			size_t result = parent;
			nimble_ptr inst = NULL;
			std::string field, value;
			
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if((result != PAR_INVALID) && (result >= stmt.size())) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, %lu\n%s", 
					NIMBLE_EXECUTOR_EXCEPTION_STRING(NIMBLE_EXECUTOR_EXCEPTION_INVALID_PARENT),
					CHK_STR(nimble_parser::statement_exception(0, true)), result);
				THROW_NIMBLE_EXECUTOR_EXCEPTION_MESSAGE(NIMBLE_EXECUTOR_EXCEPTION_INVALID_PARENT,
					"%s, pos. %lu", CHK_STR(nimble_parser::statement_exception(0, true)), 
					result);
			}

			result = (result != PAR_INVALID) ? result : 0;

			nimble_node nd = node(stmt.at(result));
			if(node_token(nd).type() != TOKEN_ASSIGNMENT) {
				TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
					NIMBLE_EXECUTOR_EXCEPTION_STRING(NIMBLE_EXECUTOR_EXCEPTION_EXPECTING_ASSIGNMENT),
					CHK_STR(nimble_parser::statement_exception(0, true)));
				THROW_NIMBLE_EXECUTOR_EXCEPTION_MESSAGE(NIMBLE_EXECUTOR_EXCEPTION_EXPECTING_ASSIGNMENT,
					"%s", CHK_STR(nimble_parser::statement_exception(0, true)));
			}

			if(nd.children().size() != STMT_ASSIGNMENT_CHILD_COUNT) {
				TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
					NIMBLE_EXECUTOR_EXCEPTION_STRING(NIMBLE_EXECUTOR_EXCEPTION_MALFORMED_STATEMENT),
					CHK_STR(nimble_parser::statement_exception(0, true)));
				THROW_NIMBLE_EXECUTOR_EXCEPTION_MESSAGE(NIMBLE_EXECUTOR_EXCEPTION_MALFORMED_STATEMENT,
					"%s", CHK_STR(nimble_parser::statement_exception(0, true)));
			}

			field = evaluate_statement_argument(stmt, nd.children().at(STMT_ASSIGNMENT_CHILD_LEFT), environment);

			switch(node_token(stmt.at(nd.children().at(STMT_ASSIGNMENT_CHILD_RIGHT))).type()) {
				case TOKEN_ARGUMENT:					
					value = evaluate_statement_argument(stmt, nd.children().at(STMT_ASSIGNMENT_CHILD_RIGHT), 
							environment);
					right_arg = true;
					break;
				case TOKEN_LITERAL:
					value = evaluate_statement_literal(stmt, nd.children().at(STMT_ASSIGNMENT_CHILD_RIGHT), 
							environment);
					break;
				default:
					TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
						NIMBLE_EXECUTOR_EXCEPTION_STRING(NIMBLE_EXECUTOR_EXCEPTION_MALFORMED_STATEMENT),
						CHK_STR(nimble_parser::statement_exception(0, true)));
					THROW_NIMBLE_EXECUTOR_EXCEPTION_MESSAGE(NIMBLE_EXECUTOR_EXCEPTION_MALFORMED_STATEMENT,
						"%s", CHK_STR(nimble_parser::statement_exception(0, true)));
			}

			inst = nimble::acquire();

			if(right_arg) {

				if(!inst->environment_contains(value)) {
					TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
						NIMBLE_EXECUTOR_EXCEPTION_STRING(NIMBLE_EXECUTOR_EXCEPTION_UNDEFINED_ARGUMENT),
						CHK_STR(value));
					THROW_NIMBLE_EXECUTOR_EXCEPTION_MESSAGE(NIMBLE_EXECUTOR_EXCEPTION_UNDEFINED_ARGUMENT,
						"%s", CHK_STR(value));
				}

				value = inst->environment_find(value)->second;
			}

			if(environment) {
				nimble_environment::add(environment, field, value);
			}

			inst->environment_set(field, value);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		size_t 
		_nimble_executor::evaluate_statement_call(
			__out int &status,
			__in const nimble_statement &stmt,
			__in_opt size_t parent,
			__inout_opt void *environment
			)
		{
			std::string ar;
			nimble_ptr inst = NULL;
			std::vector<char *> args;
			size_t iter, result = parent;
			std::vector<std::string> call;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if((result != PAR_INVALID) && (result >= stmt.size())) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, %lu\n%s", 
					NIMBLE_EXECUTOR_EXCEPTION_STRING(NIMBLE_EXECUTOR_EXCEPTION_INVALID_PARENT),
					CHK_STR(nimble_parser::statement_exception(0, true)), result);
				THROW_NIMBLE_EXECUTOR_EXCEPTION_MESSAGE(NIMBLE_EXECUTOR_EXCEPTION_INVALID_PARENT,
					"%s, pos. %lu", CHK_STR(nimble_parser::statement_exception(0, true)), 
					result);
			}

			result = (result != PAR_INVALID) ? result : 0;

			nimble_node nd = node(stmt.at(result));
			if(node_token(nd).type() != TOKEN_CALL) {
				TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
					NIMBLE_EXECUTOR_EXCEPTION_STRING(NIMBLE_EXECUTOR_EXCEPTION_EXPECTING_CALL),
					CHK_STR(nimble_parser::statement_exception(0, true)));
				THROW_NIMBLE_EXECUTOR_EXCEPTION_MESSAGE(NIMBLE_EXECUTOR_EXCEPTION_EXPECTING_CALL,
					"%s", CHK_STR(nimble_parser::statement_exception(0, true)));
			}

			if(nd.children().size() < STMT_CALL_CHILD_COUNT_MIN) {
				TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
					NIMBLE_EXECUTOR_EXCEPTION_STRING(NIMBLE_EXECUTOR_EXCEPTION_MALFORMED_STATEMENT),
					CHK_STR(nimble_parser::statement_exception(0, true)));
				THROW_NIMBLE_EXECUTOR_EXCEPTION_MESSAGE(NIMBLE_EXECUTOR_EXCEPTION_MALFORMED_STATEMENT,
					"%s", CHK_STR(nimble_parser::statement_exception(0, true)));
			}

			inst = nimble::acquire();

			for(iter = 0; iter < nd.children().size(); ++iter) {

				if(node_token(stmt.at(nd.children().at(iter))).type() == TOKEN_ARGUMENT) {
					ar = evaluate_statement_argument(stmt, nd.children().at(iter), 
						environment);

					if(!inst->environment_contains(ar)) {
						TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
							NIMBLE_EXECUTOR_EXCEPTION_STRING(NIMBLE_EXECUTOR_EXCEPTION_UNDEFINED_ARGUMENT),
							CHK_STR(ar));
						THROW_NIMBLE_EXECUTOR_EXCEPTION_MESSAGE(NIMBLE_EXECUTOR_EXCEPTION_UNDEFINED_ARGUMENT,
							"%s", CHK_STR(ar));
					}

					call.push_back(inst->environment_find(ar)->second);
				} else {
					call.push_back(evaluate_statement_literal(stmt, nd.children().at(iter), 
						environment));
				}
			}

			for(iter = 0; iter < call.size(); ++iter) {
				args.push_back((char *) call.at(iter).c_str());
			}

			args.push_back(NULL);

			if(call.front() == CMD_EXIT) {
				nimble_environment::flag_set(environment, ENV_FLAG_EXIT);
			}

			status = execv(call.front().c_str(), &args[0]);

			// TODO: handle redirection/pipes

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		size_t 
		_nimble_executor::evaluate_statement_command(
			__out int &status,
			__in const nimble_statement &stmt,
			__in_opt size_t parent,
			__inout_opt void *environment
			)
		{
			size_t iter = 0, result = parent;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if((result != PAR_INVALID) && (result >= stmt.size())) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, %lu\n%s", 
					NIMBLE_EXECUTOR_EXCEPTION_STRING(NIMBLE_EXECUTOR_EXCEPTION_INVALID_PARENT),
					CHK_STR(nimble_parser::statement_exception(0, true)), result);
				THROW_NIMBLE_EXECUTOR_EXCEPTION_MESSAGE(NIMBLE_EXECUTOR_EXCEPTION_INVALID_PARENT,
					"%s, pos. %lu", CHK_STR(nimble_parser::statement_exception(0, true)), 
					result);
			}

			result = (result != PAR_INVALID) ? result : 0;

			nimble_node nd = node(stmt.at(result));
			if(node_token(nd).type() == TOKEN_COMMAND) {

				if(nd.children().size() != STMT_COMMAND_LIST_CHILD_COUNT) {
					TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
						NIMBLE_EXECUTOR_EXCEPTION_STRING(NIMBLE_EXECUTOR_EXCEPTION_MALFORMED_STATEMENT),
						CHK_STR(nimble_parser::statement_exception(0, true)));
					THROW_NIMBLE_EXECUTOR_EXCEPTION_MESSAGE(NIMBLE_EXECUTOR_EXCEPTION_MALFORMED_STATEMENT,
						"%s", CHK_STR(nimble_parser::statement_exception(0, true)));
				}

				if(nd.children().front() >= stmt.size()) {
					TRACE_MESSAGE(TRACE_ERROR, "%s, %lu\n%s", 
						NIMBLE_EXECUTOR_EXCEPTION_STRING(NIMBLE_EXECUTOR_EXCEPTION_INVALID_CHILD),
						CHK_STR(nimble_parser::statement_exception(0, true)), 
						nd.children().front());
					THROW_NIMBLE_EXECUTOR_EXCEPTION_MESSAGE(NIMBLE_EXECUTOR_EXCEPTION_INVALID_CHILD,
						"%s, pos. %lu", CHK_STR(nimble_parser::statement_exception(0, true)), 
						nd.children().front());
				}

				nd = node(stmt.at(nd.children().front()));
			}

			switch(node_token(nd).type()) {
				case TOKEN_CALL_LIST:

					for(; iter < nd.children().size(); ++iter) {
						evaluate_statement_call(status, stmt, nd.children().at(iter), environment);

						if(status < 0) {
							break;
						}
					}
					break;
				case TOKEN_COMMAND:
					evaluate_statement_command(status, stmt, nd.children().front(), environment);
					break;
				default:
					TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
						NIMBLE_EXECUTOR_EXCEPTION_STRING(NIMBLE_EXECUTOR_EXCEPTION_EXPECTING_CALL_LIST),
						CHK_STR(nimble_parser::statement_exception(0, true)));
					THROW_NIMBLE_EXECUTOR_EXCEPTION_MESSAGE(NIMBLE_EXECUTOR_EXCEPTION_EXPECTING_CALL_LIST,
						"%s", CHK_STR(nimble_parser::statement_exception(0, true)));
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		std::string 
		_nimble_executor::evaluate_statement_literal(
			__in const nimble_statement &stmt,
			__in_opt size_t parent,
			__inout_opt void *environment
			)
		{
			std::string result;
			size_t par = parent;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if((par != PAR_INVALID) && (par >= stmt.size())) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, %lu\n%s", 
					NIMBLE_EXECUTOR_EXCEPTION_STRING(NIMBLE_EXECUTOR_EXCEPTION_INVALID_PARENT),
					CHK_STR(nimble_parser::statement_exception(0, true)), par);
				THROW_NIMBLE_EXECUTOR_EXCEPTION_MESSAGE(NIMBLE_EXECUTOR_EXCEPTION_INVALID_PARENT,
					"%s, pos. %lu", CHK_STR(nimble_parser::statement_exception(0, true)), 
					par);
			}

			par = (par != PAR_INVALID) ? par : 0;
			nimble_node nd = node(stmt.at(par));

			nimble_token tok = node_token(nd);
			if(tok.type() != TOKEN_LITERAL) {
				TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
					NIMBLE_EXECUTOR_EXCEPTION_STRING(NIMBLE_EXECUTOR_EXCEPTION_EXPECTING_LITERAL),
					CHK_STR(nimble_parser::statement_exception(0, true)));
				THROW_NIMBLE_EXECUTOR_EXCEPTION_MESSAGE(NIMBLE_EXECUTOR_EXCEPTION_EXPECTING_LITERAL,
					"%s", CHK_STR(nimble_parser::statement_exception(0, true)));
			}

			if(nd.children().size() != STMT_LITERAL_CHILD_COUNT) {
				TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
					NIMBLE_EXECUTOR_EXCEPTION_STRING(NIMBLE_EXECUTOR_EXCEPTION_MALFORMED_STATEMENT),
					CHK_STR(nimble_parser::statement_exception(0, true)));
				THROW_NIMBLE_EXECUTOR_EXCEPTION_MESSAGE(NIMBLE_EXECUTOR_EXCEPTION_MALFORMED_STATEMENT,
					"%s", CHK_STR(nimble_parser::statement_exception(0, true)));
			}

			result = tok.text();
			if(result.empty()) {
				TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
					NIMBLE_EXECUTOR_EXCEPTION_STRING(NIMBLE_EXECUTOR_EXCEPTION_MALFORMED_STATEMENT),
					CHK_STR(nimble_parser::statement_exception(0, true)));
				THROW_NIMBLE_EXECUTOR_EXCEPTION_MESSAGE(NIMBLE_EXECUTOR_EXCEPTION_MALFORMED_STATEMENT,
					"%s", CHK_STR(nimble_parser::statement_exception(0, true)));
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result));
			return result;
		}

		void 
		_nimble_executor::set(
			__in_opt const std::string &input,
			__in_opt bool is_file
			)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			nimble_parser::set(input, is_file);

			TRACE_EXIT(TRACE_VERBOSE);
		}

		void 
		_nimble_executor::set(
			__in const _nimble_executor &other
			)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			nimble_parser::set(other);

			TRACE_EXIT(TRACE_VERBOSE);
		}
	}
}
