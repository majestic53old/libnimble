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

#include "../include/nimble.h"
#include "../include/nimble_node_type.h"

namespace NIMBLE {

	namespace COMPONENT {

		_nimble_node::_nimble_node(
			__in_opt const nimble_uid &token,
			__in_opt size_t parent
			) :
				m_parent(parent),
				m_token(token)
		{
			return;
		}

		_nimble_node::_nimble_node(
			__in const _nimble_node &other
			) :
				nimble_uid_class(other),
				m_children(other.m_children),
				m_parent(other.m_parent),
				m_token(other.m_token)
		{
			return;
		}

		_nimble_node::~_nimble_node(void)
		{
			return;
		}

		_nimble_node &
		_nimble_node::operator=(
			__in const _nimble_node &other
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			if(this != &other) {
				nimble_uid_class::operator=(other);
				m_children = other.m_children;
				m_parent = other.m_parent;
				m_token = other.m_token;
			}

			return *this;
		}

		nimble_token_factory_ptr 
		_nimble_node::acquire_token(void)
		{
			return nimble_token_factory::is_allocated() 
				? nimble_token_factory::acquire() : NULL;
		}

		std::string 
		_nimble_node::as_string(
			__in const _nimble_node &node,
			__in_opt bool verbose
			)
		{
			std::stringstream result;
			nimble_token_factory_ptr fact = NULL;

			if(verbose) {
				result << nimble_uid::as_string(node.m_uid, true) << " ";
			}

			result << "[";

			if(node.m_parent == PAR_INVALID) {
				result << "ROOT";

				if(node.m_children.size()) {
					result << ", LEAF";
				}
			} else if(node.m_children.size()) {
				result << "LEAF";
			}

			result << "]";

			if(nimble_token_factory::is_allocated()) {
				fact = nimble_token_factory::acquire();
			}

			if(fact && fact->contains(node.m_token)) {
				result << fact->at(node.m_token).to_string(verbose);
			} else {
				result << nimble_uid::as_string(node.m_token, true);
			}

			if(verbose) {
				result << ", par. " << node.m_parent 
					<< ", chd. " << node.m_children.size();
			}

			return CHK_STR(result.str());
		}

		std::vector<size_t> &
		_nimble_node::children(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return m_children;
		}

		void 
		_nimble_node::clear(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			m_children.clear();
			m_parent = PAR_INVALID;
			m_token = UID_INVALID;
		}

		bool 
		_nimble_node::is_leaf(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return m_children.empty();
		}

		bool 
		_nimble_node::is_root(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return (m_parent == PAR_INVALID);
		}

		size_t &
		_nimble_node::parent(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return m_parent;
		}

		std::string 
		_nimble_node::to_string(
			__in_opt bool verbose
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return nimble_node::as_string(*this, verbose);
		}

		nimble_uid &
		_nimble_node::token(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return m_token;
		}

		// TODO: implement node factory
	}
}
