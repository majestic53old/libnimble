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

#ifndef NIMBLE_DEFINES_H_
#define NIMBLE_DEFINES_H_

#include <thread>
#include <cstdint>
#include <cstdbool>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

namespace NIMBLE {

	#define __in
	#define __in_opt
	#define __out
	#define __out_opt
	#define __inout
	#define __inout_opt

	#define EMPTY "(EMPTY)"
	#define UNKNOWN "(UNKNOWN)"

	#define REF_INITIAL 1

	#define VER_MAJ 0
	#define VER_MIN 1
	#define VER_REV 3
	#define VER_WEEK 1537

	#define ARR_LEN(_TYPE_, _ARR_) \
		sizeof(_ARR_) / sizeof(_TYPE_)

	#define _CAT_STR(_STR_) # _STR_
	#define CAT_STR(_STR_) _CAT_STR(_STR_)

	#define CHK_STR(_STR_) (_STR_.empty() ? EMPTY : _STR_.c_str())

	#define INVALID(_TYPE_) ((_TYPE_) -1)

	#define REF_PARAM(_PARAM_) (void) _PARAM_

	#define _SERIALIZE_CALL(_TYPE_, _MUTEX_) \
		std::lock_guard<_TYPE_> __LOCK ## _MUTEX_(_MUTEX_)
	#define SERIALIZE_CALL(_MUTEX_) \
		_SERIALIZE_CALL(std::mutex, _MUTEX_)
	#define SERIALIZE_CALL_RECUR(_MUTEX_) \
		_SERIALIZE_CALL(std::recursive_mutex, _MUTEX_)

	#define VAL_AS_HEX(_TYPE_, _VAL_) \
		std::setw(sizeof(_TYPE_) * 2) << std::setfill('0') << std::hex \
		<< (uintmax_t) ((_TYPE_) (_VAL_)) << std::dec

	#define VER_STR \
		CAT_STR(VER_MAJ) "." CAT_STR(VER_MIN) "." \
		CAT_STR(VER_WEEK) "." CAT_STR(VER_REV)
}

#endif // NIMBLE_DEFINES_H_
