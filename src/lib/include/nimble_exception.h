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

#ifndef NIMBLE_EXCEPTION_H_
#define NIMBLE_EXCEPTION_H_

#include <stdexcept>

namespace NIMBLE {

	#define EXCEPTION_HEADER "Exception"

	#define UNKNOWN_EXCEPTION "Unknown exception"

	#define THROW_EXCEPTION(_HEAD_, _EXCEPT_) \
		nimble_exception::generate(_HEAD_, _EXCEPT_, __FILE__, __LINE__, \
		NULL)

	#define THROW_EXCEPTION_MESSAGE(_HEAD_, _EXCEPT_, _FORMAT_, ...) \
		nimble_exception::generate(_HEAD_, _EXCEPT_, __FILE__, __LINE__, \
		_FORMAT_, __VA_ARGS__)

	typedef class _nimble_exception :
			public std::runtime_error {

		public:

			_nimble_exception(
				__in_opt const std::string &message = std::string(),
				__in_opt const std::string &source = std::string(),
				__in_opt size_t line = 0
				);

			_nimble_exception(
				__in const _nimble_exception &other
				);

			virtual ~_nimble_exception(void);

			_nimble_exception &operator=(
				__in const _nimble_exception &other
				);

			static void generate(
				__in const std::string &header,
				__in const std::string &message,
				__in const std::string &source,
				__in size_t line,
				__in const char *format,
				...
				);

			size_t &line(void);

			std::string &source(void);

			virtual std::string to_string(
				__in_opt bool verbose = false
				);

		protected:

			size_t m_line;

			std::string m_source;

		private:

			std::recursive_mutex m_lock;

	} nimble_exception, *nimble_exception_ptr;
}

#endif // NIMBLE_EXCEPTION_H_
