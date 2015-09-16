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

#ifndef NIMBLE_TRACE_H_
#define NIMBLE_TRACE_H_

namespace NIMBLE {

	#define TRACE_HEADER_ENTRY "+"
	#define TRACE_HEADER_EXIT "-"

	typedef enum {
		TRACE_NONE = 0,
		TRACE_ERROR,
		TRACE_WARNING,
		TRACE_INFORMATION,
		TRACE_VERBOSE,
	} nimble_lvl_t;

	#define NIMBLE_LEVEL_MAX TRACE_VERBOSE

	#ifndef TLEVEL
	#define TLEVEL TRACE_VERBOSE
	#endif // TLEVEL

	#ifndef NDEBUG
	#define _TRACE(_LEVEL_, _MESSAGE_) \
		nimble_trace::generate(_LEVEL_, _MESSAGE_, __FILE__, __LINE__, \
		NULL)
	#define _TRACE_MESSAGE(_LEVEL_, _MESSAGE_, _FORMAT_, ...) \
		nimble_trace::generate(_LEVEL_, _MESSAGE_, __FILE__, __LINE__, \
		_FORMAT_, __VA_ARGS__)
	#define TRACE_ENTRY(_LEVEL_) _TRACE(_LEVEL_, TRACE_HEADER_ENTRY)
	#define TRACE_ENTRY_MESSAGE(_LEVEL_, _FORMAT_, ...) \
		_TRACE_MESSAGE(_LEVEL_, TRACE_HEADER_ENTRY, _FORMAT_, \
		__VA_ARGS__)
	#define TRACE_EXIT(_LEVEL_) _TRACE(_LEVEL_, TRACE_HEADER_EXIT)
	#define TRACE_EXIT_MESSAGE(_LEVEL_, _FORMAT_, ...) \
		_TRACE_MESSAGE(_LEVEL_, TRACE_HEADER_EXIT, _FORMAT_, \
		__VA_ARGS__)
	#define TRACE_MESSAGE(_LEVEL_, _FORMAT_, ...) \
		_TRACE_MESSAGE(_LEVEL_, std::string(), _FORMAT_, \
		__VA_ARGS__)
	#else
	#define _TRACE(_LEVEL_, _MESSAGE_)
	#define _TRACE_MESSAGE(_LEVEL_, _MESSAGE_, _FORMAT_, ...)
	#define TRACE_ENTRY(_LEVEL_)
	#define TRACE_ENTRY_MESSAGE(_LEVEL_, _FORMAT_, ...)
	#define TRACE_EXIT(_LEVEL_)
	#define TRACE_EXIT_MESSAGE(_LEVEL_, _FORMAT_, ...)
	#define TRACE_MESSAGE(_LEVEL_, _FORMAT_, ...)
	#endif

	typedef class _nimble_trace {

		public:

			static void generate(
				__in nimble_lvl_t level,
				__in const std::string &message,
				__in const std::string &source,
				__in size_t line,
				__in const char *format,
				...
				);

	} nimble_trace, *nimble_trace_ptr;
}

#endif // NIMBLE_TRACE_H_
