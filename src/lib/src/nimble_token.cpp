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
			return;
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
			return;
		}

		_nimble_token::~_nimble_token(void)
		{
			return;
		}

		_nimble_token &
		_nimble_token::operator=(
			__in const _nimble_token &other
			)
		{
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

			return *this;
		}

		std::string 
		_nimble_token::as_string(
			__in const _nimble_token &tok,
			__in_opt bool verbose
			)
		{
			std::stringstream result;

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

				if(!tok.m_text.empty()) {
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

			return CHK_STR(result.str());
		}

		void 
		_nimble_token::clear(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			m_column = 0;
			m_path.clear();
			m_position = 0;
			m_row = 0;
			m_subtype = TOKSUB_INVALID;
			m_text.clear();
			m_type = TOK_INVALID;
			m_value = 0.0;
		}

		size_t &
		_nimble_token::column(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return m_column;
		}

		nimble_token_meta 
		_nimble_token::meta(
			__in_opt size_t column,
			__in_opt size_t row
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return nimble_token_meta(m_text, m_path, column, m_column, row, m_row);
		}

		std::string &
		_nimble_token::path(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return m_path;
		}

		size_t &
		_nimble_token::position(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return m_position;
		}

		size_t &
		_nimble_token::row(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return m_row;
		}

		toksub_t &
		_nimble_token::subtype(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return m_subtype;
		}

		std::string &
		_nimble_token::text(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return m_text;
		}

		std::string 
		_nimble_token::to_string(
			__in_opt bool verbose
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return nimble_token::as_string(*this, verbose);
		}

		tok_t &
		_nimble_token::type(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return m_type;
		}

		double &
		_nimble_token::value(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return m_value;
		}

		nimble_token_factory_ptr nimble_token_factory::m_instance = NULL;

		_nimble_token_factory::_nimble_token_factory(void) :
			m_initialized(false)
		{
			std::atexit(nimble_token_factory::_delete);
		}

		_nimble_token_factory::~_nimble_token_factory(void)
		{

			if(m_initialized) {
				uninitialize();
			}
		}

		void 
		_nimble_token_factory::_delete(void)
		{

			if(nimble_token_factory::m_instance) {
				delete nimble_token_factory::m_instance;
				nimble_token_factory::m_instance = NULL;
			}
		}

		nimble_token_factory_ptr 
		_nimble_token_factory::acquire(void)
		{

			if(!nimble_token_factory::m_instance) {

				nimble_token_factory::m_instance = new nimble_token_factory;
				if(!nimble_token_factory::m_instance) {
					THROW_NIMBLE_TOKEN_EXCEPTION(NIMBLE_TOKEN_EXCEPTION_ALLOCATED);
				}
			}

			return nimble_token_factory::m_instance;
		}

		nimble_token &
		_nimble_token_factory::at(
			__in const nimble_uid &uid
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_TOKEN_EXCEPTION(NIMBLE_TOKEN_EXCEPTION_UNINITIALIZED);
			}

			return find(uid)->second.first;
		}

		bool 
		_nimble_token_factory::contains(
			__in const nimble_uid &uid
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_TOKEN_EXCEPTION(NIMBLE_TOKEN_EXCEPTION_UNINITIALIZED);
			}

			return (m_map.find(uid) != m_map.end());
		}

		size_t 
		_nimble_token_factory::decrement_reference(
			__in const nimble_uid &uid
			)
		{
			size_t result;
			std::map<nimble_uid, std::pair<nimble_token, size_t>>::iterator iter;

			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_TOKEN_EXCEPTION(NIMBLE_TOKEN_EXCEPTION_UNINITIALIZED);
			}

			iter = find(uid);

			result = --iter->second.second;
			if(result < REF_INITIAL) {
				m_map.erase(iter);
			}

			return result;
		}

		std::map<nimble_uid, std::pair<nimble_token, size_t>>::iterator 
		_nimble_token_factory::find(
			__in const nimble_uid &uid
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			std::map<nimble_uid, std::pair<nimble_token, size_t>>::iterator result;

			if(!m_initialized) {
				THROW_NIMBLE_TOKEN_EXCEPTION(NIMBLE_TOKEN_EXCEPTION_UNINITIALIZED);
			}

			result = m_map.find(uid);
			if(result == m_map.end()) {
				THROW_NIMBLE_TOKEN_EXCEPTION_MESSAGE(NIMBLE_TOKEN_EXCEPTION_NOT_FOUND,
					"%s", CHK_STR(nimble_uid::as_string(uid)));
			}

			return result;
		}

		nimble_uid 
		_nimble_token_factory::generate(void)
		{
			nimble_token tok;

			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_TOKEN_EXCEPTION(NIMBLE_TOKEN_EXCEPTION_UNINITIALIZED);
			}

			m_map.insert(std::pair<nimble_uid, std::pair<nimble_token, size_t>>(tok.uid(), 
				std::pair<nimble_token, size_t>(tok, REF_INITIAL)));

			return tok.uid();
		}

		size_t 
		_nimble_token_factory::increment_reference(
			__in const nimble_uid &uid
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_TOKEN_EXCEPTION(NIMBLE_TOKEN_EXCEPTION_UNINITIALIZED);
			}

			return ++find(uid)->second.second;
		}

		void 
		_nimble_token_factory::initialize(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			if(m_initialized) {
				THROW_NIMBLE_TOKEN_EXCEPTION(NIMBLE_TOKEN_EXCEPTION_INITIALIZED);
			}

			m_initialized = true;
			m_map.clear();
		}

		bool 
		_nimble_token_factory::is_allocated(void)
		{
			return (nimble_token_factory::m_instance != NULL);
		}

		bool 
		_nimble_token_factory::is_initialized(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return m_initialized;
		}

		size_t 
		_nimble_token_factory::reference_count(
			__in const nimble_uid &uid
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_TOKEN_EXCEPTION(NIMBLE_TOKEN_EXCEPTION_UNINITIALIZED);
			}

			return find(uid)->second.second;
		}

		size_t 
		_nimble_token_factory::size(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_TOKEN_EXCEPTION(NIMBLE_TOKEN_EXCEPTION_UNINITIALIZED);
			}

			return m_map.size();
		}

		std::string 
		_nimble_token_factory::to_string(
			__in_opt bool verbose
			)
		{
			std::stringstream result;
			std::map<nimble_uid, std::pair<nimble_token, size_t>>::iterator iter;

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

			return CHK_STR(result.str());
		}

		void 
		_nimble_token_factory::uninitialize(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_TOKEN_EXCEPTION(NIMBLE_TOKEN_EXCEPTION_UNINITIALIZED);
			}

			m_map.clear();
			m_initialized = false;
		}
	}
}
