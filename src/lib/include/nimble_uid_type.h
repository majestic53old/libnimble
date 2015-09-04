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

#ifndef NIMBLE_UID_TYPE_H_
#define NIMBLE_UID_TYPE_H_

namespace NIMBLE {

	namespace COMPONENT {

		#define NIMBLE_UID_HEADER "(UID)"

		#ifndef NDEBUG
		#define NIMBLE_UID_EXCEPTION_HEADER NIMBLE_UID_HEADER
		#else
		#define NIMBLE_UID_EXCEPTION_HEADER EXCEPTION_HEADER
		#endif // NDEBUG

		enum {
			NIMBLE_UID_EXCEPTION_ALLOCATION = 0,
			NIMBLE_UID_EXCEPTION_INITIALIZED,
			NIMBLE_UID_EXCEPTION_UNINITIALIZED,
		};

		#define NIMBLE_UID_EXCEPTION_MAX NIMBLE_UID_EXCEPTION_UNINITIALIZED

		static const std::string NIMBLE_UID_EXCEPTION_STR[] = {
			NIMBLE_UID_EXCEPTION_HEADER " Failed to allocate Uid component",
			NIMBLE_UID_EXCEPTION_HEADER " Uid component is initialized",
			NIMBLE_UID_EXCEPTION_HEADER " Uid component is uninitialized",
			};

		#define NIMBLE_UID_EXCEPTION_STRING(_TYPE_) \
			((_TYPE_) > NIMBLE_UID_EXCEPTION_MAX ? UNKNOWN_EXCEPTION : \
			CHK_STR(NIMBLE_UID_EXCEPTION_STR[_TYPE_]))

		#define THROW_NIMBLE_UID_EXCEPTION(_EXCEPT_) \
			THROW_EXCEPTION(NIMBLE_UID_EXCEPTION_STRING(_EXCEPT_))
		#define THROW_NIMBLE_UID_EXCEPTION_MESSAGE(_EXCEPT_, _FORMAT_, ...) \
			THROW_EXCEPTION_MESSAGE(NIMBLE_UID_EXCEPTION_STRING(_EXCEPT_), \
			_FORMAT_, __VA_ARGS__)

		class _nimble_uid;
		typedef _nimble_uid nimble_uid, *nimble_uid_ptr;

		class _nimble_uid_class;
		typedef _nimble_uid_class nimble_uid_class, *nimble_uid_class_ptr;

		class _nimble_factory;
		typedef _nimble_factory nimble_factory, *nimble_factory_ptr;
	}
}

#endif // NIMBLE_UID_TYPE_H_
