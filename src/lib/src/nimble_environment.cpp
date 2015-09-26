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

#include <cstddef>
#include <cstring>
#include "../include/nimble.h"
#include "../include/nimble_environment_type.h"

namespace NIMBLE {

	#define ENV_ENTRY_MAX UINT16_MAX

	typedef struct __attribute__((__packed__)) _nimble_environment_header {
		uint32_t size;
		uint32_t position;
		uint16_t count;
		char **pairs;
	} nimble_environment_header, *nimble_environment_header_ptr;

	void 
	_nimble_environment::add(
		__in void *context,
		__in const std::string &field,
		__in const std::string &value
		)
	{
		nimble_environment_header_ptr head = NULL;

		TRACE_ENTRY(TRACE_VERBOSE);

		nimble_environment::validate(context);
		head = (nimble_environment_header_ptr) context;

		if((head->position + field.size() + value.size() + 2) >= head->size) {
			TRACE_MESSAGE(TRACE_ERROR, "%s", 
				NIMBLE_ENVIRONMENT_EXCEPTION_STRING(NIMBLE_ENVIRONMENT_EXCEPTION_RESOURCES));
			THROW_NIMBLE_ENVIRONMENT_EXCEPTION(NIMBLE_ENVIRONMENT_EXCEPTION_RESOURCES);
		}

		strcpy((char *) head + head->position, field.c_str());
		head->position += (field.size() + 1);
		strcpy((char *) head + head->position, value.c_str());
		head->position += (value.size() + 1);
		++head->count;

		TRACE_EXIT(TRACE_VERBOSE);
	}

	std::string 
	_nimble_environment::as_string(
		__in void *context,
		__in_opt bool verbose
		)
	{
		char **str = NULL;
		uint16_t iter = 0;
		std::stringstream result;
		std::string field, value;
		nimble_environment_header_ptr head = NULL;

		TRACE_ENTRY(TRACE_VERBOSE);

		nimble_environment::validate(context);
		head = (nimble_environment_header_ptr) context;
		result << "ENV[" << head->count << "]";

		if(verbose) {
			result << " (" << head->size << " bytes, pos. " 
				<< head->position << ") ";
		}

		result << "{";
		str = head->pairs;

		for(; iter < head->count; ++iter) {
			nimble_environment::at(context, iter, field, value);
			result << std::endl << "\t" << field << " --> " << value;
		}

		if(iter) {
			result << std::endl;
		}

		result << "}";

		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result.str()));
		return result.str();
	}

	void 
	_nimble_environment::at(
		__in void *context,
		__in uint16_t position,
		__out std::string &field,
		__out std::string &value
		)
	{
		char *base = NULL;
		uint16_t iter = 0;
		uint32_t offset = 0;
		nimble_environment_header_ptr head = NULL;

		TRACE_ENTRY(TRACE_VERBOSE);

		nimble_environment::validate(context);
		head = (nimble_environment_header_ptr) context;

		if(position >= head->count) {
			TRACE_MESSAGE(TRACE_ERROR, "%s, pos. %lu", 
				NIMBLE_ENVIRONMENT_EXCEPTION_STRING(NIMBLE_ENVIRONMENT_EXCEPTION_NOT_FOUND),
				position);
			THROW_NIMBLE_ENVIRONMENT_EXCEPTION_MESSAGE(NIMBLE_ENVIRONMENT_EXCEPTION_NOT_FOUND,
				"pos. %lu", position);
		}

		base = (char *) head + offsetof(nimble_environment_header, pairs);

		for(; iter <= position; ++iter) {
			field = base + offset;
			offset += (field.size() + 1);
			value = base + offset;
			offset += (value.size() + 1);
		}

		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s --> %s", CHK_STR(field), CHK_STR(value));
	}

	uint16_t 
	_nimble_environment::count(
		__in void *context
		)
	{
		uint16_t result = 0;

		TRACE_ENTRY(TRACE_VERBOSE);

		nimble_environment::validate(context);
		result = ((nimble_environment_header_ptr) context)->count;

		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
		return result;
	}

	void 
	_nimble_environment::initialize(
		__inout void *context,
		__in size_t size
		)
	{
		nimble_environment_header_ptr head = NULL;

		TRACE_ENTRY(TRACE_VERBOSE);

		if(!context) {
			TRACE_MESSAGE(TRACE_ERROR, "%s", 
				NIMBLE_ENVIRONMENT_EXCEPTION_STRING(NIMBLE_ENVIRONMENT_EXCEPTION_INVALID));
			THROW_NIMBLE_ENVIRONMENT_EXCEPTION(NIMBLE_ENVIRONMENT_EXCEPTION_INVALID);
		}

		memset(context, 0, size);
		head = (nimble_environment_header_ptr) context;
		head->size = size;
		head->position = offsetof(nimble_environment_header, pairs);

		TRACE_EXIT(TRACE_VERBOSE);
	}

	uint32_t 
	_nimble_environment::size(
		__in void *context
		)
	{
		uint32_t result = 0;

		TRACE_ENTRY(TRACE_VERBOSE);

		nimble_environment::validate(context);
		result = ((nimble_environment_header_ptr) context)->size;

		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", result);
		return result;
	}

	void 
	_nimble_environment::validate(
		__in void *context
		)
	{
		nimble_environment_header_ptr head = NULL;

		TRACE_ENTRY(TRACE_VERBOSE);

		if(!context) {
			TRACE_MESSAGE(TRACE_ERROR, "%s", 
				NIMBLE_ENVIRONMENT_EXCEPTION_STRING(NIMBLE_ENVIRONMENT_EXCEPTION_INVALID));
			THROW_NIMBLE_ENVIRONMENT_EXCEPTION(NIMBLE_ENVIRONMENT_EXCEPTION_INVALID);
		}

		head = (nimble_environment_header_ptr) context;
		if((head->position < offsetof(nimble_environment_header, pairs))
				|| (head->position > head->size)) {
			TRACE_MESSAGE(TRACE_ERROR, "%s", 
				NIMBLE_ENVIRONMENT_EXCEPTION_STRING(NIMBLE_ENVIRONMENT_EXCEPTION_INVALID));
			THROW_NIMBLE_ENVIRONMENT_EXCEPTION(NIMBLE_ENVIRONMENT_EXCEPTION_INVALID);
		}

		TRACE_EXIT(TRACE_VERBOSE);
	}
}
