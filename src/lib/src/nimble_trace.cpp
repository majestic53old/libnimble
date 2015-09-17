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

#include <ctime>
#include <cstdarg>
#include "../include/nimble.h"

namespace NIMBLE {

	enum {
		NIMBLE_TRACE_EXCEPTION_EMPTY = 0,
		NIMBLE_TRACE_EXCEPTION_MALFORMED,
	};

	#define NIMBLE_TRACE_EXCEPTION_MAX NIMBLE_TRACE_EXCEPTION_MALFORMED

	static const std::string NIMBLE_TRACE_EXCEPTION_STR[] = {
		"Empty exception",
		"Malformed exception",
		};

	#define NIMBLE_TRACE_EXCEPTION_STRING(_TYPE_) \
		((_TYPE_) > NIMBLE_TRACE_EXCEPTION_MAX ? UNKNOWN : \
		CHK_STR(NIMBLE_TRACE_EXCEPTION_STR[_TYPE_]))

	static const std::string NIMBLE_TRACE_LEVEL_STR[] = {
		"", "err", "warn", "info", "verb",
		};

	#define NIMBLE_TRACE_LEVEL_STRING(_TYPE_) \
		CHK_STR(NIMBLE_TRACE_LEVEL_STR[((_TYPE_) > NIMBLE_LEVEL_MAX ? 0 : \
		(_TYPE_))])
		
	bool _nimble_trace::m_started = false;

	std::ofstream _nimble_trace::m_stream;

	void 
	_nimble_trace::generate(
		__in nimble_lvl_t level,
		__in const std::string &header,
		__in const std::string &funct,
		__in const std::string &source,
		__in size_t line,
		__in const char *format,
		...
		)
	{
		std::time_t tm;
		std::string buf, tmstr;
		std::stringstream result;

		if(nimble_trace::m_started) {
			tm = std::time(NULL);
			tmstr = std::asctime(std::localtime(&tm));
			result << "[" << tmstr.substr(0, tmstr.size() - 1) << "] ";

			if(!header.empty()) {
				result << CHK_STR(header);
			}

			if(!funct.empty()) {
				result << CHK_STR(funct);
			}

			if(format) {
				va_list lst;
				va_start(lst, format);

				int len = vsnprintf(NULL, 0, format, lst);
				if(len < 0) {
					buf = NIMBLE_TRACE_EXCEPTION_STRING(
						NIMBLE_TRACE_EXCEPTION_MALFORMED);
				} else if(!len) {
					buf = NIMBLE_TRACE_EXCEPTION_STRING(
						NIMBLE_TRACE_EXCEPTION_EMPTY);
				} else {
					va_end(lst);
					va_start(lst, format);
					buf.resize(++len, '\0');

					len = vsnprintf((char *) &buf[0], len, format, lst);
					if(len < 0) {
						buf = NIMBLE_TRACE_EXCEPTION_STRING(
							NIMBLE_TRACE_EXCEPTION_MALFORMED);
					} else if(!len) {
						buf = NIMBLE_TRACE_EXCEPTION_STRING(
							NIMBLE_TRACE_EXCEPTION_EMPTY);
					}
				}

				va_end(lst);

				if(!buf.empty()) {
					result << ": " << CHK_STR(buf);
				}
			}

			result << " (";

			if(!source.empty()) {
				result << source << ":";
			}

			result << line << ")";

			switch(level) {
				case TRACE_ERROR:
					SET_TERM_ATTRIB(nimble_trace::m_stream, 1, COL_FORE_RED);
					nimble_trace::m_stream << CHK_STR(result.str()) << std::endl;
					CLEAR_TERM_ATTRIB(nimble_trace::m_stream);
					break;
				case TRACE_WARNING:
					SET_TERM_ATTRIB(nimble_trace::m_stream, 1, COL_FORE_YELLOW);
					nimble_trace::m_stream << CHK_STR(result.str()) << std::endl;
					CLEAR_TERM_ATTRIB(nimble_trace::m_stream);
					break;
				default:
					nimble_trace::m_stream << CHK_STR(result.str()) << std::endl;
					break;
			}
		}
	}

	bool 
	_nimble_trace::is_started(void)
	{
		return (nimble_trace::m_started);
	}

	void 
	_nimble_trace::start(
		__in nimble_lvl_t level,
		__in const std::string &output
		)
	{
		std::time_t tm;
		std::stringstream path;

		if(!nimble_trace::m_started) {
			std::time(&tm);
			path << output << "_" << NIMBLE_TRACE_LEVEL_STRING(level) 
				<< "_" << tm;
			nimble_trace::m_stream = std::ofstream(path.str().c_str(), 
				std::ios::out | std::ios::trunc);

			if(nimble_trace::m_stream) {
				nimble_trace::m_started = true;
			}
		}
	}

	void 
	_nimble_trace::stop(void)
	{

		if(nimble_trace::m_started) {

			if(nimble_trace::m_stream) {
				nimble_trace::m_stream.close();
			}

			nimble_trace::m_started = false;
		}
	}
}
