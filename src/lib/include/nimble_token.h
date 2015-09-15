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

#ifndef NIMBLE_TOKEN_H_
#define NIMBLE_TOKEN_H_

namespace NIMBLE {

	namespace COMPONENT {

		typedef class _nimble_token :
				public nimble_uid_class {

			public:

				_nimble_token(void);

				_nimble_token(
					__in const _nimble_token &other
					);

				virtual ~_nimble_token(void);

				_nimble_token &operator=(
					__in const _nimble_token &other
					);

				static std::string as_string(
					__in const _nimble_token &tok,
					__in_opt bool verbose = false
					);

				void clear(void);

				size_t &column(void);

				nimble_token_meta meta(
					__in_opt size_t column = 0,
					__in_opt size_t row = 0
					);

				std::string &path(void);

				size_t &position(void);

				size_t &row(void);

				toksub_t &subtype(void);

				std::string &text(void);

				virtual std::string to_string(
					__in_opt bool verbose = false
					);

				tok_t &type(void);

				double &value(void);

			protected:

				size_t m_column;

				std::string m_path;

				size_t m_position;

				size_t m_row;

				toksub_t m_subtype;

				std::string m_text;

				tok_t m_type;

				double m_value;

			private:

				std::recursive_mutex m_lock;

		} nimble_token, *nimble_token_ptr;

		typedef class _nimble_token_factory {

			public:

				~_nimble_token_factory(void);

				static _nimble_token_factory *acquire(void);

				nimble_token &at(
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

				_nimble_token_factory(void);

				_nimble_token_factory(
					__in const _nimble_token_factory &other
					);

				_nimble_token_factory &operator=(
					__in const _nimble_token_factory &other
					);

				static void _delete(void);

				std::map<nimble_uid, std::pair<nimble_token, size_t>>::iterator find(
					__in const nimble_uid &uid
					);

				std::map<nimble_uid, std::pair<nimble_token, size_t>> m_map;

				bool m_initialized;

				static _nimble_token_factory *m_instance;

			private:

				std::recursive_mutex m_lock;

		} nimble_token_factory, *nimble_token_factory_ptr;
	}
}

#endif // NIMBLE_TOKEN_H_
