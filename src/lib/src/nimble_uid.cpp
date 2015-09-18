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
			TRACE_ENTRY(TRACE_VERBOSE);
			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_uid::_nimble_uid(
			__in const _nimble_uid &other
			) :
				m_uid(other.m_uid)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_uid::~_nimble_uid(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_uid &
		_nimble_uid::operator=(
			__in const _nimble_uid &other
			)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);
		
			if(this != &other) {
				m_uid = other.m_uid;
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", this);

			return *this;
		}

		bool 
		_nimble_uid::operator==(
			__in const _nimble_uid &other
			)
		{
			bool result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = (m_uid == other.m_uid);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", result);
			return result;
		}

		bool 
		_nimble_uid::operator!=(
			__in const _nimble_uid &other
			)
		{
			bool result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = (m_uid != other.m_uid);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", result);
			return (m_uid != other.m_uid);
		}

		std::string 
		_nimble_uid::as_string(
			__in const _nimble_uid &uid,
			__in_opt bool verbose
			)
		{
			std::stringstream result;

			TRACE_ENTRY(TRACE_VERBOSE);
			REF_PARAM(verbose);

			result << "{";

			if(uid.m_uid != UID_INVALID) {
				result << VAL_AS_HEX(nimble_uid_t, uid.m_uid);
			} else {
				result << "INV";
			}

			result << "}";

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result.str()));
			return CHK_STR(result.str());
		}

		bool 
		_nimble_uid::is_valid(void)
		{
			bool result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = (m_uid != UID_INVALID);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", result);
			return result;
		}

		std::string 
		_nimble_uid::to_string(
			__in_opt bool verbose
			)
		{
			std::string result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = nimble_uid::as_string(*this, verbose);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result));
			return CHK_STR(result);
		}

		nimble_uid_t &
		_nimble_uid::uid(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);
			return m_uid;
		}

		bool 
		operator<(
			__in const nimble_uid &left,
			__in const nimble_uid &right
			)
		{
			bool result;

			TRACE_ENTRY(TRACE_VERBOSE);

			result = (left.m_uid < right.m_uid);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", result);
			return result;
		}

		nimble_uid_factory_ptr nimble_uid_factory::m_instance = NULL;

		_nimble_uid_factory::_nimble_uid_factory(void) :
			m_initialized(false)
		{
			TRACE_ENTRY(TRACE_VERBOSE);

			std::atexit(nimble_uid_factory::_delete);

			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_uid_factory::~_nimble_uid_factory(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);

			if(m_initialized) {
				uninitialize();
			}

			TRACE_EXIT(TRACE_VERBOSE);
		}

		void 
		_nimble_uid_factory::_delete(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);

			if(nimble_uid_factory::m_instance) {
				TRACE_MESSAGE(TRACE_INFORMATION, "Deleted uid component instance, ptr. 0x%p", 
					nimble_uid_factory::m_instance);
				delete nimble_uid_factory::m_instance;
				nimble_uid_factory::m_instance = NULL;
			}

			TRACE_EXIT(TRACE_VERBOSE);
		}

		nimble_uid_factory_ptr 
		_nimble_uid_factory::acquire(void)
		{
			nimble_uid_factory_ptr result = NULL;

			TRACE_ENTRY(TRACE_VERBOSE);

			if(!nimble_uid_factory::m_instance) {

				nimble_uid_factory::m_instance = new nimble_uid_factory;
				if(!nimble_uid_factory::m_instance) {
					TRACE_MESSAGE(TRACE_ERROR, "%s", 
						NIMBLE_UID_EXCEPTION_STRING(NIMBLE_UID_EXCEPTION_ALLOCATION));
					THROW_NIMBLE_UID_EXCEPTION(NIMBLE_UID_EXCEPTION_ALLOCATION);
				} else {
					TRACE_MESSAGE(TRACE_INFORMATION, "Allocated uid component instance, ptr. 0x%p", 
						nimble_uid_factory::m_instance);
				}
			}

			result = nimble_uid_factory::m_instance;

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", result);
			return result;
		}

		bool 
		_nimble_uid_factory::contains(
			__in const nimble_uid &uid
			)
		{
			bool result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_UID_EXCEPTION_STRING(NIMBLE_UID_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_UID_EXCEPTION(NIMBLE_UID_EXCEPTION_UNINITIALIZED);
			}

			result = (m_map.find(uid) != m_map.end());

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", result);
			return result;
		}

		size_t 
		_nimble_uid_factory::decrement_reference(
			__in const nimble_uid &uid
			)
		{
			size_t result;
			std::map<nimble_uid, size_t>::iterator iter;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_UID_EXCEPTION_STRING(NIMBLE_UID_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_UID_EXCEPTION(NIMBLE_UID_EXCEPTION_UNINITIALIZED);
			}

			iter = find(uid);

			result = --iter->second;
			if(result < REF_INITIAL) {
				TRACE_MESSAGE(TRACE_INFORMATION, "Removing uid: %s", 
					CHK_STR(nimble_uid::as_string(uid, true)));
				m_surplus.insert(iter->first);
				m_map.erase(iter);
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		std::map<nimble_uid, size_t>::iterator 
		_nimble_uid_factory::find(
			__in const nimble_uid &uid
			)
		{
			std::map<nimble_uid, size_t>::iterator result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_UID_EXCEPTION_STRING(NIMBLE_UID_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_UID_EXCEPTION(NIMBLE_UID_EXCEPTION_UNINITIALIZED);
			}

			result = m_map.find(uid);
			if(result == m_map.end()) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, %s", 
					NIMBLE_UID_EXCEPTION_STRING(NIMBLE_UID_EXCEPTION_UNINITIALIZED),
					 CHK_STR(nimble_uid::as_string(uid)));
				THROW_NIMBLE_UID_EXCEPTION_MESSAGE(NIMBLE_UID_EXCEPTION_NOT_FOUND,
					"%s", CHK_STR(nimble_uid::as_string(uid)));
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", result);
			return result;
		}

		size_t 
		_nimble_uid_factory::generate(
			__out nimble_uid &uid
			)
		{
			size_t result = REF_INITIAL;
			std::set<nimble_uid>::iterator iter;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_UID_EXCEPTION_STRING(NIMBLE_UID_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_UID_EXCEPTION(NIMBLE_UID_EXCEPTION_UNINITIALIZED);
			}

			if(!m_surplus.empty()) {
				iter = m_surplus.begin();
				uid = *iter;
				m_surplus.erase(iter);
			} else if(m_next != UID_INVALID) {
				uid = m_next.uid()++;
			} else {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_UID_EXCEPTION_STRING(NIMBLE_UID_EXCEPTION_RESOURCES));
				THROW_NIMBLE_UID_EXCEPTION(NIMBLE_UID_EXCEPTION_RESOURCES);
			}

			TRACE_MESSAGE(TRACE_INFORMATION, "Generating new uid: %s", 
				CHK_STR(uid.to_string(true)));
			m_map.insert(std::pair<nimble_uid, size_t>(uid, result));

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		size_t 
		_nimble_uid_factory::increment_reference(
			__in const nimble_uid &uid
			)
		{
			size_t result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_UID_EXCEPTION_STRING(NIMBLE_UID_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_UID_EXCEPTION(NIMBLE_UID_EXCEPTION_UNINITIALIZED);
			}

			result = ++find(uid)->second;

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		void 
		_nimble_uid_factory::initialize(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_UID_EXCEPTION_STRING(NIMBLE_UID_EXCEPTION_INITIALIZED));
				THROW_NIMBLE_UID_EXCEPTION(NIMBLE_UID_EXCEPTION_INITIALIZED);
			}

			m_initialized = true;
			m_map.clear();
			m_next = UID_INITIAL;
			m_surplus.clear();
			TRACE_MESSAGE(TRACE_INFORMATION, "%s", "Uid component instance initialized");

			TRACE_EXIT(TRACE_VERBOSE);
		}

		bool 
		_nimble_uid_factory::is_allocated(void)
		{
			bool result;

			TRACE_ENTRY(TRACE_VERBOSE);

			result = (nimble_uid_factory::m_instance != NULL);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", result);
			return result;
		}

		bool 
		_nimble_uid_factory::is_initialized(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);
			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", m_initialized);
			return m_initialized;
		}

		size_t 
		_nimble_uid_factory::reference_count(
			__in const nimble_uid &uid
			)
		{
			size_t result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_UID_EXCEPTION_STRING(NIMBLE_UID_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_UID_EXCEPTION(NIMBLE_UID_EXCEPTION_UNINITIALIZED);
			}

			result = find(uid)->second;

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		size_t 
		_nimble_uid_factory::size(void)
		{
			size_t result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_UID_EXCEPTION_STRING(NIMBLE_UID_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_UID_EXCEPTION(NIMBLE_UID_EXCEPTION_UNINITIALIZED);
			}

			result = m_map.size();

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		std::string 
		_nimble_uid_factory::to_string(
			__in_opt bool verbose
			)
		{
			std::stringstream result;
			std::map<nimble_uid, size_t>::iterator iter;

			TRACE_ENTRY(TRACE_VERBOSE);
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

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result.str()));
			return CHK_STR(result.str());
		}

		void 
		_nimble_uid_factory::uninitialize(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_UID_EXCEPTION_STRING(NIMBLE_UID_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_UID_EXCEPTION(NIMBLE_UID_EXCEPTION_UNINITIALIZED);
			}

			m_map.clear();
			m_next = UID_INITIAL;
			m_surplus.clear();
			m_initialized = false;
			TRACE_MESSAGE(TRACE_INFORMATION, "%s", "Uid component instance uninitialized");

			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_uid_class::_nimble_uid_class(void)
		{
			nimble_uid_factory_ptr fact = factory();

			TRACE_ENTRY(TRACE_VERBOSE);

			if(fact && fact->is_initialized()) {
				fact->generate(*this);
			} else {
				m_uid = UID_INVALID;
			}

			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_uid_class::_nimble_uid_class(
			__in const _nimble_uid_class &other
			)
		{
			nimble_uid_factory_ptr fact = factory();

			TRACE_ENTRY(TRACE_VERBOSE);

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

			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_uid_class::~_nimble_uid_class(void)
		{
			nimble_uid_factory_ptr fact = factory();

			TRACE_ENTRY(TRACE_VERBOSE);

			if(fact 
					&& fact->is_initialized() 
					&& fact->contains(*this)) {
				fact->decrement_reference(*this);
				m_uid = UID_INVALID;
			}

			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_uid_class &
		_nimble_uid_class::operator=(
			__in const _nimble_uid_class &other
			)
		{
			nimble_uid_factory_ptr fact = factory();

			TRACE_ENTRY(TRACE_VERBOSE);
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

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", this);
			return *this;
		}

		nimble_uid_factory_ptr 
		_nimble_uid_class::factory(void)
		{
			nimble_ptr inst = NULL;
			nimble_uid_factory_ptr result = NULL;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(nimble::is_allocated()) {

				inst = nimble::acquire();
				if(inst && nimble_uid_factory::is_allocated()) {
					result = inst->acquire_uid();
				}
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", result);
			return result;
		}
	}
}
