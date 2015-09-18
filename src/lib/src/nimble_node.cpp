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
			TRACE_ENTRY(TRACE_VERBOSE);
			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_node::_nimble_node(
			__in const _nimble_node &other
			) :
				nimble_uid_class(other),
				m_children(other.m_children),
				m_parent(other.m_parent),
				m_token(other.m_token)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_node::~_nimble_node(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_node &
		_nimble_node::operator=(
			__in const _nimble_node &other
			)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(this != &other) {
				nimble_uid_class::operator=(other);
				m_children = other.m_children;
				m_parent = other.m_parent;
				m_token = other.m_token;
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", this);
			return *this;
		}

		nimble_token_factory_ptr 
		_nimble_node::acquire_token(void)
		{
			nimble_token_factory_ptr result = NULL;

			TRACE_ENTRY(TRACE_VERBOSE);

			result = nimble_token_factory::is_allocated() 
				? nimble_token_factory::acquire() : NULL;

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", result);
			return result;
		}

		std::string 
		_nimble_node::as_string(
			__in const _nimble_node &node,
			__in_opt bool verbose
			)
		{
			std::stringstream result;
			nimble_token_factory_ptr fact = NULL;

			TRACE_ENTRY(TRACE_VERBOSE);

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

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result.str()));
			return CHK_STR(result.str());
		}

		std::vector<size_t> &
		_nimble_node::children(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);
			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", &m_children);
			return m_children;
		}

		void 
		_nimble_node::clear(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			m_children.clear();
			m_parent = PAR_INVALID;
			m_token = UID_INVALID;

			TRACE_EXIT(TRACE_VERBOSE);
		}

		bool 
		_nimble_node::is_leaf(void)
		{
			bool result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = m_children.empty();

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", result);
			return result;
		}

		bool 
		_nimble_node::is_root(void)
		{
			bool result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = (m_parent == PAR_INVALID);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", result);
			return result;
		}

		size_t &
		_nimble_node::parent(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);
			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", m_parent);
			return m_parent;
		}

		std::string 
		_nimble_node::to_string(
			__in_opt bool verbose
			)
		{
			std::string result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			result = nimble_node::as_string(*this, verbose);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result));
			return CHK_STR(result);
		}

		nimble_uid &
		_nimble_node::token(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);
			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(m_token.to_string(true)));
			return m_token;
		}

		nimble_node_factory_ptr nimble_node_factory::m_instance = NULL;

		_nimble_node_factory::_nimble_node_factory(void) :
			m_initialized(false)
		{
			TRACE_ENTRY(TRACE_VERBOSE);

			std::atexit(nimble_node_factory::_delete);

			TRACE_EXIT(TRACE_VERBOSE);
		}

		_nimble_node_factory::~_nimble_node_factory(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);

			if(m_initialized) {
				uninitialize();
			}

			TRACE_EXIT(TRACE_VERBOSE);
		}

		void 
		_nimble_node_factory::_delete(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);

			if(nimble_node_factory::m_instance) {
				TRACE_MESSAGE(TRACE_INFORMATION, "Deleted node component instance, ptr. 0x%p", 
					nimble_node_factory::m_instance);
				delete nimble_node_factory::m_instance;
				nimble_node_factory::m_instance = NULL;
			}

			TRACE_EXIT(TRACE_VERBOSE);
		}

		nimble_node_factory_ptr 
		_nimble_node_factory::acquire(void)
		{
			nimble_node_factory_ptr result = NULL;

			TRACE_ENTRY(TRACE_VERBOSE);

			if(!nimble_node_factory::m_instance) {

				nimble_node_factory::m_instance = new nimble_node_factory;
				if(!nimble_node_factory::m_instance) {
					TRACE_MESSAGE(TRACE_ERROR, "%s", 
						NIMBLE_NODE_EXCEPTION_STRING(NIMBLE_NODE_EXCEPTION_ALLOCATED));
					THROW_NIMBLE_NODE_EXCEPTION(NIMBLE_NODE_EXCEPTION_ALLOCATED);
				} else {
					TRACE_MESSAGE(TRACE_INFORMATION, "Allocated node component instance, ptr. 0x%p", 
						nimble_node_factory::m_instance);
				}
			}

			result = nimble_node_factory::m_instance;

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", result);
			return result;
		}

		nimble_node &
		_nimble_node_factory::at(
			__in const nimble_uid &uid
			)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_NODE_EXCEPTION_STRING(NIMBLE_NODE_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_NODE_EXCEPTION(NIMBLE_NODE_EXCEPTION_UNINITIALIZED);
			}

			nimble_node &node = find(uid)->second.first;

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(node.to_string(true)));
			return node;
		}

		bool 
		_nimble_node_factory::contains(
			__in const nimble_uid &uid
			)
		{
			bool result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_NODE_EXCEPTION_STRING(NIMBLE_NODE_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_NODE_EXCEPTION(NIMBLE_NODE_EXCEPTION_UNINITIALIZED);
			}

			result = (m_map.find(uid) != m_map.end());

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", result);
			return result;
		}

		size_t 
		_nimble_node_factory::decrement_reference(
			__in const nimble_uid &uid
			)
		{
			size_t result;
			std::map<nimble_uid, std::pair<nimble_node, size_t>>::iterator iter;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_NODE_EXCEPTION_STRING(NIMBLE_NODE_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_NODE_EXCEPTION(NIMBLE_NODE_EXCEPTION_UNINITIALIZED);
			}

			iter = find(uid);

			result = --iter->second.second;
			if(result < REF_INITIAL) {
				TRACE_MESSAGE(TRACE_INFORMATION, "Removing node instance, %s", 
					CHK_STR(nimble_node::as_string(iter->second.first, true)));
				m_map.erase(iter);
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		std::map<nimble_uid, std::pair<nimble_node, size_t>>::iterator 
		_nimble_node_factory::find(
			__in const nimble_uid &uid
			)
		{
			std::map<nimble_uid, std::pair<nimble_node, size_t>>::iterator result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_NODE_EXCEPTION_STRING(NIMBLE_NODE_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_NODE_EXCEPTION(NIMBLE_NODE_EXCEPTION_UNINITIALIZED);
			}

			result = m_map.find(uid);
			if(result == m_map.end()) {
				TRACE_MESSAGE(TRACE_ERROR, "%s, %s", 
					NIMBLE_NODE_EXCEPTION_STRING(NIMBLE_NODE_EXCEPTION_UNINITIALIZED),
					CHK_STR(nimble_uid::as_string(uid, true)));
				THROW_NIMBLE_NODE_EXCEPTION_MESSAGE(NIMBLE_NODE_EXCEPTION_NOT_FOUND,
					"%s", CHK_STR(nimble_uid::as_string(uid, true)));
			}

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", result);
			return result;
		}

		nimble_uid 
		_nimble_node_factory::generate(void)
		{
			nimble_node node;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_NODE_EXCEPTION_STRING(NIMBLE_NODE_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_NODE_EXCEPTION(NIMBLE_NODE_EXCEPTION_UNINITIALIZED);
			}

			TRACE_MESSAGE(TRACE_INFORMATION, "Generating new node: %s", 
				CHK_STR(node.to_string(true)));
			m_map.insert(std::pair<nimble_uid, std::pair<nimble_node, size_t>>(node.uid(),
				std::pair<nimble_node, size_t>(node, REF_INITIAL)));

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(node.to_string(true)));
			return node.uid();
		}

		size_t 
		_nimble_node_factory::increment_reference(
			__in const nimble_uid &uid
			)
		{
			size_t result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_NODE_EXCEPTION_STRING(NIMBLE_NODE_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_NODE_EXCEPTION(NIMBLE_NODE_EXCEPTION_UNINITIALIZED);
			}

			result = ++find(uid)->second.second;

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		void 
		_nimble_node_factory::initialize(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_NODE_EXCEPTION_STRING(NIMBLE_NODE_EXCEPTION_INITIALIZED));
				THROW_NIMBLE_NODE_EXCEPTION(NIMBLE_NODE_EXCEPTION_INITIALIZED);
			}

			m_initialized = true;
			m_map.clear();
			TRACE_MESSAGE(TRACE_INFORMATION, "%s", "Node component instance initialized");

			TRACE_EXIT(TRACE_VERBOSE);
		}

		bool 
		_nimble_node_factory::is_allocated(void)
		{
			bool result;

			TRACE_ENTRY(TRACE_VERBOSE);

			result = (nimble_node_factory::m_instance != NULL);

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		bool 
		_nimble_node_factory::is_initialized(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);
			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", m_initialized);
			return m_initialized;
		}

		size_t 
		_nimble_node_factory::reference_count(
			__in const nimble_uid &uid
			)
		{
			size_t result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_NODE_EXCEPTION_STRING(NIMBLE_NODE_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_NODE_EXCEPTION(NIMBLE_NODE_EXCEPTION_UNINITIALIZED);
			}

			result = find(uid)->second.second;

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		size_t 
		_nimble_node_factory::size(void)
		{
			size_t result;

			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_NODE_EXCEPTION_STRING(NIMBLE_NODE_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_NODE_EXCEPTION(NIMBLE_NODE_EXCEPTION_UNINITIALIZED);
			}

			result = m_map.size();

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
			return result;
		}

		std::string 
		_nimble_node_factory::to_string(
			__in_opt bool verbose
			)
		{
			std::stringstream result;
			std::map<nimble_uid, std::pair<nimble_node, size_t>>::iterator iter;

			TRACE_ENTRY(TRACE_VERBOSE);
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

			TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result.str()));
			return CHK_STR(result.str());
		}

		void 
		_nimble_node_factory::uninitialize(void)
		{
			TRACE_ENTRY(TRACE_VERBOSE);
			SERIALIZE_CALL_RECUR(m_lock);

			if(!m_initialized) {
				TRACE_MESSAGE(TRACE_ERROR, "%s", 
					NIMBLE_NODE_EXCEPTION_STRING(NIMBLE_NODE_EXCEPTION_UNINITIALIZED));
				THROW_NIMBLE_NODE_EXCEPTION(NIMBLE_NODE_EXCEPTION_UNINITIALIZED);
			}

			m_map.clear();
			m_initialized = false;
			TRACE_MESSAGE(TRACE_INFORMATION, "%s", "Node component instance uninitialized");

			TRACE_EXIT(TRACE_VERBOSE);
		}
	}
}
