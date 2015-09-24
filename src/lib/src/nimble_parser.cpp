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
			) :
				m_stmt_position(0)
		{
			TRACE_ENTRY(TRACE_VERBOSE);

			nimble_parser::set(input, is_file);

			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_parser::_nimble_parser(
			__in const _nimble_parser &other
			)
		{
			TRACE_ENTRY(TRACE_VERBOSE);

			nimble_parser::set(other);

			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_parser::~_nimble_parser(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);

			nimble_parser::clear();

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
				nimble_parser::set(other);
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", this);
			return *this;
		}

		void 
		_nimble_parser::_statement_as_string(
			__out std::stringstream &stream,
			__in const nimble_statement &stmt,
			__in_opt size_t position,
			__in_opt size_t tabs,
			__in_opt bool verbose
			)
		{
			size_t tab_iter;
			std::vector<size_t>::iterator child_iter;

			TRACE_ENTRY(TRACE_VERBOSE);

			for(tab_iter = 0; tab_iter < tabs; ++tab_iter) {
				stream << "\t";
			}

			if(position >= stmt.size()) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, pos. %lu", 
					NIMBLE_PARSER_EXCEPTION_STRING(NIMBLE_PARSER_EXCEPTION_INVALID_STATEMENT_POSITION),
					position);
				THROW_NIMBLE_PARSER_EXCEPTION_MESSAGE(NIMBLE_PARSER_EXCEPTION_INVALID_STATEMENT_POSITION,
					"%lu", position);
			}

			nimble_node &node = nimble_parser::acquire_node()->at(stmt.at(position));
			stream << node.to_string(verbose);

			if(!node.is_leaf()) {

				for(child_iter = node.children().begin();
						child_iter != node.children().end();
						++child_iter) {
					stream << std::endl;
					nimble_parser::_statement_as_string(stream, stmt, *child_iter, 
						tabs + 1, verbose);
				}
			}

			TRACE_EXIT(TRACE_VERBOSE);
		}

		nimble_node_factory_ptr 
		_nimble_parser::acquire_node(void)
		{
			nimble_node_factory_ptr result = NULL;

			TRACE_ENTRY(TRACE_VERBOSE);

			if(!nimble_node_factory::is_allocated()) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_PARSER_EXCEPTION_STRING(NIMBLE_PARSER_EXCEPTION_COMPONENT_NOT_READY));
				THROW_NIMBLE_PARSER_EXCEPTION(NIMBLE_PARSER_EXCEPTION_COMPONENT_NOT_READY);
			}

			result = nimble_node_factory::acquire();
			if(!result) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_PARSER_EXCEPTION_STRING(NIMBLE_PARSER_EXCEPTION_INVALID_COMPONENT));
				THROW_NIMBLE_PARSER_EXCEPTION(NIMBLE_PARSER_EXCEPTION_INVALID_COMPONENT);
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", result);
			return result;
		}

		void 
		_nimble_parser::clear(void)
		{
			nimble_node_factory_ptr fact = NULL;
			nimble_statement::iterator node_iter;
			std::vector<nimble_statement>::iterator stmt_iter;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			try {

				fact = nimble_parser::acquire_node();
				if(fact && fact->is_initialized()) {

					for(stmt_iter = m_stmt_list.begin(); 
							stmt_iter != m_stmt_list.end(); ++stmt_iter) {

						for(node_iter = stmt_iter->begin(); 
								node_iter != stmt_iter->end(); ++node_iter) {

							if(fact->contains(*node_iter)) {
								fact->decrement_reference(*node_iter);
							}
						}
					}
				}
			} catch(...) { }

			m_stmt_list.clear();
			m_stmt_position = 0;
			TRACE_MESSAGE(TRACE_INFORMATION, "%s", "Parser cleared");

			TRACE_EXIT(TRACE_VERBOSE);
		}

		nimble_uid 
		_nimble_parser::create_token(
			__in nimble_tok_t type
			)
		{
			nimble_uid result;
			nimble_token_factory_ptr fact = NULL;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			fact = nimble_lexer::acquire_token();
			result = fact->generate();
			fact->at(result).type() = type;

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", 
				CHK_STR(nimble_uid::as_string(result, true)));
			return result;
		}

		size_t 
		_nimble_parser::enumerate_statement(
			__inout nimble_statement &stmt,
			__in_opt size_t parent
			)
		{
			nimble_token tok;
			size_t result = parent;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = insert_node(stmt, create_token(TOKEN_STATEMENT), result);

			tok = token();
			if((tok.type() == TOKEN_SYMBOL)
					&& (tok.subtype() == SYMBOL_MODIFIER)) {
				enumerate_statement_assignment(stmt, result);
			} else {
				enumerate_statement_command_0(stmt, result);
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		size_t 
		_nimble_parser::enumerate_statement_argument(
			__inout nimble_statement &stmt,
			__in_opt size_t parent
			)
		{
			size_t result = parent;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = insert_node(stmt, create_token(TOKEN_ARGUMENT), result);

			nimble_token tok = token();
			if((tok.type() != TOKEN_SYMBOL)
					|| (tok.subtype() != SYMBOL_MODIFIER)) {
				TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
					NIMBLE_PARSER_EXCEPTION_STRING(NIMBLE_PARSER_EXCEPTION_EXPECTING_MODIFIER),
					CHK_STR(nimble_lexer::token_exception(0, true)));
				THROW_NIMBLE_PARSER_EXCEPTION_MESSAGE(NIMBLE_PARSER_EXCEPTION_EXPECTING_MODIFIER,
					"%s", CHK_STR(nimble_lexer::token_exception(0, true)));
			}

			if(!has_next_token()) {
				TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
					NIMBLE_PARSER_EXCEPTION_STRING(NIMBLE_PARSER_EXCEPTION_EXPECTING_LITERAL),
					CHK_STR(nimble_lexer::token_exception(0, true)));
				THROW_NIMBLE_PARSER_EXCEPTION_MESSAGE(NIMBLE_PARSER_EXCEPTION_EXPECTING_LITERAL,
					"%s", CHK_STR(nimble_lexer::token_exception(0, true)));
			}

			tok = move_next_token();
			if(tok.type() != TOKEN_LITERAL) {
				TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
					NIMBLE_PARSER_EXCEPTION_STRING(NIMBLE_PARSER_EXCEPTION_EXPECTING_LITERAL),
					CHK_STR(nimble_lexer::token_exception(0, true)));
				THROW_NIMBLE_PARSER_EXCEPTION_MESSAGE(NIMBLE_PARSER_EXCEPTION_EXPECTING_LITERAL,
					"%s", CHK_STR(nimble_lexer::token_exception(0, true)));
			}

			insert_node(stmt, tok, result);

			if(has_next_token()) {
				move_next_token();
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		size_t 
		_nimble_parser::enumerate_statement_assignment(
			__inout nimble_statement &stmt,
			__in_opt size_t parent
			)
		{
			size_t result = parent;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = insert_node(stmt, create_token(TOKEN_ASSIGNMENT), result);
			enumerate_statement_argument(stmt, result);

			nimble_token tok = token();
			if((tok.type() != TOKEN_SYMBOL)
					|| (tok.subtype() != SYMBOL_ASSIGNMENT)) {
				TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
					NIMBLE_PARSER_EXCEPTION_STRING(NIMBLE_PARSER_EXCEPTION_EXPECTING_ASSIGNMENT),
					CHK_STR(nimble_lexer::token_exception(0, true)));
				THROW_NIMBLE_PARSER_EXCEPTION_MESSAGE(NIMBLE_PARSER_EXCEPTION_EXPECTING_ASSIGNMENT,
					"%s", CHK_STR(nimble_lexer::token_exception(0, true)));
			}

			if(!has_next_token()) {
				TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
					NIMBLE_PARSER_EXCEPTION_STRING(NIMBLE_PARSER_EXCEPTION_EXPECTING_STATEMENT),
					CHK_STR(nimble_lexer::token_exception(0, true)));
				THROW_NIMBLE_PARSER_EXCEPTION_MESSAGE(NIMBLE_PARSER_EXCEPTION_EXPECTING_STATEMENT,
					"%s", CHK_STR(nimble_lexer::token_exception(0, true)));
			}

			tok = move_next_token();
			if((tok.type() == TOKEN_SYMBOL)
					&& (tok.subtype() == SYMBOL_MODIFIER)) {
				enumerate_statement_assignment(stmt, result);
			} else {
				enumerate_statement_command_0(stmt, result);
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		size_t 
		_nimble_parser::enumerate_statement_call(
			__inout nimble_statement &stmt,
			__in_opt size_t parent
			)
		{
			nimble_token tok;
			size_t result = parent;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = insert_node(stmt, create_token(TOKEN_CALL), result);

			tok = token();
			if(tok.type() != TOKEN_LITERAL) {
				TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
					NIMBLE_PARSER_EXCEPTION_STRING(NIMBLE_PARSER_EXCEPTION_EXPECTING_LITERAL),
					CHK_STR(nimble_lexer::token_exception(0, true)));
				THROW_NIMBLE_PARSER_EXCEPTION_MESSAGE(NIMBLE_PARSER_EXCEPTION_EXPECTING_LITERAL,
					"%s", CHK_STR(nimble_lexer::token_exception(0, true)));
			}

			insert_node(stmt, tok, result);

			if(has_next_token()) {
				tok = move_next_token();

				for(;;) {

					if((tok.type() == TOKEN_SYMBOL)
							&& (tok.subtype() == SYMBOL_MODIFIER)) {
						enumerate_statement_argument(stmt, result);
						tok = token();
					} else if(tok.type() == TOKEN_LITERAL) {
						insert_node(stmt, tok, result);

						if(!has_next_token()) {
							break;
						}

						tok = move_next_token();
					} else {
						break;
					}
				}
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		size_t 
		_nimble_parser::enumerate_statement_call_list(
			__inout nimble_statement &stmt,
			__in_opt size_t parent
			)
		{
			nimble_token tok;
			size_t result = parent;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = insert_node(stmt, create_token(TOKEN_CALL_LIST), result);

			enumerate_statement_call(stmt, result);

			tok = token();
			for(;;) {

				if((tok.type() != TOKEN_SYMBOL)
						|| (tok.subtype() != SYMBOL_SEPERATOR)) {
					break;
				}

				if(!has_next_token()) {
					TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
						NIMBLE_PARSER_EXCEPTION_STRING(NIMBLE_PARSER_EXCEPTION_EXPECTING_COMMAND),
						CHK_STR(nimble_lexer::token_exception(0, true)));
					THROW_NIMBLE_PARSER_EXCEPTION_MESSAGE(NIMBLE_PARSER_EXCEPTION_EXPECTING_COMMAND,
						"%s", CHK_STR(nimble_lexer::token_exception(0, true)));
				}

				move_next_token();
				enumerate_statement_call(stmt, result);
				tok = token();
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		size_t 
		_nimble_parser::enumerate_statement_command_0(
			__inout nimble_statement &stmt,
			__in_opt size_t parent
			)
		{
			nimble_token tok;
			size_t result = parent;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = insert_node(stmt, create_token(TOKEN_COMMAND), result);
			enumerate_statement_command_1(stmt, result);

			tok = token();
			while((tok.type() == TOKEN_SYMBOL)
					&& (tok.subtype() == SYMBOL_REDIRECT_IN)) {
				enumerate_statement_command_0_tail(stmt, result);
				tok = token();
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		size_t 
		_nimble_parser::enumerate_statement_command_0_tail(
			__inout nimble_statement &stmt,
			__in_opt size_t parent
			)
		{
			nimble_token tok;
			size_t result = parent;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			tok = token();
			if((tok.type() != TOKEN_SYMBOL)
					|| (tok.subtype() != SYMBOL_REDIRECT_IN)) {
				TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
					NIMBLE_PARSER_EXCEPTION_STRING(NIMBLE_PARSER_EXCEPTION_EXPECTING_REDIRECT_IN),
					CHK_STR(nimble_lexer::token_exception(0, true)));
				THROW_NIMBLE_PARSER_EXCEPTION_MESSAGE(NIMBLE_PARSER_EXCEPTION_EXPECTING_REDIRECT_IN,
					"%s", CHK_STR(nimble_lexer::token_exception(0, true)));
			}

			result = insert_node(stmt, tok, result);

			if(!has_next_token()) {
				TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
					NIMBLE_PARSER_EXCEPTION_STRING(NIMBLE_PARSER_EXCEPTION_EXPECTING_COMMAND),
					CHK_STR(nimble_lexer::token_exception(0, true)));
				THROW_NIMBLE_PARSER_EXCEPTION_MESSAGE(NIMBLE_PARSER_EXCEPTION_EXPECTING_COMMAND,
					"%s", CHK_STR(nimble_lexer::token_exception(0, true)));
			}

			move_next_token();
			enumerate_statement_command_1(stmt, result);

			tok = token();
			while((tok.type() == TOKEN_SYMBOL)
					&& (tok.subtype() == SYMBOL_REDIRECT_IN)) {
				enumerate_statement_command_0_tail(stmt, result);
				tok = token();
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		size_t 
		_nimble_parser::enumerate_statement_command_1(
			__inout nimble_statement &stmt,
			__in_opt size_t parent
			)
		{
			nimble_token tok;
			size_t result = parent;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			enumerate_statement_command_2(stmt, result);

			tok = token();
			while((tok.type() == TOKEN_SYMBOL)
					&& ((tok.subtype() == SYMBOL_REDIRECT_OUT)
					|| (tok.subtype() == SYMBOL_REDIRECT_OUT_APPEND)
					|| (tok.subtype() == SYMBOL_REDIRECT_OUT_ERR)
					|| (tok.subtype() == SYMBOL_REDIRECT_OUT_ERR_APPEND)
					|| (tok.subtype() == SYMBOL_REDIRECT_OUT_ERR_OVERWRITE)
					|| (tok.subtype() == SYMBOL_REDIRECT_OUT_OVERWRITE))) {
				enumerate_statement_command_1_tail(stmt, result);
				tok = token();
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		size_t 
		_nimble_parser::enumerate_statement_command_1_tail(
			__inout nimble_statement &stmt,
			__in_opt size_t parent
			)
		{
			nimble_token tok;
			size_t result = parent;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			tok = token();
			if((tok.type() != TOKEN_SYMBOL)
					|| ((tok.subtype() != SYMBOL_REDIRECT_OUT)
					&& (tok.subtype() != SYMBOL_REDIRECT_OUT_APPEND)
					&& (tok.subtype() != SYMBOL_REDIRECT_OUT_ERR)
					&& (tok.subtype() != SYMBOL_REDIRECT_OUT_ERR_APPEND)
					&& (tok.subtype() != SYMBOL_REDIRECT_OUT_ERR_OVERWRITE)
					&& (tok.subtype() != SYMBOL_REDIRECT_OUT_OVERWRITE))) {
				TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
					NIMBLE_PARSER_EXCEPTION_STRING(NIMBLE_PARSER_EXCEPTION_EXPECTING_REDIRECT_OUT),
					CHK_STR(nimble_lexer::token_exception(0, true)));
				THROW_NIMBLE_PARSER_EXCEPTION_MESSAGE(NIMBLE_PARSER_EXCEPTION_EXPECTING_REDIRECT_OUT,
					"%s", CHK_STR(nimble_lexer::token_exception(0, true)));
			}

			result = insert_node(stmt, tok, result);

			if(!has_next_token()) {
				TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
					NIMBLE_PARSER_EXCEPTION_STRING(NIMBLE_PARSER_EXCEPTION_EXPECTING_COMMAND),
					CHK_STR(nimble_lexer::token_exception(0, true)));
				THROW_NIMBLE_PARSER_EXCEPTION_MESSAGE(NIMBLE_PARSER_EXCEPTION_EXPECTING_COMMAND,
					"%s", CHK_STR(nimble_lexer::token_exception(0, true)));
			}

			move_next_token();
			enumerate_statement_command_2(stmt, result);

			tok = token();
			while((tok.type() == TOKEN_SYMBOL)
					&& ((tok.subtype() == SYMBOL_REDIRECT_OUT)
					|| (tok.subtype() == SYMBOL_REDIRECT_OUT_APPEND)
					|| (tok.subtype() == SYMBOL_REDIRECT_OUT_ERR)
					|| (tok.subtype() == SYMBOL_REDIRECT_OUT_ERR_APPEND)
					|| (tok.subtype() == SYMBOL_REDIRECT_OUT_ERR_OVERWRITE)
					|| (tok.subtype() == SYMBOL_REDIRECT_OUT_OVERWRITE))) {
				enumerate_statement_command_1_tail(stmt, result);
				tok = token();
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		size_t 
		_nimble_parser::enumerate_statement_command_2(
			__inout nimble_statement &stmt,
			__in_opt size_t parent
			)
		{
			nimble_token tok;
			size_t result = parent;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			enumerate_statement_command_3(stmt, result);

			tok = token();
			while((tok.type() == TOKEN_SYMBOL)
					&& (tok.subtype() == SYMBOL_PIPE)) {
				enumerate_statement_command_2_tail(stmt, result);
				tok = token();
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		size_t 
		_nimble_parser::enumerate_statement_command_2_tail(
			__inout nimble_statement &stmt,
			__in_opt size_t parent
			)
		{
			nimble_token tok;
			size_t result = parent;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			tok = token();
			if((tok.type() != TOKEN_SYMBOL)
					|| (tok.subtype() != SYMBOL_PIPE)) {
				TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
					NIMBLE_PARSER_EXCEPTION_STRING(NIMBLE_PARSER_EXCEPTION_EXPECTING_REDIRECT_IN),
					CHK_STR(nimble_lexer::token_exception(0, true)));
				THROW_NIMBLE_PARSER_EXCEPTION_MESSAGE(NIMBLE_PARSER_EXCEPTION_EXPECTING_REDIRECT_IN,
					"%s", CHK_STR(nimble_lexer::token_exception(0, true)));
			}

			result = insert_node(stmt, tok, result);

			if(!has_next_token()) {
				TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
					NIMBLE_PARSER_EXCEPTION_STRING(NIMBLE_PARSER_EXCEPTION_EXPECTING_COMMAND),
					CHK_STR(nimble_lexer::token_exception(0, true)));
				THROW_NIMBLE_PARSER_EXCEPTION_MESSAGE(NIMBLE_PARSER_EXCEPTION_EXPECTING_COMMAND,
					"%s", CHK_STR(nimble_lexer::token_exception(0, true)));
			}

			move_next_token();
			enumerate_statement_command_3(stmt, result);

			tok = token();
			while((tok.type() == TOKEN_SYMBOL)
					&& (tok.subtype() == SYMBOL_PIPE)) {
				enumerate_statement_command_2_tail(stmt, result);
				tok = token();
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		size_t 
		_nimble_parser::enumerate_statement_command_3(
			__inout nimble_statement &stmt,
			__in_opt size_t parent
			)
		{
			nimble_token tok;
			size_t result = parent;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			tok = token();
			switch(tok.type()) {
				case TOKEN_LITERAL:
					enumerate_statement_call_list(stmt, result);
					break;
				case TOKEN_SYMBOL:

					switch(tok.subtype()) {
						case SYMBOL_MODIFIER:
							enumerate_statement_argument(stmt, result);
							break;
						case SYMBOL_OPEN_PARENTHESIS:

							if(!has_next_token()) {
								TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
									NIMBLE_PARSER_EXCEPTION_STRING(
									NIMBLE_PARSER_EXCEPTION_EXPECTING_COMMAND),
									CHK_STR(nimble_lexer::token_exception(0, true)));
								THROW_NIMBLE_PARSER_EXCEPTION_MESSAGE(
									NIMBLE_PARSER_EXCEPTION_EXPECTING_COMMAND,
									"%s", CHK_STR(nimble_lexer::token_exception(0, true)));
							}

							move_next_token();
							enumerate_statement_command_0(stmt, result);
							tok = token();

							if((tok.type() != TOKEN_SYMBOL)
									|| (tok.subtype() != SYMBOL_CLOSE_PARENTHESIS)) {
								TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
									NIMBLE_PARSER_EXCEPTION_STRING(
									NIMBLE_PARSER_EXCEPTION_EXPECTING_CLOSING_PARETHESIS),
									CHK_STR(nimble_lexer::token_exception(0, true)));
								THROW_NIMBLE_PARSER_EXCEPTION_MESSAGE(
									NIMBLE_PARSER_EXCEPTION_EXPECTING_CLOSING_PARETHESIS,
									"%s", CHK_STR(nimble_lexer::token_exception(0, true)));
							}

							if(has_next_token()) {
								move_next_token();
							}
							break;
						case SYMBOL_SEPERATOR:

							if(has_next_token()) {
								move_next_token();
							}
							break;
						default:
							TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", 
								NIMBLE_PARSER_EXCEPTION_STRING(
								NIMBLE_PARSER_EXCEPTION_EXPECTING_COMMAND),
								CHK_STR(nimble_lexer::token_exception(0, true)));
							THROW_NIMBLE_PARSER_EXCEPTION_MESSAGE(
								NIMBLE_PARSER_EXCEPTION_EXPECTING_COMMAND,
								"%s", CHK_STR(nimble_lexer::token_exception(0, true)));
					}
					break;
				default:
					TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", NIMBLE_PARSER_EXCEPTION_STRING(
						NIMBLE_PARSER_EXCEPTION_EXPECTING_COMMAND),
						CHK_STR(nimble_lexer::token_exception(0, true)));
					THROW_NIMBLE_PARSER_EXCEPTION_MESSAGE(NIMBLE_PARSER_EXCEPTION_EXPECTING_COMMAND,
						"%s", CHK_STR(nimble_lexer::token_exception(0, true)));
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
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
			TRACE_MESSAGE(TRACE_INFORMATION, "Parser discovered %lu statements", 
				result);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		bool 
		_nimble_parser::has_next_statement(void)
		{
			bool result;
			nimble_node_factory_ptr node_fact = NULL;
			nimble_token_factory_ptr tok_fact = NULL;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			node_fact = nimble_parser::acquire_node();
			tok_fact = nimble_lexer::acquire_token();
			result = (node_token(statement().front()).type() != TOKEN_END);

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

		size_t 
		_nimble_parser::insert_node(
			__inout nimble_statement &stmt,
			__in const nimble_uid &token,
			__in_opt size_t parent
			)
		{
			size_t result;
			nimble_uid uid;
			nimble_node_factory_ptr fact = NULL;

			TRACE_ENTRY(TRACE_VERBOSE);

			fact = nimble_parser::acquire_node();
			uid = fact->generate();
			nimble_node &node = fact->at(uid);
			node.set(token);
			node.parent() = parent;
			result = stmt.size();
			stmt.push_back(uid);

			if(parent != PAR_INVALID) {
				fact->at(stmt.at(parent)).children().push_back(result);
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		void 
		_nimble_parser::insert_statement(
			__in const nimble_statement &stmt
			)
		{
			size_t position;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			position = m_stmt_position + 1;
			if(position < m_stmt_list.size()) {
				m_stmt_list.insert(m_stmt_list.begin() + position, stmt);
			} else {
				m_stmt_list.push_back(stmt);
			}

			TRACE_EXIT(TRACE_VERBOSE);
		}

		nimble_statement &
		_nimble_parser::move_next_statement(void)
		{
			nimble_statement stmt_new;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!has_next_statement()) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, pos. %lu", 
					NIMBLE_PARSER_EXCEPTION_STRING(NIMBLE_PARSER_EXCEPTION_NO_NEXT_STATEMENT),
					m_stmt_position);
				THROW_NIMBLE_PARSER_EXCEPTION_MESSAGE(NIMBLE_PARSER_EXCEPTION_NO_NEXT_STATEMENT,
					"%lu", m_stmt_position);
			}

			if(token().type() == TOKEN_BEGIN) {
				move_next_token();
			}

			if(has_next_token()
					&& (m_stmt_position <= (m_stmt_list.size() - SENTINEL_PARSER))) {
				enumerate_statement(stmt_new);
				insert_statement(stmt_new);
			}

			++m_stmt_position;
			nimble_statement &stmt = statement();
			TRACE_MESSAGE(TRACE_INFORMATION, "Moved to next statement[%lu] -> %s", m_stmt_position, 
				CHK_STR(nimble_parser::statement_as_string(stmt, true)));

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

			--m_stmt_position;
			nimble_statement &stmt = statement();
			TRACE_MESSAGE(TRACE_INFORMATION, "Moved to previous statement[%lu] -> %s", m_stmt_position, 
				CHK_STR(nimble_parser::statement_as_string(stmt, true)));

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", 
				CHK_STR(nimble_parser::statement_as_string(stmt, true)));
			return stmt;
		}

		nimble_token &
		_nimble_parser::node_token(
			__in const nimble_uid &uid
			)
		{
			nimble_node_factory_ptr node_fact = NULL;
			nimble_token_factory_ptr tok_fact = NULL;

			TRACE_ENTRY(TRACE_VERBOSE);

			node_fact = nimble_parser::acquire_node();
			tok_fact = nimble_lexer::acquire_token();
			nimble_token &tok = tok_fact->at(node_fact->at(uid).token());

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "%s", CHK_STR(tok.to_string(true)));
			return tok;
		}

		void 
		_nimble_parser::reset(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			m_stmt_position = 0;
			TRACE_MESSAGE(TRACE_INFORMATION, "%s", "Parser reset");

			TRACE_EXIT(TRACE_VERBOSE);
		}

		void 
		_nimble_parser::set(
			__in_opt const std::string &input,
			__in_opt bool is_file
			)
		{
			nimble_statement stmt_beg, stmt_end;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			nimble_parser::clear();
			nimble_lexer::set(input, is_file);
			insert_node(stmt_beg, token_begin());
			insert_node(stmt_end, token_end());
			insert_statement(stmt_beg);
			insert_statement(stmt_end);

			nimble_parser::reset();
			TRACE_MESSAGE(TRACE_INFORMATION, "Parser set input -> \'%s\', file -> 0x%x", 
				CHK_STR(input), is_file);

			TRACE_EXIT(TRACE_VERBOSE);
		}

		void 
		_nimble_parser::set(
			__in const _nimble_parser &other
			)
		{
			nimble_node_factory_ptr fact = NULL;
			nimble_statement::iterator node_iter;
			std::vector<nimble_statement>::iterator stmt_iter;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			nimble_parser::clear();
			nimble_lexer::operator=(other);
			m_stmt_list = other.m_stmt_list;
			m_stmt_position = other.m_stmt_position;

			try {

				fact = nimble_parser::acquire_node();
				if(fact && fact->is_initialized()) {

					for(stmt_iter = m_stmt_list.begin(); 
							stmt_iter != m_stmt_list.end(); ++stmt_iter) {

						for(node_iter = stmt_iter->begin(); 
								node_iter != stmt_iter->end(); ++node_iter) {

							if(fact->contains(*node_iter)) {
								fact->increment_reference(*node_iter);
							}
						}
					}
				}
			} catch(...) { }

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
			__in const nimble_statement &stmt,
			__in_opt bool verbose
			)
		{
			std::stringstream result;

			TRACE_ENTRY(TRACE_VERBOSE);

			nimble_parser::_statement_as_string(result, stmt, 0, 0, verbose);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result.str()));
			return CHK_STR(result.str());
		}

		nimble_statement &
		_nimble_parser::statement_begin(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(m_stmt_list.empty()) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_PARSER_EXCEPTION_STRING(NIMBLE_PARSER_EXCEPTION_INVALID_STATEMENT_POSITION));
				THROW_NIMBLE_PARSER_EXCEPTION(NIMBLE_PARSER_EXCEPTION_INVALID_STATEMENT_POSITION);
			}

			nimble_statement &stmt = m_stmt_list.front();

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", 
				CHK_STR(nimble_parser::statement_as_string(stmt, true)));
			return stmt;
		}

		nimble_statement &
		_nimble_parser::statement_end(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(m_stmt_list.empty()) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_PARSER_EXCEPTION_STRING(NIMBLE_PARSER_EXCEPTION_INVALID_STATEMENT_POSITION));
				THROW_NIMBLE_PARSER_EXCEPTION(NIMBLE_PARSER_EXCEPTION_INVALID_STATEMENT_POSITION);
			}

			nimble_statement &stmt = m_stmt_list.back();

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", 
				CHK_STR(nimble_parser::statement_as_string(stmt, true)));
			return stmt;
		}

		std::string 
		_nimble_parser::statement_exception(
			__in_opt size_t tabs,
			__in_opt bool verbose
			)
		{
			std::stringstream result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result << nimble_lexer::token_exception(tabs, verbose);

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
			std::stringstream result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(verbose) {
				result << "(" << m_stmt_position << "/" << (m_stmt_list.size() - 1) << ") ";
			}

			if(m_stmt_position >= m_stmt_list.size()) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, pos. %lu", 
					NIMBLE_PARSER_EXCEPTION_STRING(NIMBLE_PARSER_EXCEPTION_INVALID_STATEMENT_POSITION),
					m_tok_position);
				THROW_NIMBLE_PARSER_EXCEPTION_MESSAGE(NIMBLE_PARSER_EXCEPTION_INVALID_STATEMENT_POSITION,
					"%lu", m_stmt_position);
			}

			result << nimble_parser::statement_as_string(statement(), verbose);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result.str()));
			return CHK_STR(result.str());
		}
	}
}
