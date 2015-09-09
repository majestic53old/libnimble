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
#include "../include/nimble_uid_type.h"

namespace NIMBLE {

	namespace COMPONENT {

		_nimble_uid::_nimble_uid(
			__in_opt nimble_uid_t uid
			) :
				m_uid(uid)
		{
			return;
		}

		_nimble_uid::_nimble_uid(
			__in const _nimble_uid &other
			) :
				m_uid(other.m_uid)
		{
			return;
		}

		_nimble_uid::~_nimble_uid(void)
		{
			return;
		}

		_nimble_uid &
		_nimble_uid::operator=(
			__in const _nimble_uid &other
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);
		
			if(this != &other) {
				m_uid = other.m_uid;
			}

			return *this;
		}

		bool 
		_nimble_uid::operator==(
			__in const _nimble_uid &other
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return (m_uid == other.m_uid);
		}

		bool 
		_nimble_uid::operator!=(
			__in const _nimble_uid &other
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return (m_uid != other.m_uid);
		}

		std::string 
		_nimble_uid::as_string(
			__in const _nimble_uid &uid,
			__in_opt bool verbose
			)
		{
			std::stringstream result;

			REF_PARAM(verbose);

			result << "{";

			if(uid.m_uid != UID_INVALID) {
				result << VAL_AS_HEX(nimble_uid_t, uid.m_uid);
			} else {
				result << "INV";
			}

			result << "}";

			return CHK_STR(result.str());
		}

		bool 
		_nimble_uid::is_valid(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return (m_uid != UID_INVALID);
		}

		std::string 
		_nimble_uid::to_string(
			__in_opt bool verbose
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return nimble_uid::as_string(*this, verbose);
		}

		nimble_uid_t &
		_nimble_uid::uid(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return m_uid;
		}

		bool 
		operator<(
			__in const nimble_uid &left,
			__in const nimble_uid &right
			)
		{
			return (left.m_uid < right.m_uid);
		}

		nimble_uid_factory_ptr nimble_uid_factory::m_instance = NULL;

		_nimble_uid_factory::_nimble_uid_factory(void) :
			m_initialized(false)
		{
			std::atexit(nimble_uid_factory::_delete);
		}

		_nimble_uid_factory::~_nimble_uid_factory(void)
		{

			if(m_initialized) {
				uninitialize();
			}
		}

		void 
		_nimble_uid_factory::_delete(void)
		{

			if(nimble_uid_factory::m_instance) {
				delete nimble_uid_factory::m_instance;
				nimble_uid_factory::m_instance = NULL;
			}
		}

		nimble_uid_factory_ptr 
		_nimble_uid_factory::acquire(void)
		{

			if(!nimble_uid_factory::m_instance) {

				nimble_uid_factory::m_instance = new nimble_uid_factory;
				if(!nimble_uid_factory::m_instance) {
					THROW_NIMBLE_UID_EXCEPTION(NIMBLE_UID_EXCEPTION_ALLOCATION);
				}
			}

			return nimble_uid_factory::m_instance;
		}

		bool 
		_nimble_uid_factory::contains(
			__in const nimble_uid &uid
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_UID_EXCEPTION(NIMBLE_UID_EXCEPTION_UNINITIALIZED);
			}

			return (m_map.find(uid) != m_map.end());
		}

		size_t 
		_nimble_uid_factory::decrement_reference(
			__in const nimble_uid &uid
			)
		{
			size_t result;
			std::map<nimble_uid, size_t>::iterator iter;

			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_UID_EXCEPTION(NIMBLE_UID_EXCEPTION_UNINITIALIZED);
			}

			iter = find(uid);

			result = --iter->second;
			if(result < REF_INITIAL) {
				m_surplus.insert(iter->first);
				m_map.erase(iter);
			}

			return result;
		}

		std::map<nimble_uid, size_t>::iterator 
		_nimble_uid_factory::find(
			__in const nimble_uid &uid
			)
		{
			std::map<nimble_uid, size_t>::iterator result;

			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_UID_EXCEPTION(NIMBLE_UID_EXCEPTION_UNINITIALIZED);
			}

			result = m_map.find(uid);
			if(result == m_map.end()) {
				THROW_NIMBLE_UID_EXCEPTION_MESSAGE(NIMBLE_UID_EXCEPTION_NOT_FOUND,
					"%s", CHK_STR(nimble_uid::as_string(uid)));
			}

			return result;
		}

		size_t 
		_nimble_uid_factory::generate(
			__out nimble_uid &uid
			)
		{
			size_t result = REF_INITIAL;
			std::set<nimble_uid>::iterator iter;

			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_UID_EXCEPTION(NIMBLE_UID_EXCEPTION_UNINITIALIZED);
			}

			if(!m_surplus.empty()) {
				iter = m_surplus.begin();
				uid = *iter;
				m_surplus.erase(iter);
			} else if(m_next != UID_INVALID) {
				uid = m_next.uid()++;
			} else {
				THROW_NIMBLE_UID_EXCEPTION(NIMBLE_UID_EXCEPTION_RESOURCES);
			}

			m_map.insert(std::pair<nimble_uid, size_t>(uid, result));

			return result;
		}

		size_t 
		_nimble_uid_factory::increment_reference(
			__in const nimble_uid &uid
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_UID_EXCEPTION(NIMBLE_UID_EXCEPTION_UNINITIALIZED);
			}

			return ++find(uid)->second;
		}

		void 
		_nimble_uid_factory::initialize(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			if(m_initialized) {
				THROW_NIMBLE_UID_EXCEPTION(NIMBLE_UID_EXCEPTION_INITIALIZED);
			}

			m_initialized = true;
			m_map.clear();
			m_next = UID_INITIAL;
			m_surplus.clear();
		}

		bool 
		_nimble_uid_factory::is_allocated(void)
		{
			return (nimble_uid_factory::m_instance != NULL);
		}

		bool 
		_nimble_uid_factory::is_initialized(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return m_initialized;
		}

		size_t 
		_nimble_uid_factory::reference_count(
			__in const nimble_uid &uid
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_UID_EXCEPTION(NIMBLE_UID_EXCEPTION_UNINITIALIZED);
			}

			return find(uid)->second;
		}

		size_t 
		_nimble_uid_factory::size(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_UID_EXCEPTION(NIMBLE_UID_EXCEPTION_UNINITIALIZED);
			}

			return m_map.size();
		}

		std::string 
		_nimble_uid_factory::to_string(
			__in_opt bool verbose
			)
		{
			std::stringstream result;
			std::map<nimble_uid, size_t>::iterator iter;

			SERIALIZE_CALL_RECUR(m_lock);

			result << "(" << (m_initialized ? "INIT" : "UNINIT") << ") " << NIMBLE_UID_HEADER 
				<< "[" << m_map.size() << "]";

			if(verbose) {
				result << " (" << VAL_AS_HEX(nimble_uid_factory_ptr, this) << ")";

				for(iter = m_map.begin(); iter != m_map.end(); ++iter) {
					result << std::endl << "--- " << nimble_uid::as_string(iter->first)
						<< ", ref. " << iter->second;
				}
			}

			return CHK_STR(result.str());
		}

		void 
		_nimble_uid_factory::uninitialize(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_UID_EXCEPTION(NIMBLE_UID_EXCEPTION_UNINITIALIZED);
			}

			m_map.clear();
			m_next = UID_INITIAL;
			m_surplus.clear();
			m_initialized = false;
		}

		_nimble_uid_class::_nimble_uid_class(void)
		{
			nimble_uid_factory_ptr fact = factory();

			if(fact && fact->is_initialized()) {
				fact->generate(*this);
			} else {
				m_uid = UID_INVALID;
			}
		}

		_nimble_uid_class::_nimble_uid_class(
			__in const _nimble_uid_class &other
			)
		{
			nimble_uid_factory_ptr fact = factory();

			if(fact 
					&& fact->is_initialized() 
					&& fact->contains(*this)) {
				fact->decrement_reference(*this);
			}

			nimble_uid::operator=(other);

			if(fact 
					&& fact->is_initialized() 
					&& fact->contains(*this)) {
				fact->increment_reference(*this);
			}
		}

		_nimble_uid_class::~_nimble_uid_class(void)
		{
			nimble_uid_factory_ptr fact = factory();

			if(fact 
					&& fact->is_initialized() 
					&& fact->contains(*this)) {
				fact->decrement_reference(*this);
				m_uid = UID_INVALID;
			}
		}

		_nimble_uid_class &
		_nimble_uid_class::operator=(
			__in const _nimble_uid_class &other
			)
		{
			nimble_uid_factory_ptr fact = factory();

			SERIALIZE_CALL_RECUR(m_lock);

			if(this != &other) {

				if(fact 
						&& fact->is_initialized() 
						&& fact->contains(*this)) {
					fact->decrement_reference(*this);
				}

				nimble_uid::operator=(other);

				if(fact 
						&& fact->is_initialized() 
						&& fact->contains(*this)) {
					fact->increment_reference(*this);
				}
			}

			return *this;
		}

		nimble_uid_factory_ptr 
		_nimble_uid_class::factory(void)
		{
			nimble_ptr inst = NULL;
			nimble_uid_factory_ptr result = NULL;

			SERIALIZE_CALL_RECUR(m_lock);

			if(nimble::is_allocated()) {

				inst = nimble::acquire();
				if(inst && nimble_uid_factory::is_allocated()) {
					result = inst->acquire_uid();
				}
			}

			return result;
		}
	}
}
