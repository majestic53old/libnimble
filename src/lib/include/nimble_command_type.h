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

#ifndef NIMBLE_COMMAND_TYPE_H_
#define NIMBLE_COMMAND_TYPE_H_

namespace NIMBLE {

	namespace COMPONENT {

		#define NIMBLE_COMMAND_HEADER "Command"

		#ifndef NDEBUG
		#define NIMBLE_COMMAND_EXCEPTION_HEADER NIMBLE_COMMAND_HEADER
		#else
		#define NIMBLE_COMMAND_EXCEPTION_HEADER EXCEPTION_HEADER
		#endif // NDEBUG

		enum {
			NIMBLE_COMMAND_EXCEPTION_ACTIVE = 0,
			NIMBLE_COMMAND_EXCEPTION_ALLOCATED,
			NIMBLE_COMMAND_EXCEPTION_ALLOCATED_SHARE,
			NIMBLE_COMMAND_EXCEPTION_INITIALIZED,
			NIMBLE_COMMAND_EXCEPTION_INVALID_CALLBACK,
			NIMBLE_COMMAND_EXCEPTION_INVALID_PID,
			NIMBLE_COMMAND_EXCEPTION_NOT_ACTIVE,
			NIMBLE_COMMAND_EXCEPTION_NOT_FOUND,
			NIMBLE_COMMAND_EXCEPTION_PID_KILL,
			NIMBLE_COMMAND_EXCEPTION_PID_WAIT,
			NIMBLE_COMMAND_EXCEPTION_UNALLOCATED_SHARE,
			NIMBLE_COMMAND_EXCEPTION_UNINITIALIZED,
		};

		#define NIMBLE_COMMAND_EXCEPTION_MAX NIMBLE_COMMAND_EXCEPTION_UNINITIALIZED

		static const std::string NIMBLE_COMMAND_EXCEPTION_STR[] = {
			"Command is active",
			"Failed to allocate command component",
			"Failed to allocate command share",
			"Command component is initialized",
			"Command received invalid callback",
			"Command failed to create child process",
			"Command is not active",
			"Command does not exist",
			"Command failed to kill child process",
			"Command failed to wait for child process",
			"Failed to unallocate command share",
			"Command component is uninitialized",
			};

		#define NIMBLE_COMMAND_EXCEPTION_STRING(_TYPE_) \
			((_TYPE_) > NIMBLE_COMMAND_EXCEPTION_MAX ? UNKNOWN_EXCEPTION : \
			CHK_STR(NIMBLE_COMMAND_EXCEPTION_STR[_TYPE_]))

		#define THROW_NIMBLE_COMMAND_EXCEPTION(_EXCEPT_) \
			THROW_EXCEPTION(NIMBLE_COMMAND_EXCEPTION_HEADER, \
			NIMBLE_COMMAND_EXCEPTION_STRING(_EXCEPT_))
		#define THROW_NIMBLE_COMMAND_EXCEPTION_MESSAGE(_EXCEPT_, _FORMAT_, ...) \
			THROW_EXCEPTION_MESSAGE(NIMBLE_COMMAND_EXCEPTION_HEADER, \
			NIMBLE_COMMAND_EXCEPTION_STRING(_EXCEPT_), _FORMAT_, __VA_ARGS__)

		class _nimble_command;
		typedef _nimble_command nimble_command, *nimble_command_ptr;

		class _nimble_command_factory;
		typedef _nimble_command_factory nimble_command_factory, 
			*nimble_command_factory_ptr;
	}
}

#endif // NIMBLE_COMMAND_TYPE_H_
