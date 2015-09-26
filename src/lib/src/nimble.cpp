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

#include <csignal>
#include <cstdlib>
#include "../include/nimble.h"
#include "../include/nimble_type.h"

namespace NIMBLE {

	#define CHAR_ENV_ASSIGN '='
	#define CHAR_ENV_HOME '~'
	#define CHAR_HOST_SEP '@'
	#define ENV_HOME "HOME"
	#define ENV_HOST "HOST"
	#define ENV_PWD "PWD"
	#define ENV_UNKNOWN "unknown"
	#define ENV_USER "USER"

	nimble_ptr nimble::m_instance = NULL;

	_nimble::_nimble(void) :
		m_factory_command(nimble_command_factory::acquire()),
		m_factory_node(nimble_node_factory::acquire()),
		m_factory_token(nimble_token_factory::acquire()),
		m_factory_uid(nimble_uid_factory::acquire()),
		m_initialized(false),
		m_result(0)
	{
		TRACE_ENTRY(TRACE_VERBOSE);

		std::atexit(nimble::_delete);

		TRACE_EXIT(TRACE_VERBOSE);
	}

	_nimble::~_nimble(void)
	{
		TRACE_ENTRY(TRACE_VERBOSE);

		if(m_initialized) {
			uninitialize();
		}

		TRACE_EXIT(TRACE_VERBOSE);
	}

	void 
	_nimble::_delete(void)
	{
		TRACE_ENTRY(TRACE_VERBOSE);

		if(nimble::m_instance) {
			delete nimble::m_instance;
			TRACE_MESSAGE(TRACE_INFORMATION, "Deleted library instance, ptr. 0x%p", 
				nimble::m_instance);
			nimble::m_instance = NULL;
		}

		TRACE_EXIT(TRACE_VERBOSE);
	}

	void 
	_nimble::_signal_abort(
		__in int sig
		)
	{
		TRACE_ENTRY(TRACE_VERBOSE);

		std::cout << std::endl << "Aborted." << std::endl;

		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "sig. 0x%x", sig);
		std::exit(sig);
	}

	void 
	_nimble::_signal_attention(
		__in int sig
		)
	{

		// TODO
		/*nimble_ptr inst = NULL;

		TRACE_ENTRY(TRACE_VERBOSE);

		if(nimble::is_allocated()) {

			inst = nimble::acquire();
			if(inst && inst->is_initialized()) {
				inst->acquire_command()->stop_last(SIGKILL);
			}
		}*/
		// ---

		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "sig. 0x%x", sig);
		std::exit(sig);
	}

	void 
	_nimble::_signal_illegal(
		__in int sig
		)
	{
		TRACE_ENTRY(TRACE_VERBOSE);

		std::cout << std::endl << "Illegal operation." << std::endl;

		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "sig. 0x%x", sig);
		std::exit(sig);
	}

	void 
	_nimble::_signal_invalid(
		__in int sig
		)
	{
		TRACE_ENTRY(TRACE_VERBOSE);

		std::cout << std::endl << "Segmentation fault." << std::endl;

		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "sig. 0x%x", sig);
		std::exit(sig);
	}

	void 
	_nimble::_signal_terminate(
		__in int sig
		)
	{
		TRACE_ENTRY(TRACE_VERBOSE);

		std::cout << std::endl << "Terminated." << std::endl;

		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "sig. 0x%x", sig);
		std::exit(sig);
	}

	nimble_ptr 
	_nimble::acquire(void)
	{
		nimble_ptr result = NULL;

		TRACE_ENTRY(TRACE_VERBOSE);

		if(!nimble::m_instance) {

			nimble::m_instance = new nimble;
			if(!nimble::m_instance) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", NIMBLE_EXCEPTION_STRING(
					NIMBLE_EXCEPTION_ALLOCATION));
				THROW_NIMBLE_EXCEPTION(NIMBLE_EXCEPTION_ALLOCATION);
			} else {
				TRACE_MESSAGE(TRACE_INFORMATION, "Allocated library instance, ptr. 0x%p", 
					nimble::m_instance);
			}
		}
		
		result = nimble::m_instance;

		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", result);
		return result;
	}

	nimble_command_factory_ptr 
	_nimble::acquire_command(void)
	{
		TRACE_ENTRY(TRACE_VERBOSE);
		SERIALIZE_CALL_RECUR(m_lock);
		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", m_factory_command);
		return m_factory_command;
	}

	nimble_node_factory_ptr 
	_nimble::acquire_node(void)
	{
		TRACE_ENTRY(TRACE_VERBOSE);
		SERIALIZE_CALL_RECUR(m_lock);
		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", m_factory_node);
		return m_factory_node;
	}

	nimble_token_factory_ptr 
	_nimble::acquire_token(void)
	{
		TRACE_ENTRY(TRACE_VERBOSE);
		SERIALIZE_CALL_RECUR(m_lock);
		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", m_factory_token);
		return m_factory_token;
	}

	nimble_uid_factory_ptr 
	_nimble::acquire_uid(void)
	{
		TRACE_ENTRY(TRACE_VERBOSE);
		SERIALIZE_CALL_RECUR(m_lock);
		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", m_factory_uid);
		return m_factory_uid;
	}

	void 
	_nimble::display_prompt(
		__in std::string &home,
		__in std::string &host,
		__in std::string &pwd,
		__in std::string &user,
		__in_opt bool advance,
		__in_opt bool update
		)
	{
		size_t pos = 0;
		std::map<std::string, std::string>::iterator iter;

		TRACE_ENTRY(TRACE_VERBOSE);
		SERIALIZE_CALL_RECUR(m_lock);

		if(!m_initialized) {
			TRACE_MESSAGE(TRACE_ERROR, "%s", NIMBLE_EXCEPTION_STRING(
				NIMBLE_EXCEPTION_UNINITIALIZED));
			THROW_NIMBLE_EXCEPTION(NIMBLE_EXCEPTION_UNINITIALIZED);
		}

		if(update) {

			iter = environment_find(ENV_HOME);
			if(iter != m_environment_map.end()) {
				home = iter->second;
			} else {
				home.clear();
			}
			
			iter = environment_find(ENV_HOST);
			if(iter != m_environment_map.end()) {
				host = iter->second;
			} else {
				host.clear();
			}

			iter = environment_find(ENV_PWD);
			if(iter != m_environment_map.end()) {
				pwd = iter->second;
			} else {
				pwd.clear();
			}

			iter = environment_find(ENV_USER);
			if(iter != m_environment_map.end()) {
				user = iter->second;
			} else {
				user.clear();
			}

			pos = pwd.find(home, pos);
			if(pos != std::string::npos) {
				pwd.replace(pos, home.size(), std::string(1, CHAR_ENV_HOME));
			}
		}

		if(advance) {
			std::cout << std::endl;
		}

		CLEAR_TERM_ATTRIB(std::cout);

		if(!pwd.empty()) {
			SET_TERM_ATTRIB(std::cout, 1, COL_FORE_GREEN);
			std::cout << "[" << pwd << "]" << std::endl;
			CLEAR_TERM_ATTRIB(std::cout);
		}

		SET_TERM_ATTRIB(std::cout, 1, COL_FORE_CYAN);
		std::cout << (!user.empty() ? user : ENV_UNKNOWN);

		if(!host.empty()) {
			std::cout << CHAR_HOST_SEP << (!host.empty() 
				? host : ENV_UNKNOWN);
		}

		CLEAR_TERM_ATTRIB(std::cout);

		SET_TERM_ATTRIB(std::cout, 2, COL_FORM_BOLD, COL_FORE_YELLOW);
		std::cout << " -> ";
		CLEAR_TERM_ATTRIB(std::cout);

		TRACE_EXIT(TRACE_VERBOSE);
	}

	std::string 
	_nimble::environment_as_string(
		__in_opt bool verbose
		)
	{
		std::stringstream result;
		std::map<std::string, std::string>::iterator iter;

		TRACE_ENTRY(TRACE_VERBOSE);
		SERIALIZE_CALL_RECUR(m_lock);

		if(verbose) {
			result << "ENV[" << m_environment_map.size() << "] {" << std::endl;
		}

		for(iter = m_environment_map.begin(); iter != m_environment_map.end(); ++iter) {

			if(iter != m_environment_map.begin()) {
				result << std::endl;
			}

			if(verbose) {
				result << "\t";
			}

			result << CHK_STR(iter->first) << " --> " << CHK_STR(iter->second);
		}

		if(verbose) {
			result << std::endl << "}";
		}

		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result.str()));
		return CHK_STR(result.str());
	}

	bool 
	_nimble::environment_contains(
		__in const std::string &field
		)
	{
		bool result;

		TRACE_ENTRY(TRACE_VERBOSE);
		SERIALIZE_CALL_RECUR(m_lock);

		result = (m_environment_map.find(field) != m_environment_map.end());

		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", result);
		return result;
	}

	std::map<std::string, std::string>::iterator 
	_nimble::environment_find(
		__in const std::string &field
		)
	{
		std::map<std::string, std::string>::iterator result;

		TRACE_ENTRY(TRACE_VERBOSE);
		SERIALIZE_CALL_RECUR(m_lock);

		if(!m_initialized) {
			TRACE_MESSAGE(TRACE_ERROR, "%s", NIMBLE_EXCEPTION_STRING(
				NIMBLE_EXCEPTION_UNINITIALIZED));
			THROW_NIMBLE_EXCEPTION(NIMBLE_EXCEPTION_UNINITIALIZED);
		}

		result = m_environment_map.find(field);

		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%p", result);
		return result;
	}

	void 
	_nimble::environment_set(
		__in const std::string &field,
		__in const std::string &value
		)
	{
		std::map<std::string, std::string>::iterator iter;

		TRACE_ENTRY(TRACE_VERBOSE);
		SERIALIZE_CALL_RECUR(m_lock);

		iter = m_environment_map.find(field);
		if(iter == m_environment_map.end()) {
			m_environment_map.insert(std::pair<std::string, std::string>(field, value));
		} else {
			iter->second = value;
		}

		TRACE_EXIT(TRACE_VERBOSE);
	}

	void 
	_nimble::environment_update(
		__in_opt const char **environment,
		__in_opt const char *entry
		)
	{
		size_t len;		
		const char *value = NULL;
		std::string field, field_entry, field_value;
		std::map<std::string, std::string>::iterator iter;

		TRACE_ENTRY(TRACE_VERBOSE);
		SERIALIZE_CALL_RECUR(m_lock);

		if(!m_initialized) {
			TRACE_MESSAGE(TRACE_ERROR, "%s", NIMBLE_EXCEPTION_STRING(
				NIMBLE_EXCEPTION_UNINITIALIZED));
			THROW_NIMBLE_EXCEPTION(NIMBLE_EXCEPTION_UNINITIALIZED);
		}

		if(!environment) {

			if(entry) {

				value = std::getenv(entry);
				if(value) {

					iter = environment_find(entry);
					if(iter == m_environment_map.end()) {
						TRACE_MESSAGE(TRACE_INFORMATION, "Adding environment pair <%s, %s>", 
							entry, value);
						m_environment_map.insert(std::pair<std::string, std::string>(
							entry, value));
					} else {
						TRACE_MESSAGE(TRACE_INFORMATION, 
							"Changing environment pair <%s, %s> -> <%s, %s>", 
							entry, CHK_STR(iter->second), entry, value);
						iter->second = value;
					}
				}
			}
		} else {
			m_environment_map.clear();

			for(; environment != NULL; ++environment) {

				if(!*environment) {
					break;
				}

				field = *environment;
				len = field.find_first_of(CHAR_ENV_ASSIGN);
				field_entry = field.substr(0, len);
				field_value = field.substr(len + 1, field.size());
				TRACE_MESSAGE(TRACE_INFORMATION, "Adding environment pair <%s, %s>", 
					CHK_STR(field_entry), CHK_STR(field_value));
				m_environment_map.insert(std::pair<std::string, std::string>(
					field_entry, field_value));
			}
		}

		TRACE_EXIT(TRACE_VERBOSE);
	}

	void 
	_nimble::initialize(void)
	{
		TRACE_ENTRY(TRACE_VERBOSE);
		SERIALIZE_CALL_RECUR(m_lock);

		if(m_initialized) {
			TRACE_MESSAGE(TRACE_ERROR, "%s", NIMBLE_EXCEPTION_STRING(
				NIMBLE_EXCEPTION_INITIALIZED));
			THROW_NIMBLE_EXCEPTION(NIMBLE_EXCEPTION_INITIALIZED);
		}

		m_initialized = true;
		m_environment_map.clear();
		m_factory_uid->initialize();
		m_factory_token->initialize();
		m_factory_node->initialize();
		m_factory_command->initialize();
		m_result = 0;

		TRACE_EXIT(TRACE_VERBOSE);
	}

	bool 
	_nimble::is_allocated(void)
	{
		bool result;

		TRACE_ENTRY(TRACE_VERBOSE);

		result = (nimble::m_instance != NULL);

		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", result);
		return result;
	}

	bool 
	_nimble::is_initialized(void)
	{
		TRACE_ENTRY(TRACE_VERBOSE);
		SERIALIZE_CALL_RECUR(m_lock);
		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", m_initialized);
		return m_initialized;
	}

	void 
	_nimble::signal_set(void)
	{
		TRACE_ENTRY(TRACE_VERBOSE);
		SERIALIZE_CALL_RECUR(m_lock);

		if(!m_initialized) {
			TRACE_MESSAGE(TRACE_ERROR, "%s", NIMBLE_EXCEPTION_STRING(
				NIMBLE_EXCEPTION_UNINITIALIZED));
			THROW_NIMBLE_EXCEPTION(NIMBLE_EXCEPTION_UNINITIALIZED);
		}

		std::signal(SIGABRT, nimble::_signal_abort);
		std::signal(SIGINT, nimble::_signal_attention);
		std::signal(SIGILL, nimble::_signal_illegal);
		std::signal(SIGSEGV, nimble::_signal_invalid);
		std::signal(SIGTERM, nimble::_signal_terminate);

		TRACE_EXIT(TRACE_VERBOSE);
	}

	int 
	_nimble::run(
		__in int count,
		__in const char **arguments,
		__in const char **environment
		)
	{
		int result = 0;
		bool update = true;
		std::string home, host, input, pwd, user;

		TRACE_ENTRY(TRACE_VERBOSE);
		SERIALIZE_CALL_RECUR(m_lock);

		if(!m_initialized) {
			TRACE_MESSAGE(TRACE_ERROR, "%s", NIMBLE_EXCEPTION_STRING(
				NIMBLE_EXCEPTION_UNINITIALIZED));
			THROW_NIMBLE_EXCEPTION(NIMBLE_EXCEPTION_UNINITIALIZED);
		}

		signal_set();
		environment_update(environment);

		try {

			for(;;) {
				result = 0;
				m_result = 0;
				display_prompt(home, host, pwd, user, true, update);

				if(update) {
					update = false;
				}

				std::getline(std::cin, input);
				TRACE_MESSAGE(TRACE_INFORMATION, "Command: \'%s\'[%lu]", 
					CHK_STR(input), input.size());

				try {
					m_factory_command->run(m_factory_command->generate(), input, 
						update);
					result = m_result;
					TRACE_MESSAGE(TRACE_INFORMATION, "Command result: 0x%x", 
						result);					
				} catch(nimble_exception &exc) {
					TRACE_MESSAGE(TRACE_ERROR, "%s", CHK_STR(exc.to_string(true)));
					std::cerr << exc.to_string(true) << std::endl;
					result = INVALID_TYPE(int);
				} catch(std::exception &exc) {
					TRACE_MESSAGE(TRACE_ERROR, "%s", exc.what());
					std::cerr << exc.what() << std::endl;
					result = INVALID_TYPE(int);
				}
			}
		} catch(nimble_exception &exc) {
			TRACE_MESSAGE(TRACE_ERROR, "%s", CHK_STR(exc.to_string(true)));
			std::cerr << exc.to_string(true) << std::endl;
			result = INVALID_TYPE(int);
		} catch(std::exception &exc) {
			TRACE_MESSAGE(TRACE_ERROR, "%s", exc.what());
			std::cerr << exc.what() << std::endl;
			result = INVALID_TYPE(int);
		}

		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", result);
		return result;
	}

	std::string 
	_nimble::to_string(
		__in_opt bool verbose
		)
	{
		std::stringstream result;

		TRACE_ENTRY(TRACE_VERBOSE);
		SERIALIZE_CALL_RECUR(m_lock);

		result << "(" << (m_initialized ? "INIT" : "UNINIT") << ") " << NIMBLE_HEADER;

		if(verbose) {
			result << " (" << VAL_AS_HEX(nimble_ptr, this) << ")";
		}

		result << std::endl << m_factory_uid->to_string(verbose);
		result << std::endl << m_factory_token->to_string(verbose);
		result << std::endl << m_factory_node->to_string(verbose);
		result << std::endl << m_factory_command->to_string(verbose);

		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result.str()));
		return CHK_STR(result.str());
	}

	void 
	_nimble::uninitialize(void)
	{
		TRACE_ENTRY(TRACE_VERBOSE);
		SERIALIZE_CALL_RECUR(m_lock);

		if(!m_initialized) {
			TRACE_MESSAGE(TRACE_ERROR, "%s", NIMBLE_EXCEPTION_STRING(
				NIMBLE_EXCEPTION_UNINITIALIZED));
			THROW_NIMBLE_EXCEPTION(NIMBLE_EXCEPTION_UNINITIALIZED);
		}

		m_result = 0;
		m_factory_command->uninitialize();
		m_factory_node->uninitialize();
		m_factory_token->uninitialize();
		m_factory_uid->uninitialize();
		m_environment_map.clear();
		m_initialized = false;

		TRACE_EXIT(TRACE_VERBOSE);
	}

	std::string 
	_nimble::version(void)
	{
		std::string result;

		TRACE_ENTRY(TRACE_VERBOSE);

		result = VER_STR;

		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result));
		return CHK_STR(result);
	}
}
