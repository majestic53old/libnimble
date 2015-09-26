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

#ifndef NIMBLE_ENVIRONMENT_TYPE_H_
#define NIMBLE_ENVIRONMENT_TYPE_H_

namespace NIMBLE {

	#define NIMBLE_ENVIRONMENT_HEADER "Environment"

	#ifndef NDEBUG
	#define NIMBLE_ENVIRONMENT_EXCEPTION_HEADER NIMBLE_ENVIRONMENT_HEADER
	#else
	#define NIMBLE_ENVIRONMENT_EXCEPTION_HEADER EXCEPTION_HEADER
	#endif // NDEBUG

	enum {
		NIMBLE_ENVIRONMENT_EXCEPTION_INVALID = 0,
		NIMBLE_ENVIRONMENT_EXCEPTION_NOT_FOUND,
		NIMBLE_ENVIRONMENT_EXCEPTION_RESOURCES,
	};

	#define NIMBLE_ENVIRONMENT_EXCEPTION_MAX NIMBLE_ENVIRONMENT_EXCEPTION_RESOURCES

	static const std::string NIMBLE_ENVIRONMENT_EXCEPTION_STR[] = {
		"Invalid environment",
		"Environment entry does not exist",
		"Environment is full",
		};

	#define NIMBLE_ENVIRONMENT_EXCEPTION_STRING(_TYPE_) \
		((_TYPE_) > NIMBLE_ENVIRONMENT_EXCEPTION_MAX ? UNKNOWN_EXCEPTION : \
		CHK_STR(NIMBLE_ENVIRONMENT_EXCEPTION_STR[_TYPE_]))

	#define THROW_NIMBLE_ENVIRONMENT_EXCEPTION(_EXCEPT_) \
		THROW_EXCEPTION(NIMBLE_ENVIRONMENT_EXCEPTION_HEADER, \
		NIMBLE_ENVIRONMENT_EXCEPTION_STRING(_EXCEPT_))
	#define THROW_NIMBLE_ENVIRONMENT_EXCEPTION_MESSAGE(_EXCEPT_, _FORMAT_, ...) \
		THROW_EXCEPTION_MESSAGE(NIMBLE_ENVIRONMENT_EXCEPTION_HEADER, \
		NIMBLE_ENVIRONMENT_EXCEPTION_STRING(_EXCEPT_), _FORMAT_, __VA_ARGS__)

	class _nimble_environment;
	typedef _nimble_environment nimble_environment, *nimble_environment_ptr;
}

#endif // NIMBLE_ENVIRONMENT_TYPE_H_
