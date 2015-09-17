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

		nimble_node_factory_ptr nimble_node_factory::m_instance = NULL;

		_nimble_node_factory::_nimble_node_factory(void) :
			m_initialized(false)
		{
			std::atexit(nimble_node_factory::_delete);
		}

		_nimble_node_factory::~_nimble_node_factory(void)
		{

			if(m_initialized) {
				uninitialize();
			}
		}

		void 
		_nimble_node_factory::_delete(void)
		{

			if(nimble_node_factory::m_instance) {
				delete nimble_node_factory::m_instance;
				nimble_node_factory::m_instance = NULL;
			}
		}

		nimble_node_factory_ptr 
		_nimble_node_factory::acquire(void)
		{

			if(!nimble_node_factory::m_instance) {

				nimble_node_factory::m_instance = new nimble_node_factory;
				if(!nimble_node_factory::m_instance) {
					THROW_NIMBLE_NODE_EXCEPTION(NIMBLE_NODE_EXCEPTION_ALLOCATED);
				}
			}

			return nimble_node_factory::m_instance;
		}

		nimble_node &
		_nimble_node_factory::at(
			__in const nimble_uid &uid
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_NODE_EXCEPTION(NIMBLE_NODE_EXCEPTION_UNINITIALIZED);
			}

			return find(uid)->second.first;
		}

		bool 
		_nimble_node_factory::contains(
			__in const nimble_uid &uid
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_NODE_EXCEPTION(NIMBLE_NODE_EXCEPTION_UNINITIALIZED);
			}

			return (m_map.find(uid) != m_map.end());
		}

		size_t 
		_nimble_node_factory::decrement_reference(
			__in const nimble_uid &uid
			)
		{
			size_t result;
			std::map<nimble_uid, std::pair<nimble_node, size_t>>::iterator iter;

			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_NODE_EXCEPTION(NIMBLE_NODE_EXCEPTION_UNINITIALIZED);
			}

			iter = find(uid);

			result = --iter->second.second;
			if(result < REF_INITIAL) {
				m_map.erase(iter);
			}

			return result;
		}

		std::map<nimble_uid, std::pair<nimble_node, size_t>>::iterator 
		_nimble_node_factory::find(
			__in const nimble_uid &uid
			)
		{
			std::map<nimble_uid, std::pair<nimble_node, size_t>>::iterator result;

			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_NODE_EXCEPTION(NIMBLE_NODE_EXCEPTION_UNINITIALIZED);
			}

			result = m_map.find(uid);
			if(result == m_map.end()) {
				THROW_NIMBLE_NODE_EXCEPTION_MESSAGE(NIMBLE_NODE_EXCEPTION_NOT_FOUND,
					"%s", CHK_STR(nimble_uid::as_string(uid, true)));
			}

			return result;
		}

		nimble_uid 
		_nimble_node_factory::generate(void)
		{
			nimble_node node;

			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_NODE_EXCEPTION(NIMBLE_NODE_EXCEPTION_UNINITIALIZED);
			}

			m_map.insert(std::pair<nimble_uid, std::pair<nimble_node, size_t>>(node.uid(),
				std::pair<nimble_node, size_t>(node, REF_INITIAL)));

			return node.uid();
		}

		size_t 
		_nimble_node_factory::increment_reference(
			__in const nimble_uid &uid
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_NODE_EXCEPTION(NIMBLE_NODE_EXCEPTION_UNINITIALIZED);
			}

			return ++find(uid)->second.second;
		}

		void 
		_nimble_node_factory::initialize(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			if(m_initialized) {
				THROW_NIMBLE_NODE_EXCEPTION(NIMBLE_NODE_EXCEPTION_INITIALIZED);
			}

			m_initialized = true;
			m_map.clear();
		}

		bool 
		_nimble_node_factory::is_allocated(void)
		{
			return (nimble_node_factory::m_instance != NULL);
		}

		bool 
		_nimble_node_factory::is_initialized(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);
			return m_initialized;
		}

		size_t 
		_nimble_node_factory::reference_count(
			__in const nimble_uid &uid
			)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_NODE_EXCEPTION(NIMBLE_NODE_EXCEPTION_UNINITIALIZED);
			}

			return find(uid)->second.second;
		}

		size_t 
		_nimble_node_factory::size(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_NODE_EXCEPTION(NIMBLE_NODE_EXCEPTION_UNINITIALIZED);
			}

			return m_map.size();
		}

		std::string 
		_nimble_node_factory::to_string(
			__in_opt bool verbose
			)
		{
			std::stringstream result;
			std::map<nimble_uid, std::pair<nimble_node, size_t>>::iterator iter;

			SERIALIZE_CALL_RECUR(m_lock);

			result << "(" << (m_initialized ? "INIT" : "UNINIT") << ") " << NIMBLE_NODE_HEADER 
				<< "[" << m_map.size() << "]";

			if(verbose) {
				result << " (" << VAL_AS_HEX(nimble_node_factory_ptr, this) << ")";

				for(iter = m_map.begin(); iter != m_map.end(); ++iter) {
					result << std::endl << "--- " << nimble_node::as_string(iter->second.first, true)
						<< ", ref. " << iter->second.second;
				}
			} 

			return CHK_STR(result.str());
		}

		void 
		_nimble_node_factory::uninitialize(void)
		{
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				THROW_NIMBLE_NODE_EXCEPTION(NIMBLE_NODE_EXCEPTION_UNINITIALIZED);
			}

			m_map.clear();
			m_initialized = false;
		}
	}
}
