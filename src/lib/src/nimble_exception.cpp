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

#include <cstdarg>
#include "../include/nimble.h"

namespace NIMBLE {

	enum {
		NIMBLE_EXCEPTION_EXCEPTION_EMPTY = 0,
		NIMBLE_EXCEPTION_EXCEPTION_MALFORMED,
	};

	#define NIMBLE_EXCEPTION_EXCEPTION_MAX NIMBLE_EXCEPTION_EXCEPTION_MALFORMED

	static const std::string NIMBLE_EXCEPTION_EXCEPTION_STR[] = {
		"Empty exception",
		"Malformed exception",
		};

	#define NIMBLE_EXCEPTION_EXCEPTION_STRING(_TYPE_) \
		((_TYPE_) > NIMBLE_EXCEPTION_EXCEPTION_MAX ? UNKNOWN_EXCEPTION : \
		CHK_STR(NIMBLE_EXCEPTION_EXCEPTION_STR[_TYPE_]))

	_nimble_exception::_nimble_exception(
		__in_opt const std::string &message,
		__in_opt const std::string &source,
		__in_opt size_t line
		) :
			std::runtime_error(CHK_STR(message)),
			m_line(line),
			m_source(source)
	{
		return;
	}

	_nimble_exception::_nimble_exception(
		__in const _nimble_exception &other
		) :
			std::runtime_error(other),
			m_line(other.m_line),
			m_source(other.m_source)
	{
		return;
	}

	_nimble_exception::~_nimble_exception(void)
	{
		return;
	}

	_nimble_exception &
	_nimble_exception::operator=(
		__in const _nimble_exception &other
		)
	{
		SERIALIZE_CALL_RECUR(m_lock);

		if(this != &other) {
			std::runtime_error::operator=(other);
			m_line = other.m_line;
			m_source = other.m_source;
		}

		return *this;
	}

	void 
	_nimble_exception::generate(
		__in const std::string &message,
		__in const std::string &source,
		__in size_t line,
		__in const char *format,
		...
		)
	{
		std::string buf;
		std::stringstream result;

		if(!message.empty()) {
			result << message;
		}

		if(format) {
			va_list lst;
			va_start(lst, format);

			int len = vsnprintf(NULL, 0, format, lst);
			if(len < 0) {
				buf = NIMBLE_EXCEPTION_EXCEPTION_STRING(
					NIMBLE_EXCEPTION_EXCEPTION_MALFORMED);
			} else if(!len) {
				buf = NIMBLE_EXCEPTION_EXCEPTION_STRING(
					NIMBLE_EXCEPTION_EXCEPTION_EMPTY);
			} else {
				va_end(lst);
				va_start(lst, format);
				buf.resize(++len, '\0');

				len = vsnprintf((char *) &buf[0], len, format, lst);
				if(len < 0) {
					buf = NIMBLE_EXCEPTION_EXCEPTION_STRING(
						NIMBLE_EXCEPTION_EXCEPTION_MALFORMED);
				} else if(!len) {
					buf = NIMBLE_EXCEPTION_EXCEPTION_STRING(
						NIMBLE_EXCEPTION_EXCEPTION_EMPTY);
				}
			}

			if(!buf.empty()) {
			
				if(!message.empty()) {
					result << ": ";
				}

				result << CHK_STR(buf);
			}

			va_end(lst);
		}

		throw nimble_exception(CHK_STR(result.str()), source, line);
	}

	size_t &
	_nimble_exception::line(void)
	{
		SERIALIZE_CALL_RECUR(m_lock);
		return m_line;
	}

	std::string &
	_nimble_exception::source(void)
	{
		SERIALIZE_CALL_RECUR(m_lock);
		return m_source;
	}

	std::string 
	_nimble_exception::to_string(
		__in_opt bool verbose
		)
	{
		std::stringstream result;

		SERIALIZE_CALL_RECUR(m_lock);

		if(verbose) {
			result << "(";

			if(!m_source.empty()) {
				result << m_source << ":";
			}

			result << m_line << ") ";
		}	

		result << what();

		return CHK_STR(result.str());
	}
}
