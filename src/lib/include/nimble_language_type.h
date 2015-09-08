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

#ifndef NIMBLE_LANGUAGE_TYPE_H_
#define NIMBLE_LANGUAGE_TYPE_H_

namespace NIMBLE {

	#define NIMBLE_LANGUAGE_HEADER "Language"

	#ifndef NDEBUG
	#define NIMBLE_LANGUAGE_EXCEPTION_HEADER NIMBLE_LANGUAGE_HEADER
	#else
	#define NIMBLE_LANGUAGE_EXCEPTION_HEADER EXCEPTION_HEADER
	#endif // NDEBUG

	enum {
		NIMBLE_LANGUAGE_EXCEPTION_INVALID_BASE,
		NIMBLE_LANGUAGE_EXCEPTION_INVALID_DIGIT,
		NIMBLE_LANGUAGE_EXCEPTION_MALFORMED_VALUE,
	};

	#define NIMBLE_LANGUAGE_EXCEPTION_MAX NIMBLE_LANGUAGE_EXCEPTION_MALFORMED_VALUE

	static const std::string NIMBLE_LANGUAGE_EXCEPTION_STR[] = {
		"Invalid base",
		"Invalid digit",
		"Malformed value",
		};

	#define NIMBLE_LANGUAGE_EXCEPTION_STRING(_TYPE_) \
		((_TYPE_) > NIMBLE_LANGUAGE_EXCEPTION_MAX ? UNKNOWN_EXCEPTION : \
		CHK_STR(NIMBLE_LANGUAGE_EXCEPTION_STR[_TYPE_]))

	#define THROW_NIMBLE_LANGUAGE_EXCEPTION(_EXCEPT_) \
		THROW_EXCEPTION(NIMBLE_LANGUAGE_EXCEPTION_HEADER, \
		NIMBLE_LANGUAGE_EXCEPTION_STRING(_EXCEPT_))
	#define THROW_NIMBLE_LANGUAGE_EXCEPTION_MESSAGE(_EXCEPT_, _FORMAT_, ...) \
		THROW_EXCEPTION_MESSAGE(NIMBLE_LANGUAGE_EXCEPTION_HEADER, \
		NIMBLE_LANGUAGE_EXCEPTION_STRING(_EXCEPT_), _FORMAT_, __VA_ARGS__)

	class _nimble_language;
	typedef _nimble_language nimble_language, *nimble_language_ptr;
}

#endif // NIMBLE_LANGUAGE_TYPE_H_
