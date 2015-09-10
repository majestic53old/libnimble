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

#ifndef NIMBLE_COMMAND_H_
#define NIMBLE_COMMAND_H_

#include <unistd.h>

namespace NIMBLE {

	namespace COMPONENT {

		#define PID_INVALID INVALID(pid_t)

		typedef void (*nimble_cmd_cb)(
			__in int
			);

		typedef void (*_nimble_cmd_fact_cb)(
			__in const nimble_uid &
			);

		typedef class _nimble_command :
				public nimble_uid_class {

			public:

				_nimble_command(void);

				_nimble_command(
					__in const _nimble_command &other
					);

				virtual ~_nimble_command(void);

				_nimble_command &operator=(
					__in const _nimble_command &other
					);

				static std::string as_string(
					__in const _nimble_command &command,
					__in_opt bool verbose = false
					);

				bool is_active(void);

				int result(void);

				void run(
					__in const std::string &command,
					__in _nimble_cmd_fact_cb complete
					);

				void stop(void);

				virtual std::string to_string(
					__in_opt bool verbose = false
					);

			protected:

				void run_command(
					__in const std::string &command
					);

				bool m_active;

				_nimble_cmd_fact_cb m_complete;

				pid_t m_pid;

				int m_result;

				std::thread m_thread;

			private:

				std::recursive_mutex m_lock;

		} nimble_command, *nimble_command_ptr;

		typedef class _nimble_command_factory {

			public:

				~_nimble_command_factory(void);

				static _nimble_command_factory *acquire(void);

				bool contains(
					__in const nimble_uid &uid
					);

				nimble_uid generate(void);

				void initialize(void);

				static bool is_allocated(void);

				bool is_initialized(void);

				void run(
					__in const nimble_uid &uid,
					__in const std::string &command,
					__in nimble_cmd_cb complete
					);

				size_t size(void);

				std::string to_string(
					__in_opt bool verbose = false
					);

				void uninitialize(void);

			protected:

				_nimble_command_factory(void);

				_nimble_command_factory(
					__in const _nimble_command_factory &other
					);

				_nimble_command_factory &operator=(
					__in const _nimble_command_factory &other
					);

				static void _delete(void);

				static void _remove(
					__in const nimble_uid &uid
					);

				std::map<nimble_uid, std::pair<nimble_command, 
						std::pair<nimble_cmd_cb, _nimble_cmd_fact_cb>>>::iterator find(
					__in const nimble_uid &uid
					);

				std::map<nimble_uid, std::pair<nimble_command, 
					std::pair<nimble_cmd_cb, _nimble_cmd_fact_cb>>> m_map;

				bool m_initialized;

				static _nimble_command_factory *m_instance;

			private:

				std::recursive_mutex m_lock;

		} nimble_command_factory, *nimble_command_factory_ptr;
	}
}

#endif // NIMBLE_COMMAND_H_
