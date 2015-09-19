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

#include <fstream>

namespace NIMBLE {

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

	#ifndef TLOGOUT
	#define TLOGOUT "./log"
	#endif // TLOGOUT

	#ifndef NDEBUG
	#define TRACE_HEADER_ENTRY "+"
	#define TRACE_HEADER_EXIT "-"
	#define _TRACE(_LEVEL_, _HEADER_) {\
		if(((TLEVEL) > TRACE_NONE) && ((TLEVEL) >= (_LEVEL_))) { \
		nimble_trace::generate(_LEVEL_, _HEADER_, __FUNCTION__, \
		__FILE__, __LINE__, NULL); \
		} \
		}
	#define _TRACE_MESSAGE(_LEVEL_, _HEADER_, _FORMAT_, ...) { \
		if(((TLEVEL) > TRACE_NONE) && ((TLEVEL) >= (_LEVEL_))) { \
		nimble_trace::generate(_LEVEL_, _HEADER_, __FUNCTION__, \
		__FILE__, __LINE__, _FORMAT_, __VA_ARGS__); \
		} \
		}
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
	#define TRACE_START() nimble_trace::start(TLEVEL, TLOGOUT)
	#define TRACE_STARTED() nimble_trace::is_started()
	#define TRACE_STOP() nimble_trace::stop()
	#else
	#define _TRACE(_LEVEL_, _HEADER_)
	#define _TRACE_MESSAGE(_LEVEL_, _HEADER_, _FORMAT_, ...)
	#define TRACE_ENTRY(_LEVEL_)
	#define TRACE_ENTRY_MESSAGE(_LEVEL_, _FORMAT_, ...)
	#define TRACE_EXIT(_LEVEL_)
	#define TRACE_EXIT_MESSAGE(_LEVEL_, _FORMAT_, ...)
	#define TRACE_MESSAGE(_LEVEL_, _FORMAT_, ...)
	#define TRACE_START()
	#define TRACE_STARTED()
	#define TRACE_STOP()
	#endif

	typedef class _nimble_trace {

		public:

			static void generate(
				__in nimble_lvl_t level,
				__in const std::string &header,
				__in const std::string &funct,
				__in const std::string &source,
				__in size_t line,
				__in const char *format,
				...
				);

			static bool is_started(void);

			static void start(
				__in nimble_lvl_t level,
				__in const std::string &output
				);

			static void stop(void);

		protected:

			static std::mutex m_lock;

			static bool m_started;

			static std::ofstream m_stream;

	} nimble_trace, *nimble_trace_ptr;
}

#endif // NIMBLE_TRACE_H_
