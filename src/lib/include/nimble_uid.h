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

#ifndef NIMBLE_UID_H_
#define NIMBLE_UID_H_

namespace NIMBLE {

	namespace COMPONENT {

		#define UID_INVALID INVALID(nimble_uid_t)

		typedef uintmax_t nimble_uid_t;

		typedef class _nimble_uid {

			public:

				_nimble_uid(
					__in_opt nimble_uid_t uid = UID_INVALID
					);

				_nimble_uid(
					__in const _nimble_uid &other
					);

				virtual ~_nimble_uid(void);

				_nimble_uid &operator=(
					__in const _nimble_uid &other
					);

				bool operator==(
					__in const _nimble_uid &other
					);

				bool operator!=(
					__in const _nimble_uid &other
					);

				bool is_valid(void);

				virtual std::string to_string(
					__in_opt bool verbose = false
					);

				nimble_uid_t &uid(void);

			protected:

				friend bool operator<(
					__in const _nimble_uid &left,
					__in const _nimble_uid &right
					);

				nimble_uid_t m_uid;

			private:

				std::recursive_mutex m_lock;

		} nimble_uid, *nimble_uid_ptr;

		bool operator<(
			__in const nimble_uid &left,
			__in const nimble_uid &right
			);

		// TODO
	}
}

#endif // NIMBLE_UID_H_
