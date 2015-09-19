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

#ifndef NIMBLE_NODE_H_
#define NIMBLE_NODE_H_

#include <vector>

namespace NIMBLE {

	namespace COMPONENT {

		#define PAR_INVALID INVALID_TYPE(size_t)

		typedef class _nimble_node :
				public nimble_uid_class {

			public:

				_nimble_node(
					__in_opt const nimble_uid &token = UID_INVALID,
					__in_opt size_t parent = PAR_INVALID
					);

				_nimble_node(
					__in const _nimble_node &other
					);

				virtual ~_nimble_node(void);

				_nimble_node &operator=(
					__in const _nimble_node &other
					);

				static std::string as_string(
					__in const _nimble_node &node,
					__in_opt bool verbose = false
					);

				std::vector<size_t> &children(void);

				virtual void clear(void);

				bool is_leaf(void);

				bool is_root(void);

				size_t &parent(void);

				virtual void set(
					__in const nimble_uid &token
					);

				virtual std::string to_string(
					__in_opt bool verbose = false
					);

				nimble_uid token(void);

			protected:

				static nimble_token_factory_ptr acquire_token(void);

				std::vector<size_t> m_children;

				size_t m_parent;

				nimble_uid m_token;

			private:

				std::recursive_mutex m_lock;

		} nimble_node, *nimble_node_ptr;

		typedef class _nimble_node_factory {

			public:

				~_nimble_node_factory(void);

				static _nimble_node_factory *acquire(void);

				nimble_node &at(
					__in const nimble_uid &uid
					);

				bool contains(
					__in const nimble_uid &uid
					);

				size_t decrement_reference(
					__in const nimble_uid &uid
					);

				nimble_uid generate(void);

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

				_nimble_node_factory(void);

				_nimble_node_factory(
					__in const _nimble_node_factory &other
					);

				_nimble_node_factory &operator=(
					__in const _nimble_node_factory &other
					);

				static void _delete(void);

				std::map<nimble_uid, std::pair<nimble_node, size_t>>::iterator find(
					__in const nimble_uid &uid
					);

				bool m_initialized;

				static _nimble_node_factory *m_instance;

				std::map<nimble_uid, std::pair<nimble_node, size_t>> m_map;

			private:

				std::recursive_mutex m_lock;

		} nimble_node_factory, *nimble_node_factory_ptr;
	}
}

#endif // NIMBLE_NODE_H_
