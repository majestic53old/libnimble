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

#include <signal.h>
#include "../include/nimble.h"
#include "../include/nimble_command_type.h"

namespace NIMBLE {

	namespace COMPONENT {

		_nimble_command::_nimble_command(void) :
			m_active(false),
			m_complete(NULL),
			m_pid(PID_INVALID),
			m_result(0)
		{
			return;
		}

		_nimble_command::_nimble_command(
			__in const _nimble_command &other
			) :
				m_active(other.m_active),
				m_complete(other.m_complete),
				m_pid(other.m_pid),
				m_result(other.m_result)
		{
			return;
		}

		_nimble_command::~_nimble_command(void)
		{

			if(m_active) {
				stop();
			}
		}

		_nimble_command &
		_nimble_command::operator=(
			__in const _nimble_command &other
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			if(this != &other) {
				m_active = other.m_active;
				m_complete = other.m_complete;
				m_pid = other.m_pid;
				m_result = other.m_result;
			}

			return *this;
		}

		std::string 
		_nimble_command::as_string(
			__in const _nimble_command &command,
			__in_opt bool verbose
			)
		{
			std::stringstream result;

			if(verbose) {
				result << "{" << VAL_AS_HEX(nimble_uid_t, command.m_uid) << "} ";
			}

			result << "[" << (command.m_thread.joinable() ? "SYNC" : "ASYNC") << ", " 
				<< (command.m_active ? "ACT" : "INACT") << "]";

			if(command.m_active) {
				result << " " << command.m_thread.get_id() << ", pid. " 
					<< VAL_AS_HEX(pid_t, command.m_pid) << ", cb. "
					<< VAL_AS_HEX(_nimble_cmd_fact_cb, command.m_complete);
			}

			if(verbose) {
				result << ", last. " << VAL_AS_HEX(int, command.m_result);
			}

			return CHK_STR(result.str());
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
			__in const std::string &command,
			__in _nimble_cmd_fact_cb complete
			)
		{
			// TODO
			std::cout << "_nimble_command::run entry" << std::endl;
			// ---

			SERIALIZE_CALL_RECUR(m_lock);

			if(!complete) {
				THROW_NIMBLE_COMMAND_EXCEPTION(NIMBLE_COMMAND_EXCEPTION_INVALID_CALLBACK);
			}

			if(m_active) {
				THROW_NIMBLE_COMMAND_EXCEPTION_MESSAGE(NIMBLE_COMMAND_EXCEPTION_NOT_ACTIVE,
					"%s", CHK_STR(nimble_uid::to_string(true)));
			}

			m_active = true;
			m_complete = complete;
			m_pid = PID_INVALID;

			m_thread = std::thread(&nimble_command::run_command, this, command);
			m_thread.join();

			if(m_complete) {
				m_complete(m_uid);
				m_complete = NULL;
			}

			m_active = false;
			m_pid = PID_INVALID;

			// TODO
			std::cout << "_nimble_command::run exit" << std::endl;
			// ---
		}

		void 
		_nimble_command::run_command(
			__in const std::string &command
			)
		{
			// TODO
			std::cout << "_nimble_command::run_command entry" << std::endl;
			// ---

			m_pid = fork();
			if(m_pid == PID_INVALID) {
				THROW_NIMBLE_COMMAND_EXCEPTION(NIMBLE_COMMAND_EXCEPTION_INVALID_PID);
			} else if(!m_pid) {

				// TODO: run command
				// TODO: set result
				//std::cout << command << std::endl;
				m_result = 0;
				exit(0);
				// ---
			}

			// TODO
			std::cout << "_nimble_command::run_command exit" << std::endl;
			// ---
		}

		void 
		_nimble_command::stop(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_active) {
				THROW_NIMBLE_COMMAND_EXCEPTION_MESSAGE(NIMBLE_COMMAND_EXCEPTION_NOT_ACTIVE,
					"%s", CHK_STR(nimble_uid::to_string(true)));
			}

			kill(m_pid, SIGTERM);

			// TODO: debug
			std::cout << "Process ";
			SET_TERM_ATTRIB(std::cout, 1, COL_FORE_YELLOW);
			std::cout << "[0x" << VAL_AS_HEX(pid_t, m_pid) << "]";
			CLEAR_TERM_ATTRIB(std::cout);
			std::cout << " killed." << std::endl;
			// ---

			if(m_thread.joinable()) {
				m_thread.join();
			}

			m_active = false;
			m_pid = PID_INVALID;

			if(m_complete) {
				m_complete(m_uid);
				m_complete = NULL;
			}	
		}

		std::string 
		_nimble_command::to_string(
			__in_opt bool verbose
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return nimble_command::as_string(*this, verbose);
		}

		nimble_command_factory_ptr nimble_command_factory::m_instance = NULL;

		_nimble_command_factory::_nimble_command_factory(void) :
			m_initialized(false)
		{
			std::atexit(nimble_command_factory::_delete);
		}

		_nimble_command_factory::~_nimble_command_factory(void)
		{

			if(m_initialized) {
				uninitialize();
			}
		}

		void 
		_nimble_command_factory::_delete(void)
		{

			if(nimble_command_factory::m_instance) {
				delete nimble_command_factory::m_instance;
				nimble_command_factory::m_instance = NULL;
			}
		}

		void 
		_nimble_command_factory::_remove(
			__in const nimble_uid &uid
			)
		{
			// TODO
			std::cout << "_nimble_command_factory::_remove entry" << std::endl;
			// ---

			nimble_command_factory_ptr inst = NULL;
			std::map<nimble_uid, std::pair<nimble_command, 
					std::pair<nimble_cmd_cb, _nimble_cmd_fact_cb>>>::iterator iter;

			if(nimble_command_factory::is_allocated()) {

				inst = nimble_command_factory::m_instance;
				if(inst 
						&& inst->is_initialized()
						&& inst->contains(uid)) {
					iter = inst->find(uid);
					iter->second.second.first(iter->second.first.result());
					inst->m_map.erase(iter);
				}
			}

			// TODO
			std::cout << "_nimble_command_factory::_remove exit" << std::endl;
			// ---
		}

		nimble_command_factory_ptr 
		_nimble_command_factory::acquire(void)
		{

			if(!nimble_command_factory::m_instance) {

				nimble_command_factory::m_instance = new nimble_command_factory;
				if(!nimble_command_factory::m_instance) {
					THROW_NIMBLE_COMMAND_EXCEPTION(NIMBLE_COMMAND_EXCEPTION_ALLOCATED);
				}
			}

			return nimble_command_factory::m_instance;
		}

		bool 
		_nimble_command_factory::contains(
			__in const nimble_uid &uid
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_COMMAND_EXCEPTION(NIMBLE_COMMAND_EXCEPTION_UNINITIALIZED);
			}

			return (m_map.find(uid) != m_map.end());
		}

		std::map<nimble_uid, std::pair<nimble_command, 
				std::pair<nimble_cmd_cb, _nimble_cmd_fact_cb>>>::iterator 
		_nimble_command_factory::find(
			__in const nimble_uid &uid
			)
		{
			std::map<nimble_uid, std::pair<nimble_command, 
					std::pair<nimble_cmd_cb, _nimble_cmd_fact_cb>>>::iterator result;

			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_COMMAND_EXCEPTION(NIMBLE_COMMAND_EXCEPTION_UNINITIALIZED);
			}

			result = m_map.find(uid);
			if(result == m_map.end()) {
				THROW_NIMBLE_COMMAND_EXCEPTION_MESSAGE(NIMBLE_COMMAND_EXCEPTION_NOT_FOUND,
					"%s", CHK_STR(nimble_uid::as_string(uid, true)));
			}

			return result;
		}

		nimble_uid 
		_nimble_command_factory::generate(void)
		{
			nimble_command cmd;

			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_COMMAND_EXCEPTION(NIMBLE_COMMAND_EXCEPTION_UNINITIALIZED);
			}

			m_map.insert(std::pair<nimble_uid, std::pair<nimble_command, 
				std::pair<nimble_cmd_cb, _nimble_cmd_fact_cb>>>(cmd.uid(), 
				std::pair<nimble_command, std::pair<nimble_cmd_cb, _nimble_cmd_fact_cb>>(cmd, 
				std::pair<nimble_cmd_cb, _nimble_cmd_fact_cb>(NULL, nimble_command_factory::_remove))));

			return cmd.uid();
		}

		void 
		_nimble_command_factory::initialize(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			if(m_initialized) {
				THROW_NIMBLE_COMMAND_EXCEPTION(NIMBLE_COMMAND_EXCEPTION_INITIALIZED);
			}

			m_initialized = true;
			m_map.clear();
		}

		bool 
		_nimble_command_factory::is_allocated(void)
		{
			return (nimble_command_factory::m_instance != NULL);
		}

		bool 
		_nimble_command_factory::is_initialized(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return m_initialized;
		}

		void 
		_nimble_command_factory::run(
			__in const nimble_uid &uid,
			__in const std::string &command,
			__in nimble_cmd_cb complete
			)
		{
			// TODO
			std::cout << "_nimble_command_factory::run entry" << std::endl;
			// ---

			std::map<nimble_uid, std::pair<nimble_command, 
					std::pair<nimble_cmd_cb, _nimble_cmd_fact_cb>>>::iterator iter;

			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_COMMAND_EXCEPTION(NIMBLE_COMMAND_EXCEPTION_UNINITIALIZED);
			}

			// TODO
			iter = find(uid);
			iter->second.second.first = complete;
			iter->second.first.run(command, iter->second.second.second);
			// ---

			// TODO
			std::cout << "_nimble_command_factory::run exit" << std::endl;
			// ---
		}

		size_t 
		_nimble_command_factory::size(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_COMMAND_EXCEPTION(NIMBLE_COMMAND_EXCEPTION_UNINITIALIZED);
			}

			return m_map.size();
		}

		std::string 
		_nimble_command_factory::to_string(
			__in_opt bool verbose
			)
		{
			std::stringstream result;
			std::map<nimble_uid, std::pair<nimble_command, 
					std::pair<nimble_cmd_cb, _nimble_cmd_fact_cb>>>::iterator iter;

			SERIALIZE_CALL_RECUR(m_lock);

			result << "(" << (m_initialized ? "INIT" : "UNINIT") << ") " << NIMBLE_COMMAND_HEADER 
				<< "[" << m_map.size() << "]";

			if(verbose) {
				result << " (" << VAL_AS_HEX(nimble_command_factory_ptr, this) << ")";

				for(iter = m_map.begin(); iter != m_map.end(); ++iter) {
					result << std::endl << "--- " << nimble_command::as_string(
						iter->second.first, true);
				}
			}

			return CHK_STR(result.str());
		}

		void 
		_nimble_command_factory::uninitialize(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_COMMAND_EXCEPTION(NIMBLE_COMMAND_EXCEPTION_UNINITIALIZED);
			}

			m_map.clear();
			m_initialized = false;
		}
	}
}
