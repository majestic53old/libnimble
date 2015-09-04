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

#ifndef NIMBLE_H_
#define NIMBLE_H_

//#define NDEBUG

#ifndef NIMBLE
#define NIMBLE nb
#endif // NIMBLE

#include "nimble_defines.h"
#include "nimble_exception.h"

using namespace NIMBLE;

#ifndef COMPONENT
#define COMPONENT comp
#endif // COMPONENT

#include "nimble_uid.h"

using namespace NIMBLE::COMPONENT;

namespace NIMBLE {

	typedef class _nimble {

		public:

			~_nimble(void);

			static _nimble *acquire(void);

			void initialize(void);

			static bool is_allocated(void);

			bool is_initialized(void);

			std::string to_string(
				__in_opt bool verbose = false
				);

			void uninitialize(void);

			static std::string version(void);

		protected:

			_nimble(void);

			_nimble(
				__in const _nimble &other
				);

			_nimble &operator=(
				__in const _nimble &other
				);

			static void _delete(void);

			bool m_initialized;

			static _nimble *m_instance;

		private:

			std::recursive_mutex m_lock;

	} nimble, *nimble_ptr;
}

#endif // NIMBLE_H_
