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

namespace NIMBLE {

	namespace COMPONENT {

		typedef void (*nimble_cmd_cb)(int);

		typedef void (*_nimble_cmd_fact_cb)(nimble_uid);

		typedef class _nimble_command :
				public nimble_uid_class {

			public:

				_nimble_command(void);

				virtual ~_nimble_command(void);

				bool is_active(void);

				int result(void);

				void run(
					__in int count,
					__in const char **arguments,
					__in _nimble_cmd_fact_cb complete
					);

				void stop(void);

				std::thread &thread(void);

				virtual std::string to_string(
					__in_opt bool verbose = false
					);

			protected:

				_nimble_command(
					__in const _nimble_command &other
					);

				_nimble_command &operator=(
					__in const _nimble_command &other
					);

				void run_command(
					__in int count,
					__in const char **arguments,
					__in _nimble_cmd_fact_cb complete
					);

				bool m_active;

				int m_result;

				bool m_stop;

				std::thread m_thread;

			private:

				std::recursive_mutex m_lock;

		} nimble_command, *nimble_command_ptr;

		typedef class _nimble_command_factory {

			public:

				~_nimble_command_factory(void);

				bool contains(
					__in const nimble_uid &uid
					);

				nimble_uid &generate(void);

				void initialize(void);

				static bool is_allocated(void);

				bool is_initialized(void);

				void run(
					__in const nimble_uid &uid,
					__in int count,
					__in const char **arguments,
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

				std::map<nimble_uid, std::pair<nimble_command, 
						std::pair<nimble_cmd_cb, _nimble_cmd_fact_cb>>>::iterator find(
					__in const nimble_uid &uid
					);

				/*
				 * TODO: Calling conventions
				 * 	1) User calls generate and gets a UID
				 * 	2) User calls run with UID and command callback
				 * 	3) User command is run using command::run with factory callback
				 * 	4) When command is complete, factory callback is invoked
				 * 	5) Factory callback calls user callback with status
				 *	7) Factory callback deletes instance of command from factory map
				 */

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
