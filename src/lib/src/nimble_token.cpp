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
#include "../include/nimble_token_type.h"

namespace NIMBLE {

	namespace COMPONENT {

		_nimble_token::_nimble_token(void) :
			m_column(0),
			m_position(0),
			m_row(0),
			m_subtype(TOKSUB_INVALID),
			m_type(TOK_INVALID),
			m_value(0.0)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_token::_nimble_token(
			__in const _nimble_token &other
			) :
				nimble_uid_class(other),
				m_column(other.m_column),
				m_path(other.m_path),
				m_position(other.m_position),
				m_row(other.m_row),
				m_subtype(other.m_subtype),
				m_text(other.m_text),
				m_type(other.m_type),
				m_value(other.m_value)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_token::~_nimble_token(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_token &
		_nimble_token::operator=(
			__in const _nimble_token &other
			)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(this != &other) {
				nimble_uid_class::operator=(other);
				m_column = other.m_column;
				m_path = other.m_path;
				m_position = other.m_position;
				m_row = other.m_row;
				m_subtype = other.m_subtype;
				m_text = other.m_text;
				m_type = other.m_type;
				m_value = other.m_value;
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", this);
			return *this;
		}

		std::string 
		_nimble_token::as_string(
			__in const _nimble_token &tok,
			__in_opt bool verbose
			)
		{
			std::stringstream result;

			TRACE_ENTRY(TRACE_VERBOSE);

			if(verbose) {
				result << nimble_uid::as_string(tok.m_uid, true) << " ";
			}

			result << "[" << ((tok.m_type == TOK_INVALID) ? INVALID : 
				nimble_language::type_as_string(tok.m_type));

			if(tok.m_subtype != TOKSUB_INVALID) {
				result << ", " << nimble_language::subtype_as_string(
					tok.m_type, tok.m_subtype);
			}

			result << "]";

			if((tok.m_type != TOKEN_BEGIN) 
					&& (tok.m_type != TOKEN_END)) {

				if(!tok.m_text.empty()
						&& (tok.m_type == TOKEN_LITERAL)) {
					result << " \'" << tok.m_text << "\'";
				}

				if(tok.m_type == TOKEN_IMMEDIATE) {
					result << " = " << tok.m_value << "(10)";
				}

				result << " (";

				if(!tok.m_path.empty()) {
					result << tok.m_path << ":";
				}

				result << tok.m_row;

				if(verbose) {
					result << " (" << tok.m_position << ":" << tok.m_row 
						<< ":" << tok.m_column << ")";
				}

				result << ")";
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result.str()));
			return CHK_STR(result.str());
		}

		void 
		_nimble_token::clear(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			m_column = 0;
			m_path.clear();
			m_position = 0;
			m_row = 0;
			m_subtype = TOKSUB_INVALID;
			m_text.clear();
			m_type = TOK_INVALID;
			m_value = 0.0;

			TRACE_EXIT(TRACE_VERBOSE);
		}

		size_t &
		_nimble_token::column(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);
			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", m_column);
			return m_column;
		}

		nimble_token_meta 
		_nimble_token::meta(
			__in_opt size_t column,
			__in_opt size_t row
			)
		{
			nimble_token_meta result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = nimble_token_meta(m_text, m_path, column, m_column, row, m_row);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result.to_string(true)));
			return result;
		}

		std::string &
		_nimble_token::path(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);
			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(m_path));
			return m_path;
		}

		size_t &
		_nimble_token::position(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);
			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", m_position);
			return m_position;
		}

		size_t &
		_nimble_token::row(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);
			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", m_row);
			return m_row;
		}

		toksub_t &
		_nimble_token::subtype(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);
			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s (0x%x)", 
				CHK_STR(nimble_language::subtype_as_string(m_type, m_subtype)), 
				m_subtype);
			return m_subtype;
		}

		std::string &
		_nimble_token::text(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);
			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(m_text));
			return m_text;
		}

		std::string 
		_nimble_token::to_string(
			__in_opt bool verbose
			)
		{
			std::string result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = nimble_token::as_string(*this, verbose);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result));
			return CHK_STR(result);
		}

		tok_t &
		_nimble_token::type(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);
			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s (0x%x)", 
				CHK_STR(nimble_language::type_as_string(m_type)), m_type);
			return m_type;
		}

		double &
		_nimble_token::value(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);
			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %.8f", m_value);
			return m_value;
		}

		nimble_token_factory_ptr nimble_token_factory::m_instance = NULL;

		_nimble_token_factory::_nimble_token_factory(void) :
			m_initialized(false)
		{
			TRACE_ENTRY(TRACE_VERBOSE);

			std::atexit(nimble_token_factory::_delete);

			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_token_factory::~_nimble_token_factory(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);

			if(m_initialized) {
				uninitialize();
			}

			TRACE_EXIT(TRACE_VERBOSE);
		}

		void 
		_nimble_token_factory::_delete(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);

			if(nimble_token_factory::m_instance) {
				TRACE_MESSAGE(TRACE_INFORMATION, "Deleted token component instance, ptr. 0x%p", 
					nimble_token_factory::m_instance);
				delete nimble_token_factory::m_instance;
				nimble_token_factory::m_instance = NULL;
			}

			TRACE_EXIT(TRACE_VERBOSE);
		}

		nimble_token_factory_ptr 
		_nimble_token_factory::acquire(void)
		{
			nimble_token_factory_ptr result = NULL;

			TRACE_ENTRY(TRACE_VERBOSE);

			if(!nimble_token_factory::m_instance) {

				nimble_token_factory::m_instance = new nimble_token_factory;
				if(!nimble_token_factory::m_instance) {
					TRACE_MESSAGE(TRACE_ERROR, "%s", 
						NIMBLE_TOKEN_EXCEPTION_STRING(NIMBLE_TOKEN_EXCEPTION_ALLOCATED));
					THROW_NIMBLE_TOKEN_EXCEPTION(NIMBLE_TOKEN_EXCEPTION_ALLOCATED);
				} else {
					TRACE_MESSAGE(TRACE_INFORMATION, "Allocated token component instance, ptr. 0x%p", 
						nimble_token_factory::m_instance);
				}
			}

			result = nimble_token_factory::m_instance;

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", result);
			return result;
		}

		nimble_token &
		_nimble_token_factory::at(
			__in const nimble_uid &uid
			)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_TOKEN_EXCEPTION_STRING(NIMBLE_TOKEN_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_TOKEN_EXCEPTION(NIMBLE_TOKEN_EXCEPTION_UNINITIALIZED);
			}

			nimble_token &tok = find(uid)->second.first;

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(tok.to_string(true)));
			return tok;
		}

		bool 
		_nimble_token_factory::contains(
			__in const nimble_uid &uid
			)
		{
			bool result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_TOKEN_EXCEPTION_STRING(NIMBLE_TOKEN_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_TOKEN_EXCEPTION(NIMBLE_TOKEN_EXCEPTION_UNINITIALIZED);
			}

			result = (m_map.find(uid) != m_map.end());

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", result);
			return result;
		}

		size_t 
		_nimble_token_factory::decrement_reference(
			__in const nimble_uid &uid
			)
		{
			size_t result;
			std::map<nimble_uid, std::pair<nimble_token, size_t>>::iterator iter;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_TOKEN_EXCEPTION_STRING(NIMBLE_TOKEN_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_TOKEN_EXCEPTION(NIMBLE_TOKEN_EXCEPTION_UNINITIALIZED);
			}

			iter = find(uid);

			result = --iter->second.second;
			if(result < REF_INITIAL) {
				TRACE_MESSAGE(TRACE_INFORMATION, "Removing token: %s", 
					CHK_STR(nimble_token::as_string(iter->second.first, true)));
				m_map.erase(iter);
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		std::map<nimble_uid, std::pair<nimble_token, size_t>>::iterator 
		_nimble_token_factory::find(
			__in const nimble_uid &uid
			)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			std::map<nimble_uid, std::pair<nimble_token, size_t>>::iterator result;

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_TOKEN_EXCEPTION_STRING(NIMBLE_TOKEN_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_TOKEN_EXCEPTION(NIMBLE_TOKEN_EXCEPTION_UNINITIALIZED);
			}

			result = m_map.find(uid);
			if(result == m_map.end()) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, %s", 
					NIMBLE_TOKEN_EXCEPTION_STRING(NIMBLE_TOKEN_EXCEPTION_NOT_FOUND),
					CHK_STR(nimble_uid::as_string(uid)));
				THROW_NIMBLE_TOKEN_EXCEPTION_MESSAGE(NIMBLE_TOKEN_EXCEPTION_NOT_FOUND,
					"%s", CHK_STR(nimble_uid::as_string(uid)));
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", result);
			return result;
		}

		nimble_uid 
		_nimble_token_factory::generate(void)
		{
			nimble_token tok;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_TOKEN_EXCEPTION_STRING(NIMBLE_TOKEN_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_TOKEN_EXCEPTION(NIMBLE_TOKEN_EXCEPTION_UNINITIALIZED);
			}

			TRACE_MESSAGE(TRACE_INFORMATION, "Generating new token: %s", 
				CHK_STR(tok.to_string(true)));
			m_map.insert(std::pair<nimble_uid, std::pair<nimble_token, size_t>>(tok.uid(), 
				std::pair<nimble_token, size_t>(tok, REF_INITIAL)));

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(tok.to_string(true)));
			return tok.uid();
		}

		size_t 
		_nimble_token_factory::increment_reference(
			__in const nimble_uid &uid
			)
		{
			size_t result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_TOKEN_EXCEPTION_STRING(NIMBLE_TOKEN_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_TOKEN_EXCEPTION(NIMBLE_TOKEN_EXCEPTION_UNINITIALIZED);
			}

			result = ++find(uid)->second.second;

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		void 
		_nimble_token_factory::initialize(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_TOKEN_EXCEPTION_STRING(NIMBLE_TOKEN_EXCEPTION_INITIALIZED));
				THROW_NIMBLE_TOKEN_EXCEPTION(NIMBLE_TOKEN_EXCEPTION_INITIALIZED);
			}

			m_initialized = true;
			m_map.clear();
			TRACE_MESSAGE(TRACE_INFORMATION, "%s", "Token component instance initialized");

			TRACE_EXIT(TRACE_VERBOSE);
		}

		bool 
		_nimble_token_factory::is_allocated(void)
		{
			bool result;

			TRACE_ENTRY(TRACE_VERBOSE);

			result = (nimble_token_factory::m_instance != NULL);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", result);
			return result;
		}

		bool 
		_nimble_token_factory::is_initialized(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);
			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", m_initialized);
			return m_initialized;
		}

		size_t 
		_nimble_token_factory::reference_count(
			__in const nimble_uid &uid
			)
		{
			size_t result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_TOKEN_EXCEPTION_STRING(NIMBLE_TOKEN_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_TOKEN_EXCEPTION(NIMBLE_TOKEN_EXCEPTION_UNINITIALIZED);
			}

			result = find(uid)->second.second;

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		size_t 
		_nimble_token_factory::size(void)
		{
			size_t result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_TOKEN_EXCEPTION_STRING(NIMBLE_TOKEN_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_TOKEN_EXCEPTION(NIMBLE_TOKEN_EXCEPTION_UNINITIALIZED);
			}

			result = m_map.size();

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		std::string 
		_nimble_token_factory::to_string(
			__in_opt bool verbose
			)
		{
			std::stringstream result;
			std::map<nimble_uid, std::pair<nimble_token, size_t>>::iterator iter;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result << "(" << (m_initialized ? "INIT" : "UNINIT") << ") " << NIMBLE_TOKEN_HEADER 
				<< "[" << m_map.size() << "]";

			if(verbose) {
				result << " (" << VAL_AS_HEX(nimble_token_factory_ptr, this) << ")";

				for(iter = m_map.begin(); iter != m_map.end(); ++iter) {
					result << std::endl << "--- " << nimble_token::as_string(iter->second.first, true)
						<< ", ref. " << iter->second.second;
				}
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result.str()));
			return CHK_STR(result.str());
		}

		void 
		_nimble_token_factory::uninitialize(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_TOKEN_EXCEPTION_STRING(NIMBLE_TOKEN_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_TOKEN_EXCEPTION(NIMBLE_TOKEN_EXCEPTION_UNINITIALIZED);
			}

			m_map.clear();
			m_initialized = false;
			TRACE_MESSAGE(TRACE_INFORMATION, "%s", "Token component instance uninitialized");

			TRACE_EXIT(TRACE_VERBOSE);
		}
	}
}
