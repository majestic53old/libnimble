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

#ifndef NIMBLE_TOKEN_TYPE_H_
#define NIMBLE_TOKEN_TYPE_H_

namespace NIMBLE {

	namespace COMPONENT {

		#define NIMBLE_TOKEN_HEADER "Token"

		#ifndef NDEBUG
		#define NIMBLE_TOKEN_EXCEPTION_HEADER NIMBLE_TOKEN_HEADER
		#else
		#define NIMBLE_TOKEN_EXCEPTION_HEADER EXCEPTION_HEADER
		#endif // NDEBUG

		enum {
			NIMBLE_TOKEN_EXCEPTION_ALLOCATED = 0,
			NIMBLE_TOKEN_EXCEPTION_INITIALIZED,
			NIMBLE_TOKEN_EXCEPTION_NOT_FOUND,
			NIMBLE_TOKEN_EXCEPTION_UNINITIALIZED,
		};

		#define NIMBLE_TOKEN_EXCEPTION_MAX NIMBLE_TOKEN_EXCEPTION_UNINITIALIZED

		static const std::string NIMBLE_TOKEN_EXCEPTION_STR[] = {
			"Failed to allocate token component",
			"Token component is initialized",
			"Token does not exist",
			"Token component is uninitialized",
			};

		#define NIMBLE_TOKEN_EXCEPTION_STRING(_TYPE_) \
			((_TYPE_) > NIMBLE_TOKEN_EXCEPTION_MAX ? UNKNOWN_EXCEPTION : \
			CHK_STR(NIMBLE_TOKEN_EXCEPTION_STR[_TYPE_]))

		#define THROW_NIMBLE_TOKEN_EXCEPTION(_EXCEPT_) \
			THROW_EXCEPTION(NIMBLE_TOKEN_HEADER, \
			NIMBLE_TOKEN_EXCEPTION_STRING(_EXCEPT_))
		#define THROW_NIMBLE_TOKEN_EXCEPTION_MESSAGE(_EXCEPT_, _FORMAT_, ...) \
			THROW_EXCEPTION_MESSAGE(NIMBLE_TOKEN_HEADER, \
			NIMBLE_TOKEN_EXCEPTION_STRING(_EXCEPT_), _FORMAT_, __VA_ARGS__)

		class _nimble_token;
		typedef _nimble_token nimble_token, *nimble_token_ptr;

		class _nimble_token_factory;
		typedef _nimble_token_factory nimble_token_factory, *nimble_token_factory_ptr;
	}
}

#endif // NIMBLE_TOKEN_TYPE_H_
