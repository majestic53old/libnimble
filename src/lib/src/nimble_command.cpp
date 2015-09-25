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

#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
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
			TRACE_ENTRY(TRACE_VERBOSE);
			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_command::_nimble_command(
			__in const _nimble_command &other
			) :
				nimble_uid_class(other),
				m_active(other.m_active),
				m_complete(other.m_complete),
				m_pid(other.m_pid),
				m_result(other.m_result)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_command::~_nimble_command(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);

			if(m_active) {
				stop();
			}

			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_command &
		_nimble_command::operator=(
			__in const _nimble_command &other
			)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(this != &other) {
				nimble_uid_class::operator=(other);
				m_active = other.m_active;
				m_complete = other.m_complete;
				m_pid = other.m_pid;
				m_result = other.m_result;
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", this);
			return *this;
		}

		std::string 
		_nimble_command::as_string(
			__in const _nimble_command &command,
			__in_opt bool verbose
			)
		{
			std::stringstream result;

			TRACE_ENTRY(TRACE_VERBOSE);

			if(verbose) {
				result << nimble_uid::as_string(command.m_uid) << " ";
			}

			result << "[" << (command.m_active ? "ACT" : "INACT") << "]";

			if(command.m_active) {
				result << ", comp. " << VAL_AS_HEX(_nimble_cmd_fact_cb, command.m_complete)
					<< ", pid. " << VAL_AS_HEX(pid_t, command.m_pid)
					<< ", res. " << VAL_AS_HEX(int, command.m_result);
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result.str()));
			return CHK_STR(result.str());
		}

		bool 
		_nimble_command::is_active(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);
			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", m_active);
			return m_active;
		}

		int 
		_nimble_command::result(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);
			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", m_result);
			return m_result;
		}

		void 
		_nimble_command::run(
			__in const std::string &command,
			__in _nimble_cmd_fact_cb complete,
			__out bool &update
			)
		{
			nimble_executor exe;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(m_active) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, %s", NIMBLE_COMMAND_EXCEPTION_STRING(
					NIMBLE_COMMAND_EXCEPTION_ACTIVE), CHK_STR(nimble_uid::as_string(m_uid)));
				THROW_NIMBLE_COMMAND_EXCEPTION_MESSAGE(NIMBLE_COMMAND_EXCEPTION_ACTIVE,
					"%s", CHK_STR(nimble_uid::as_string(m_uid)));
			}

			if(!complete) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, %s", NIMBLE_COMMAND_EXCEPTION_STRING(
					NIMBLE_COMMAND_EXCEPTION_INVALID_CALLBACK), CHK_STR(nimble_uid::as_string(m_uid)));
				THROW_NIMBLE_COMMAND_EXCEPTION_MESSAGE(NIMBLE_COMMAND_EXCEPTION_INVALID_CALLBACK,
					"%s", CHK_STR(nimble_uid::as_string(m_uid)));
			}

			m_active = true;
			m_complete = complete;
			m_pid = PID_INVALID;
			m_result = 0;

			m_pid = fork();
			if(m_pid == PID_INVALID) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, %s", NIMBLE_COMMAND_EXCEPTION_STRING(
					NIMBLE_COMMAND_EXCEPTION_INVALID_PID), CHK_STR(nimble_uid::as_string(m_uid)));
				THROW_NIMBLE_COMMAND_EXCEPTION_MESSAGE(NIMBLE_COMMAND_EXCEPTION_INVALID_PID,
					"%s", CHK_STR(nimble_uid::as_string(m_uid)));
			}

			if(!m_pid) {

				try {
					exe.set(command);
					m_result = exe.evaluate();
				} catch(nimble_exception &exc) {
					TRACE_MESSAGE(TRACE_ERROR, "%s", CHK_STR(exc.to_string(true)));
					std::cerr << exc.to_string(true) << std::endl;
					m_result = INVALID_TYPE(int);
				} catch(std::exception &exc) {
					TRACE_MESSAGE(TRACE_ERROR, "%s", exc.what());
					std::cerr << exc.what() << std::endl;
					m_result = INVALID_TYPE(int);
				}

				_exit(m_result);
			} else {

				if(waitpid(m_pid, &m_result, 0) == PID_INVALID) {
					TRACE_MESSAGE(TRACE_ERROR, "%s, %s, pid. %x", NIMBLE_COMMAND_EXCEPTION_STRING(
						NIMBLE_COMMAND_EXCEPTION_PID_WAIT), CHK_STR(nimble_uid::as_string(m_uid)), 
						m_pid);
					THROW_NIMBLE_COMMAND_EXCEPTION_MESSAGE(NIMBLE_COMMAND_EXCEPTION_PID_WAIT,
						"%s, pid. %x", CHK_STR(nimble_uid::as_string(m_uid)), m_pid);
				}

				m_active = false;
				m_pid = PID_INVALID;
			}

			if(m_complete) {
				m_complete(*this);
				m_complete = NULL;
			}

			TRACE_EXIT(TRACE_VERBOSE);
		}

		void 
		_nimble_command::stop(
			__in_opt int sig
			)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_active) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, %s", NIMBLE_COMMAND_EXCEPTION_STRING(
					NIMBLE_COMMAND_EXCEPTION_NOT_ACTIVE),
					CHK_STR(nimble_uid::as_string(m_uid)));
				THROW_NIMBLE_COMMAND_EXCEPTION_MESSAGE(NIMBLE_COMMAND_EXCEPTION_NOT_ACTIVE,
					"%s", CHK_STR(nimble_uid::as_string(m_uid)));
			}

			if(m_pid > 0) {

				if(kill(m_pid, sig) == INVALID_TYPE(int)) {
					TRACE_MESSAGE(TRACE_ERROR, "%s, %s, pid. %x, err. %x", 
						NIMBLE_COMMAND_EXCEPTION_STRING(NIMBLE_COMMAND_EXCEPTION_PID_KILL),
						CHK_STR(nimble_uid::as_string(m_uid)), m_pid, errno);
					THROW_NIMBLE_COMMAND_EXCEPTION_MESSAGE(NIMBLE_COMMAND_EXCEPTION_PID_KILL,
						"%s, pid. %x, err. %x", CHK_STR(nimble_uid::as_string(m_uid)), 
						m_pid, errno);
				}
			}

			m_active = false;
			m_pid = PID_INVALID;
			m_result = 0;

			if(m_complete) {
				m_complete(*this);
				m_complete = NULL;
			}

			TRACE_EXIT(TRACE_VERBOSE);
		}

		std::string 
		_nimble_command::to_string(
			__in_opt bool verbose
			)
		{
			std::string result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = nimble_command::as_string(*this, verbose);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result));
			return CHK_STR(result);
		}

		nimble_command_factory_ptr nimble_command_factory::m_instance = NULL;

		_nimble_command_factory::_nimble_command_factory(void) :
			m_initialized(false),
			m_last(UID_INVALID)
		{
			TRACE_ENTRY(TRACE_VERBOSE);

			std::atexit(nimble_command_factory::_delete);

			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_command_factory::~_nimble_command_factory(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);

			if(m_initialized) {
				uninitialize();
			}

			TRACE_EXIT(TRACE_VERBOSE);
		}

		void 
		_nimble_command_factory::_delete(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);

			if(nimble_command_factory::m_instance) {
				delete nimble_command_factory::m_instance;
				TRACE_MESSAGE(TRACE_INFORMATION, "Deleted command component instance, ptr. 0x%p", 
					nimble_command_factory::m_instance);
				nimble_command_factory::m_instance = NULL;
			}

			TRACE_EXIT(TRACE_VERBOSE);
		}

		void 
		_nimble_command_factory::_remove(
			__in const nimble_uid &uid
			)
		{
			nimble_command_factory_ptr inst = NULL;

			TRACE_ENTRY(TRACE_VERBOSE);

			if(nimble_command_factory::is_allocated()) {

				inst = nimble_command_factory::acquire();
				if(inst 
						&& inst->is_initialized()
						&& inst->contains(uid)) {
					TRACE_MESSAGE(TRACE_INFORMATION, "Removing command: %s", 
						CHK_STR(nimble_uid::as_string(uid, true)));
					inst->m_map.erase(inst->find(uid));
				}
			}

			TRACE_EXIT(TRACE_VERBOSE);
		}

		nimble_command_factory_ptr 
		_nimble_command_factory::acquire(void)
		{
			nimble_command_factory_ptr result = NULL;

			TRACE_ENTRY(TRACE_VERBOSE);

			if(!nimble_command_factory::m_instance) {

				nimble_command_factory::m_instance = new nimble_command_factory;
				if(!nimble_command_factory::m_instance) {
					TRACE_MESSAGE(TRACE_ERROR, "%s", NIMBLE_COMMAND_EXCEPTION_STRING(
						NIMBLE_COMMAND_EXCEPTION_ALLOCATED));
					THROW_NIMBLE_COMMAND_EXCEPTION(NIMBLE_COMMAND_EXCEPTION_ALLOCATED);
				} else {
					TRACE_MESSAGE(TRACE_INFORMATION, "Allocated command component instance, ptr. 0x%p", 
						nimble_command_factory::m_instance);
				}
			}

			result = nimble_command_factory::m_instance;

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", result);
			return result;
		}

		bool 
		_nimble_command_factory::contains(
			__in const nimble_uid &uid
			)
		{
			bool result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", NIMBLE_COMMAND_EXCEPTION_STRING(
					NIMBLE_COMMAND_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_COMMAND_EXCEPTION(NIMBLE_COMMAND_EXCEPTION_UNINITIALIZED);
			}

			result = (m_map.find(uid) != m_map.end());

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", result);
			return result;
		}

		std::map<nimble_uid, std::pair<nimble_command, 
				_nimble_cmd_fact_cb>>::iterator 
		_nimble_command_factory::find(
			__in const nimble_uid &uid
			)
		{
			std::map<nimble_uid, std::pair<nimble_command, 
					_nimble_cmd_fact_cb>>::iterator result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", NIMBLE_COMMAND_EXCEPTION_STRING(
					NIMBLE_COMMAND_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_COMMAND_EXCEPTION(NIMBLE_COMMAND_EXCEPTION_UNINITIALIZED);
			}

			result = m_map.find(uid);
			if(result == m_map.end()) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, %s", NIMBLE_COMMAND_EXCEPTION_STRING(
					NIMBLE_COMMAND_EXCEPTION_NOT_FOUND), 
					CHK_STR(nimble_uid::as_string(uid)));
				THROW_NIMBLE_COMMAND_EXCEPTION_MESSAGE(NIMBLE_COMMAND_EXCEPTION_NOT_FOUND,
					"%s", CHK_STR(nimble_uid::as_string(uid)));
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", result);
			return result;
		}

		nimble_uid 
		_nimble_command_factory::generate(void)
		{
			nimble_command command;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", NIMBLE_COMMAND_EXCEPTION_STRING(
					NIMBLE_COMMAND_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_COMMAND_EXCEPTION(NIMBLE_COMMAND_EXCEPTION_UNINITIALIZED);
			}

			m_last = command.uid();
			TRACE_MESSAGE(TRACE_INFORMATION, "Generating new command: %s", 
				CHK_STR(m_last.to_string(true)));
			m_map.insert(std::pair<nimble_uid, std::pair<nimble_command, _nimble_cmd_fact_cb>>(
				m_last, std::pair<nimble_command, _nimble_cmd_fact_cb>(command, 
				nimble_command_factory::_remove)));

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(m_last.to_string(true)));
			return m_last;
		}

		void 
		_nimble_command_factory::initialize(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", NIMBLE_COMMAND_EXCEPTION_STRING(
					NIMBLE_COMMAND_EXCEPTION_INITIALIZED));
				THROW_NIMBLE_COMMAND_EXCEPTION(NIMBLE_COMMAND_EXCEPTION_INITIALIZED);
			}

			m_initialized = true;
			m_last = UID_INVALID;
			m_map.clear();
			TRACE_MESSAGE(TRACE_INFORMATION, "%s", "Command component instance initialized");

			TRACE_EXIT(TRACE_VERBOSE);
		}

		bool 
		_nimble_command_factory::is_allocated(void)
		{
			bool result;

			TRACE_ENTRY(TRACE_VERBOSE);

			result = (nimble_command_factory::m_instance != NULL);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", result);
			return result;
		}

		bool 
		_nimble_command_factory::is_initialized(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);
			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", m_initialized);
			return m_initialized;
		}

		void 
		_nimble_command_factory::run(
			__in const nimble_uid &uid,
			__in const std::string &command,
			__out bool &update
			)
		{
			std::map<nimble_uid, std::pair<nimble_command, 
					_nimble_cmd_fact_cb>>::iterator iter;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", NIMBLE_COMMAND_EXCEPTION_STRING(
					NIMBLE_COMMAND_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_COMMAND_EXCEPTION(NIMBLE_COMMAND_EXCEPTION_UNINITIALIZED);
			}

			iter = find(uid);
			TRACE_MESSAGE(TRACE_INFORMATION, "Running command \'%s\'", CHK_STR(command));
			iter->second.first.run(command, iter->second.second, update);

			TRACE_EXIT(TRACE_VERBOSE);
		}

		size_t 
		_nimble_command_factory::size(void)
		{
			size_t result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", NIMBLE_COMMAND_EXCEPTION_STRING(
					NIMBLE_COMMAND_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_COMMAND_EXCEPTION(NIMBLE_COMMAND_EXCEPTION_UNINITIALIZED);
			}

			result = m_map.size();

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		void 
		_nimble_command_factory::stop_last(
			__in_opt int sig
			)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", NIMBLE_COMMAND_EXCEPTION_STRING(
					NIMBLE_COMMAND_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_COMMAND_EXCEPTION(NIMBLE_COMMAND_EXCEPTION_UNINITIALIZED);
			}

			if(contains(m_last)) {
				TRACE_MESSAGE(TRACE_INFORMATION, "Stopping last process, sig. 0x%x", sig);
				find(m_last)->second.first.stop(sig);
			}

			TRACE_EXIT(TRACE_VERBOSE);
		}

		std::string 
		_nimble_command_factory::to_string(
			__in_opt bool verbose
			)
		{
			std::stringstream result;
			std::map<nimble_uid, std::pair<nimble_command, 
					_nimble_cmd_fact_cb>>::iterator iter;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result << "(" << (m_initialized ? "INIT" : "UNINIT") << ") " << NIMBLE_COMMAND_HEADER 
				<< "[" << m_map.size() << "]";

			if(verbose) {
				result << " (" << VAL_AS_HEX(nimble_command_factory_ptr, this) << ")";

				for(iter = m_map.begin(); iter != m_map.end(); ++iter) {
					result << std::endl << "--- " << iter->second.first.to_string(verbose);
				}
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result.str()));
			return CHK_STR(result.str());
		}

		void 
		_nimble_command_factory::uninitialize(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", NIMBLE_COMMAND_EXCEPTION_STRING(
					NIMBLE_COMMAND_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_COMMAND_EXCEPTION(NIMBLE_COMMAND_EXCEPTION_UNINITIALIZED);
			}

			m_last = UID_INVALID;
			m_map.clear();
			m_initialized = false;
			TRACE_MESSAGE(TRACE_INFORMATION, "%s", "Command component instance uninitialized");

			TRACE_EXIT(TRACE_VERBOSE);
		}
	}
}
