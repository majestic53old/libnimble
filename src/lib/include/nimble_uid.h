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

#include <map>
#include <set>

namespace NIMBLE {

	namespace COMPONENT {

		#define UID_INITIAL (UID_INVALID + 1)
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

				static std::string as_string(
					__in const _nimble_uid &uid,
					__in_opt bool verbose = false
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

		typedef class _nimble_uid_factory {

			public:

				~_nimble_uid_factory(void);

				static _nimble_uid_factory *acquire(void);

				bool contains(
					__in const nimble_uid &uid
					);

				size_t decrement_reference(
					__in const nimble_uid &uid
					);

				size_t generate(
					__out nimble_uid &uid
					);

				size_t increment_reference(
					__in const nimble_uid &uid
					);

				void initialize(void);

				static bool is_allocated(void);

				bool is_initialized(void);

				size_t reference_count(
					__in const nimble_uid &uid
					);

				size_t size(void);

				std::string to_string(
					__in_opt bool verbose = false
					);

				void uninitialize(void);

			protected:

				_nimble_uid_factory(void);

				_nimble_uid_factory(
					__in const _nimble_uid_factory &other
					);

				_nimble_uid_factory &operator=(
					__in const _nimble_uid_factory &other
					);

				static void _delete(void);

				std::map<nimble_uid, size_t>::iterator find(
					__in const nimble_uid &uid
					);

				bool m_initialized;

				static _nimble_uid_factory *m_instance;

				std::map<nimble_uid, size_t> m_map;

				nimble_uid m_next;

				std::set<nimble_uid> m_surplus;

			private:

				std::recursive_mutex m_lock;

		} nimble_uid_factory, *nimble_uid_factory_ptr;

		typedef class _nimble_uid_class :
				public nimble_uid {

			public:

				_nimble_uid_class(void);

				_nimble_uid_class(
					__in const _nimble_uid_class &other
					);

				virtual ~_nimble_uid_class(void);

				_nimble_uid_class &operator=(
					__in const _nimble_uid_class &other
					);

			protected:

				nimble_uid_factory_ptr factory(void);

			private:

				std::recursive_mutex m_lock;

		} nimble_uid_class, *nimble_uid_class_ptr;
	}
}

#endif // NIMBLE_UID_H_
