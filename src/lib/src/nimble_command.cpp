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
#include "../include/nimble_command_type.h"

namespace NIMBLE {

	namespace COMPONENT {

		_nimble_command::_nimble_command(void) :
			m_active(false),
			m_result(0),
			m_stop(false)
		{
			return;
		}

		_nimble_command::~_nimble_command(void)
		{
			stop();
		}

		bool 
		_nimble_command::is_active(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return m_active;
		}

		int 
		_nimble_command::result(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return m_result;
		}

		void 
		_nimble_command::run(
			__in int count,
			__in const char **arguments,
			__in _nimble_cmd_fact_cb complete
			)
		{
			bool async = false;

			if((!count && arguments) || (count && !arguments)) {
				THROW_NIMBLE_COMMAND_EXCEPTION_MESSAGE(
					NIMBLE_COMMAND_EXCEPTION_INVALID_ARGUMENTS,
					"%p[%lu]", arguments, count);
			}

			if(!complete) {
				THROW_NIMBLE_COMMAND_EXCEPTION(
					NIMBLE_COMMAND_EXCEPTION_INVALID_CALLBACK);
			}

			m_active = true;
			m_stop = false;

			// TODO: determine if background command

			m_thread = std::thread(&nimble_command::run_command, this, 
				count, arguments, complete);

			if(async) {
				m_thread.detach();
			} else if(m_thread.joinable()) {
				m_thread.join();
			}
		}

		void 
		_nimble_command::run_command(
			__in int count,
			__in const char **arguments,
			__in _nimble_cmd_fact_cb complete
			)
		{

			if((!count && arguments) || (count && !arguments)) {
				THROW_NIMBLE_COMMAND_EXCEPTION_MESSAGE(
					NIMBLE_COMMAND_EXCEPTION_INVALID_ARGUMENTS,
					"%p[%lu]", arguments, count);
			}

			if(!complete) {
				THROW_NIMBLE_COMMAND_EXCEPTION(
					NIMBLE_COMMAND_EXCEPTION_INVALID_CALLBACK);
			}

			// TODO: run command

			m_active = false;

			if(complete) {
				complete(m_uid);
			}
		}

		void 
		_nimble_command::stop(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			m_stop = true;

			if(m_thread.joinable()) {
				m_thread.join();
			}

			m_active = false;
		}

		std::thread &
		_nimble_command::thread(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return m_thread;
		}

		std::string 
		_nimble_command::to_string(
			__in_opt bool verbose
			)
		{
			std::stringstream result;

			SERIALIZE_CALL_RECUR(m_lock);

			if(verbose) {
				result << nimble_uid::to_string(true) << " ";
			}

			result << "[" << (m_thread.joinable() ? "SYNC" : "ASYNC") << ", " 
				<< (m_active ? "ACT" : "INACT") << "]";

			if(m_active) {
				result << " " << m_thread.get_id();
			}

			if(verbose) {
				result << ", last. " << VAL_AS_HEX(int, m_result);
			}

			return CHK_STR(result.str());
		}

		// TODO: implement command factory
	}
}
