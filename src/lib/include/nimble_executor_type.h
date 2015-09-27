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

#ifndef NIMBLE_EXECUTOR_TYPE_H_
#define NIMBLE_EXECUTOR_TYPE_H_

namespace NIMBLE {

	namespace LANGUAGE {

		#define NIMBLE_EXECUTOR_HEADER "Executor"

		#ifndef NDEBUG
		#define NIMBLE_EXECUTOR_EXCEPTION_HEADER NIMBLE_EXECUTOR_HEADER
		#else
		#define NIMBLE_EXECUTOR_EXCEPTION_HEADER EXCEPTION_HEADER
		#endif // NDEBUG

		enum {
			NIMBLE_EXECUTOR_EXCEPTION_EXPECTING_ARGUMENT = 0,
			NIMBLE_EXECUTOR_EXCEPTION_EXPECTING_ASSIGNMENT,
			NIMBLE_EXECUTOR_EXCEPTION_EXPECTING_CALL,
			NIMBLE_EXECUTOR_EXCEPTION_EXPECTING_CALL_LIST,
			NIMBLE_EXECUTOR_EXCEPTION_EXPECTING_COMMAND,
			NIMBLE_EXECUTOR_EXCEPTION_EXPECTING_LITERAL,
			NIMBLE_EXECUTOR_EXCEPTION_EXPECTING_STATEMENT,
			NIMBLE_EXECUTOR_EXCEPTION_INVALID_CHILD,
			NIMBLE_EXECUTOR_EXCEPTION_INVALID_PARENT,
			NIMBLE_EXECUTOR_EXCEPTION_MALFORMED_STATEMENT,
			NIMBLE_EXECUTOR_EXCEPTION_UNDEFINED_ARGUMENT,
		};

		#define NIMBLE_EXECUTOR_EXCEPTION_MAX NIMBLE_EXECUTOR_EXCEPTION_UNDEFINED_ARGUMENT

		static const std::string NIMBLE_EXECUTOR_EXCEPTION_STR[] = {
			"Expecting argument",
			"Expecting assignment",
			"Expecting call",
			"Expecting call list",
			"Expecting command",
			"Expecting literal",
			"Expecting statement",
			"Invalid child node position",
			"Invalid parent node position",
			"Malformed statement",
			"Undefined argument",
			};

		#define NIMBLE_EXECUTOR_EXCEPTION_STRING(_TYPE_) \
			((_TYPE_) > NIMBLE_EXECUTOR_EXCEPTION_MAX ? UNKNOWN_EXCEPTION : \
			CHK_STR(NIMBLE_EXECUTOR_EXCEPTION_STR[_TYPE_]))

		#define THROW_NIMBLE_EXECUTOR_EXCEPTION(_EXCEPT_) \
			THROW_EXCEPTION(NIMBLE_EXECUTOR_EXCEPTION_HEADER, \
			NIMBLE_EXECUTOR_EXCEPTION_STRING(_EXCEPT_))
		#define THROW_NIMBLE_EXECUTOR_EXCEPTION_MESSAGE(_EXCEPT_, _FORMAT_, ...) \
			THROW_EXCEPTION_MESSAGE(NIMBLE_EXECUTOR_EXCEPTION_HEADER, \
			NIMBLE_EXECUTOR_EXCEPTION_STRING(_EXCEPT_), _FORMAT_, __VA_ARGS__)

		class _nimble_executor;
		typedef _nimble_executor nimble_executor, *nimble_executor_ptr;
	}
}

#endif // NIMBLE_EXECUTOR_TYPE_H_
