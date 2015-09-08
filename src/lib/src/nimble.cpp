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
	#define ENV_HOST "HOSTNAME"
	#define ENV_PWD "PWD"
	#define ENV_UNKNOWN "unknown"
	#define ENV_USER "USERNAME"

	nimble_ptr nimble::m_instance = NULL;

	_nimble::_nimble(void) :
		m_factory_uid(nimble_uid_factory::acquire()),
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

	void 
	_nimble::_signal_abort(
		__in int sig
		)
	{
		std::cout << std::endl << "Aborted." << std::endl;
		std::exit(sig);
	}

	void 
	_nimble::_signal_attention(
		__in int sig
		)
	{
		// TODO
		std::cout << std::endl;
		std::exit(sig);
		// ---
	}

	void 
	_nimble::_signal_float(
		__in int sig
		)
	{
		std::cout << std::endl << "Floating-point exception." << std::endl;
		std::exit(sig);
	}

	void 
	_nimble::_signal_illegal(
		__in int sig
		)
	{
		std::cout << std::endl << "Illegal operation." << std::endl;
		std::exit(sig);
	}

	void 
	_nimble::_signal_invalid(
		__in int sig
		)
	{
		std::cout << std::endl << "Segmentation fault." << std::endl;
		std::exit(sig);
	}

	void 
	_nimble::_signal_terminate(
		__in int sig
		)
	{
		std::cout << std::endl << "Terminated." << std::endl;
		std::exit(sig);
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

	nimble_uid_factory_ptr 
	_nimble::acquire_uid(void)
	{
		SERIALIZE_CALL_RECUR(m_lock);
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

		SERIALIZE_CALL_RECUR(m_lock);

		if(!m_initialized) {
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
		std::cout << (!user.empty() ? user : ENV_UNKNOWN) << CHAR_HOST_SEP
			<< (!host.empty() ? host : ENV_UNKNOWN);
		CLEAR_TERM_ATTRIB(std::cout);

		SET_TERM_ATTRIB(std::cout, 2, COL_FORM_BOLD, COL_FORE_YELLOW);
		std::cout << " -> ";
		CLEAR_TERM_ATTRIB(std::cout);
	}

	std::map<std::string, std::string>::iterator 
	_nimble::environment_find(
		__in const std::string &field
		)
	{
		SERIALIZE_CALL_RECUR(m_lock);

		if(!m_initialized) {
			THROW_NIMBLE_EXCEPTION(NIMBLE_EXCEPTION_UNINITIALIZED);
		}

		return m_environment_map.find(field);
	}

	void 
	_nimble::environment_update(
		__in_opt const char **environment,
		__in_opt const char *entry
		)
	{
		size_t len;
		std::string field;
		const char *value = NULL;
		std::map<std::string, std::string>::iterator iter;

		SERIALIZE_CALL_RECUR(m_lock);

		if(!m_initialized) {
			THROW_NIMBLE_EXCEPTION(NIMBLE_EXCEPTION_UNINITIALIZED);
		}

		if(!environment) {

			if(entry) {

				value = std::getenv(entry);
				if(value) {

					iter = environment_find(entry);
					if(iter == m_environment_map.end()) {
						m_environment_map.insert(std::pair<std::string, std::string>(
							entry, value));
					} else {
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
				m_environment_map.insert(std::pair<std::string, std::string>(
					field.substr(0, len),
					field.substr(len + 1, field.size())));
			}
		}
	}

	void 
	_nimble::initialize(void)
	{
		SERIALIZE_CALL_RECUR(m_lock);

		if(m_initialized) {
			THROW_NIMBLE_EXCEPTION(NIMBLE_EXCEPTION_INITIALIZED);
		}

		m_initialized = true;
		m_environment_map.clear();
		m_factory_uid->initialize();

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

	void 
	_nimble::signal_set(void)
	{
		SERIALIZE_CALL_RECUR(m_lock);

		if(!m_initialized) {
			THROW_NIMBLE_EXCEPTION(NIMBLE_EXCEPTION_UNINITIALIZED);
		}

		std::signal(SIGABRT, nimble::_signal_abort);
		std::signal(SIGINT, nimble::_signal_attention);
		std::signal(SIGFPE, nimble::_signal_float);
		std::signal(SIGILL, nimble::_signal_illegal);
		std::signal(SIGSEGV, nimble::_signal_invalid);
		std::signal(SIGTERM, nimble::_signal_terminate);
	}

	int 
	_nimble::start(
		__in int count,
		__in const char **arguments,
		__in const char **environment
		)
	{		
		int result = 0;
		bool update = true;
		std::string home, host, input, pwd, user;

		SERIALIZE_CALL_RECUR(m_lock);

		if(!m_initialized) {
			THROW_NIMBLE_EXCEPTION(NIMBLE_EXCEPTION_UNINITIALIZED);
		}

		signal_set();
		environment_update(environment);

		try {

			for(;;) {
				result = 0;
				display_prompt(home, host, pwd, user, true, update);

				if(update) {
					update = false;
				}

				std::getline(std::cin, input);

				try {

					// TODO: process commands

				} catch(nimble_exception &exc) {
					std::cerr << exc.to_string(true) << std::endl;
					result = INVALID(int);
				} catch(std::exception &exc) {
					std::cerr << exc.what() << std::endl;
					result = INVALID(int);
				}
			}
		} catch(nimble_exception &exc) {
			std::cerr << exc.to_string(true) << std::endl;
			result = INVALID(int);
		} catch(std::exception &exc) {
			std::cerr << exc.what() << std::endl;
			result = INVALID(int);
		}

		return result;
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

		result << std::endl << m_factory_uid->to_string(verbose);

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

		m_factory_uid->uninitialize();
		m_environment_map.clear();
		m_initialized = false;
	}

	std::string 
	_nimble::version(void)
	{
		return VER_STR;
	}
}
