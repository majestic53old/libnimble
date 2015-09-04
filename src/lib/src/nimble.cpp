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
#include "../include/nimble_type.h"

namespace NIMBLE {

	nimble_ptr nimble::m_instance = NULL;

	_nimble::_nimble(void) :
		m_initialized(false)
	{
		std::atexit(nimble::_delete);
	}

	_nimble::~_nimble(void)
	{

		if(m_initialized) {
			uninitialize();
		}
	}

	void 
	_nimble::_delete(void)
	{

		if(nimble::m_instance) {
			delete nimble::m_instance;
			nimble::m_instance = NULL;
		}
	}

	nimble_ptr 
	_nimble::acquire(void)
	{

		if(!nimble::m_instance) {

			nimble::m_instance = new nimble;
			if(!nimble::m_instance) {
				THROW_NIMBLE_EXCEPTION(NIMBLE_EXCEPTION_ALLOCATION);
			}
		}

		return nimble::m_instance;
	}

	void 
	_nimble::initialize(void)
	{
		SERIALIZE_CALL_RECUR(m_lock);

		if(m_initialized) {
			THROW_NIMBLE_EXCEPTION(NIMBLE_EXCEPTION_INITIALIZED);
		}

		m_initialized = true;

		// TODO: initialize components
	}

	bool 
	_nimble::is_allocated(void)
	{
		return (nimble::m_instance != NULL);
	}

	bool 
	_nimble::is_initialized(void)
	{
		SERIALIZE_CALL_RECUR(m_lock);
		return m_initialized;
	}

	std::string 
	_nimble::to_string(
		__in_opt bool verbose
		)
	{
		std::stringstream result;

		SERIALIZE_CALL_RECUR(m_lock);

		result << "(" << (m_initialized ? "INIT" : "UNINIT") << ") " << NIMBLE_HEADER;

		if(verbose) {
			result << " (" << VAL_AS_HEX(nimble_ptr, this) << ")";
		}

		// TODO: print components

		return CHK_STR(result.str());
	}

	void 
	_nimble::uninitialize(void)
	{
		SERIALIZE_CALL_RECUR(m_lock);

		if(!m_initialized) {
			THROW_NIMBLE_EXCEPTION(NIMBLE_EXCEPTION_UNINITIALIZED);
		}

		// TODO: uninitialize components

		m_initialized = false;
	}

	std::string 
	_nimble::version(void)
	{
		return VER_STR;
	}
}
