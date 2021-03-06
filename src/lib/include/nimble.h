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

#ifndef NIMBLE_H_
#define NIMBLE_H_

#define NDEBUG

#ifndef TLEVEL
#define TLEVEL TRACE_ERROR
#endif // TLEVEL

#ifndef TLOGOUT
#define TLOGOUT "nimble"
#endif // TLOGOUT

#include <map>

#ifndef NIMBLE
#define NIMBLE nb
#endif // NIMBLE

#include "nimble_defines.h"
#include "nimble_color.h"
#include "nimble_trace.h"
#include "nimble_language.h"
#include "nimble_exception.h"
#include "nimble_environment.h"

using namespace NIMBLE;

#ifndef COMPONENT
#define COMPONENT comp
#endif // COMPONENT

#include "nimble_uid.h"
#include "nimble_command.h"
#include "nimble_token.h"
#include "nimble_node.h"

using namespace NIMBLE::COMPONENT;

#ifndef LANGUAGE
#define LANGUAGE lang
#endif // LANGUAGE

#include "nimble_lexer.h"
#include "nimble_parser.h"
#include "nimble_executor.h"

using namespace NIMBLE::LANGUAGE;

namespace NIMBLE {

	typedef class _nimble {

		public:

			~_nimble(void);

			static _nimble *acquire(void);

			nimble_command_factory_ptr acquire_command(void);

			nimble_node_factory_ptr acquire_node(void);

			nimble_token_factory_ptr acquire_token(void);

			nimble_uid_factory_ptr acquire_uid(void);

			std::string environment_as_string(
				__in_opt bool verbose = false
				);

			static std::string environment_as_string(
				__in const nimble_environment_map &environment,
				__in_opt bool verbose = false
				);

			bool environment_contains(
				__in const std::string &field
				);

			nimble_environment_map::iterator environment_find(
				__in const std::string &field
				);

			nimble_environment_map_ptr environment_instance(void);

			void environment_set(
				__in const std::string &field,
				__in const std::string &value
				);

			void initialize(void);

			static bool is_allocated(void);

			bool is_initialized(void);

			int run(
				__in int count,
				__in const char **arguments,
				__in const char **environment
				);

			std::string to_string(
				__in_opt bool verbose = false
				);

			void uninitialize(void);

			static std::string version(void);

		protected:

			_nimble(void);

			_nimble(
				__in const _nimble &other
				);

			_nimble &operator=(
				__in const _nimble &other
				);

			static void _delete(void);

			static void _signal_abort(
				__in int sig
				);

			static void _signal_attention(
				__in int sig
				);

			static void _signal_illegal(
				__in int sig
				);

			static void _signal_invalid(
				__in int sig
				);

			static void _signal_terminate(
				__in int sig
				);

			void display_prompt(
				__in std::string &home,
				__in std::string &host,
				__in std::string &pwd,
				__in std::string &user,
				__in_opt bool advance = false,
				__in_opt bool update = false
				);

			void environment_update(
				__in_opt const char **environment = NULL,
				__in_opt const char *entry = NULL
				);

			void signal_set(void);

			nimble_environment_map m_environment_map;

			nimble_command_factory_ptr m_factory_command;

			nimble_node_factory_ptr m_factory_node;

			nimble_token_factory_ptr m_factory_token;

			nimble_uid_factory_ptr m_factory_uid;

			bool m_initialized;

			static _nimble *m_instance;

			int m_result;

		private:

			std::recursive_mutex m_lock;

	} nimble, *nimble_ptr;
}

#endif // NIMBLE_H_
