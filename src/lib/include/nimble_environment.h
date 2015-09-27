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

#ifndef NIMBLE_ENVIRONMENT_H_
#define NIMBLE_ENVIRONMENT_H_

namespace NIMBLE {

	#define ENV_FLAG_EXIT 0x1
	#define ENV_FLAG_MAX ENV_FLAG_EXIT
	#define ENV_MEM_LEN 1024

	typedef std::map<std::string, std::string> nimble_environment_map, 
		*nimble_environment_map_ptr;

	typedef class _nimble_environment {

		public:

			static void add(
				__in void *context,
				__in const std::string &field,
				__in const std::string &value
				);

			static std::string as_string(
				__in void *context,
				__in_opt bool verbose = false
				);

			static void at(
				__in void *context,
				__in uint16_t position,
				__out std::string &field,
				__out std::string &value
				);

			static uint16_t count(
				__in void *context
				);

			static void flag_clear(
				__in void *context,
				__in uint8_t flag
				);

			static void flag_set(
				__in void *context,
				__in uint8_t flag
				);

			static void initialize(
				__inout void *context,
				__in size_t size
				);

			static bool is_flag_set(
				__in void *context,
				__in uint8_t flag
				);

			static uint32_t size(
				__in void *context
				);

		protected:

			static void validate(
				__in void *context
				);

	} nimble_environment, *nimble_environment_ptr;
}

#endif // NIMBLE_ENVIRONMENT_H_
