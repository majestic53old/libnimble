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

#ifndef NIMBLE_NODE_TYPE_H_
#define NIMBLE_NODE_TYPE_H_

namespace NIMBLE {

	namespace COMPONENT {

		#define NIMBLE_NODE_HEADER "Node"

		#ifndef NDEBUG
		#define NIMBLE_NODE_EXCEPTION_HEADER NIMBLE_NODE_HEADER
		#else
		#define NIMBLE_NODE_EXCEPTION_HEADER EXCEPTION_HEADER
		#endif // NDEBUG

		enum {
			NIMBLE_NODE_EXCEPTION_ALLOCATED = 0,
			NIMBLE_NODE_EXCEPTION_INITIALIZED,
			NIMBLE_NODE_EXCEPTION_NOT_FOUND,
			NIMBLE_NODE_EXCEPTION_UNINITIALIZED,
		};

		#define NIMBLE_NODE_EXCEPTION_MAX NIMBLE_NODE_EXCEPTION_UNINITIALIZED

		static const std::string NIMBLE_NODE_EXCEPTION_STR[] = {
			"Failed to allocate node component",
			"Node component is initialized",
			"Node does not exist",
			"Node component is uninitialized",
			};

		#define NIMBLE_NODE_EXCEPTION_STRING(_TYPE_) \
			((_TYPE_) > NIMBLE_NODE_EXCEPTION_MAX ? UNKNOWN_EXCEPTION : \
			CHK_STR(NIMBLE_NODE_EXCEPTION_STR[_TYPE_]))

		#define THROW_NIMBLE_NODE_EXCEPTION(_EXCEPT_) \
			THROW_EXCEPTION(NIMBLE_NODE_EXCEPTION_HEADER, \
			NIMBLE_NODE_EXCEPTION_STRING(_EXCEPT_))
		#define THROW_NIMBLE_NODE_EXCEPTION_MESSAGE(_EXCEPT_, _FORMAT_, ...) \
			THROW_EXCEPTION_MESSAGE(NIMBLE_NODE_EXCEPTION_HEADER, \
			NIMBLE_NODE_EXCEPTION_STRING(_EXCEPT_), _FORMAT_, __VA_ARGS__)

		class _nimble_node;
		typedef _nimble_node nimble_node, *nimble_node_ptr;

		class _nimble_node_factory;
		typedef _nimble_node_factory nimble_node_factory, *nimble_node_factory_ptr;
	}
}

#endif // NIMBLE_NODE_TYPE_H_
