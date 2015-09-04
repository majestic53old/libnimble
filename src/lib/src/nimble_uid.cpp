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
			std::stringstream result;

			SERIALIZE_CALL_RECUR(m_lock);
			REF_PARAM(verbose);

			result << "{";

			if(m_uid != UID_INVALID) {
				result << VAL_AS_HEX(nimble_uid_t, m_uid);
			} else {
				result << "INV";
			}

			result << "}";

			return CHK_STR(result.str());
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
	}
}
