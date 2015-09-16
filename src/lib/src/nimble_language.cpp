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

#include <cmath>
#include <cctype>
#include <set>
#include "../include/nimble.h"
#include "../include/nimble_language_type.h"

namespace NIMBLE {

	#define CHAR_DECIMAL '.'

	static char BASE_CHAR_VAL[] = {
		'0', '1', '2', '3', '4', '5', '6', '7', 
		'8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
		};

	static const std::string SYMBOL_STR[] = {
		")", "&", "$", "(", "|", "<", ">", ">>",
		">&", ">>&", ">&!", ">!", ";", "!",
		};

	#define SYMBOL_STRING(_TYPE_) \
		((_TYPE_) > SYMBOL_MAX ? UNKNOWN : \
		CHK_STR(SYMBOL_STR[_TYPE_]))

	static const std::set<std::string> SYMBOL_SET = {
		SYMBOL_STR, SYMBOL_STR + (SYMBOL_MAX + 1)
		};

	#define IS_SYMBOL(_STR_) (SYMBOL_SET.find(_STR_) != SYMBOL_SET.end())

	static const std::string TOKEN_STR[] = {
		"BEGIN", "END", "IMMEDIATE", "LITERAL", "SYMBOL",
		};

	#define TOKEN_STRING(_TYPE_) \
		((_TYPE_) > TOKEN_MAX ? UNKNOWN : \
		CHK_STR(TOKEN_STR[_TYPE_]))

	static const std::string *TOKEN_STR_PTR[] = {
		NULL, NULL, NULL, NULL, SYMBOL_STR,
		};

	#define TOKEN_STRING_POINTER(_TYPE_) \
		((_TYPE_) > TOKEN_MAX ? NULL : \
		TOKEN_STR_PTR[_TYPE_])

	static const size_t TOKEN_STR_LEN[] = {
		0, 0, 0, 0, (SYMBOL_MAX + 1),
		};

	#define TOKEN_STRING_LENGTH(_TYPE_) \
		((_TYPE_) > TOKEN_MAX ? 0 : \
		TOKEN_STR_LEN[_TYPE_])

	double 
	_nimble_language::_as_value(
		__in char ch,
		__in int position,
		__in base_t base,
		__in_opt const nimble_token_meta &meta
		)
	{
		int val = 0;
		double result = 0.0;

		for(; val < base; ++val) {

			if(ch == BASE_CHAR_VAL[val]) {
				break;
			}
		}

		if(val >= base) {
			THROW_NIMBLE_LANGUAGE_EXCEPTION_MESSAGE(NIMBLE_LANGUAGE_EXCEPTION_INVALID_BASE,
				"Expecting base (%lu)\n%s", base,
				CHK_STR(nimble_token_meta::as_string(meta, 0, true)));
		}

		result = val * std::pow((double) base, (double) (position + 
			((position >= 0) ? -1 : 0)));

		return result;
	}

	double 
	_nimble_language::as_value(
		__in const std::string &text,
		__in base_t base,
		__in_opt const nimble_token_meta &meta
		)
	{
		char ch;
		int iter, frac;	
		double result = 0.0;
		bool has_frac = false;
		nimble_token_meta info = meta;

		for(iter = 0; iter < text.size(); ++iter, ++info.column()) {

			ch = text.at(iter);
			if(ch == CHAR_DECIMAL) {

				if(base != BASE_DECIMAL) {
					THROW_NIMBLE_LANGUAGE_EXCEPTION_MESSAGE(
						NIMBLE_LANGUAGE_EXCEPTION_INVALID_BASE,
						"Expecting base (%lu)\n%s", base,
						CHK_STR(nimble_token_meta::as_string(info, 0, true)));
				}

				if(has_frac) {
					THROW_NIMBLE_LANGUAGE_EXCEPTION_MESSAGE(
						NIMBLE_LANGUAGE_EXCEPTION_MALFORMED_VALUE,
						"\n%s", CHK_STR(nimble_token_meta::as_string(info, 0, true)));
				}

				has_frac = true;
				frac = iter;
			} else if(!std::isxdigit(ch)) {
				THROW_NIMBLE_LANGUAGE_EXCEPTION_MESSAGE(
					NIMBLE_LANGUAGE_EXCEPTION_INVALID_DIGIT,
					"\n%s", CHK_STR(nimble_token_meta::as_string(info, 0, true)));
			}
		}

		info = meta;

		for(iter = 0; iter < (has_frac ? frac : text.size()); 
				++iter, ++info.column()) {
			result += _as_value(text.at(iter), 
				(has_frac ? frac : text.size()) - iter, 
				base, info);
		}

		if(has_frac) {
			info = meta;
			info.column() += (frac + 1);

			for(iter = (frac + 1); iter < text.size(); 
					++iter, ++info.column()) {
				result += _as_value(text.at(iter), 
					frac - iter, base, info);
			}
		}

		return result;
	}

	bool 
	_nimble_language::is_symbol(
		__in const std::string &text
		)
	{
		return IS_SYMBOL(text);
	}

	toksub_t 
	_nimble_language::subtype(
		__in const std::string &text,
		__in tok_t type
		)
	{
		size_t iter = 0, len;
		toksub_t result = TOKSUB_INVALID;

		len = TOKEN_STRING_LENGTH(type);

		const std::string *tok = TOKEN_STRING_POINTER(type);
		if(tok) {

			for(; iter < len; ++iter) {

				if(tok[iter] == text) {
					result = iter;
					break;
				}
			}
		}

		return result;
	}

	std::string 
	_nimble_language::subtype_as_string(
		__in tok_t type,
		__in toksub_t subtype
		)
	{
		std::string result = UNKNOWN;

		if(subtype < TOKEN_STRING_LENGTH(type)) {

			const std::string *tok = TOKEN_STRING_POINTER(type);
			if(tok) {
				result = tok[subtype];
			}
		}

		return result;
	}

	std::string 
	_nimble_language::type_as_string(
		__in tok_t type
		)
	{
		return TOKEN_STRING(type);
	}

	_nimble_token_meta::_nimble_token_meta(
		__in_opt const std::string &line,
		__in_opt const std::string &path,
		__in_opt size_t column,
		__in_opt size_t column_off,
		__in_opt size_t row,
		__in_opt size_t row_off
		) :
			m_column(column),
			m_column_offset(column_off),
			m_line(line),
			m_path(path),
			m_row(row),
			m_row_offset(row_off)
	{
		return;
	}

	_nimble_token_meta::_nimble_token_meta(
		__in const _nimble_token_meta &other
		) :
			m_column(other.m_column),
			m_column_offset(other.m_column_offset),
			m_line(other.m_line),
			m_path(other.m_path),
			m_row(other.m_row),
			m_row_offset(other.m_row_offset)
	{
		return;
	}

	_nimble_token_meta::~_nimble_token_meta(void)
	{
		return;
	}

	_nimble_token_meta &
	_nimble_token_meta::operator=(
		__in const _nimble_token_meta &other
		)
	{
		SERIALIZE_CALL_RECUR(m_lock);

		if(this != &other) {
			m_column = other.m_column;
			m_column_offset = other.m_column_offset;
			m_line = other.m_line;
			m_path = other.m_path;
			m_row = other.m_row;
			m_row_offset = other.m_row_offset;
		}

		return *this;
	}

	std::string 
	_nimble_token_meta::as_string(
		__in const _nimble_token_meta &meta,
		__in_opt size_t tabs,
		__in_opt bool verbose
		)
	{
		return nimble_lexer_base::character_exception(meta.m_line, meta.m_path, 
			meta.m_column, meta.m_column_offset, meta.m_row, meta.m_row_offset, 
			tabs, verbose);
	}

	size_t &
	_nimble_token_meta::column(void)
	{
		SERIALIZE_CALL_RECUR(m_lock);
		return m_column;
	}

	size_t &
	_nimble_token_meta::column_offset(void)
	{
		SERIALIZE_CALL_RECUR(m_lock);
		return m_column_offset;
	}

	std::string &
	_nimble_token_meta::line(void)
	{
		SERIALIZE_CALL_RECUR(m_lock);
		return m_line;
	}

	std::string &
	_nimble_token_meta::path(void)
	{
		SERIALIZE_CALL_RECUR(m_lock);
		return m_path;
	}

	size_t &
	_nimble_token_meta::row(void)
	{
		SERIALIZE_CALL_RECUR(m_lock);
		return m_row;
	}

	size_t &
	_nimble_token_meta::row_offset(void)
	{
		SERIALIZE_CALL_RECUR(m_lock);
		return m_row_offset;
	}

	std::string 
	_nimble_token_meta::to_string(
		__in_opt size_t tabs,
		__in_opt bool verbose
		)
	{
		SERIALIZE_CALL_RECUR(m_lock);
		return nimble_token_meta::as_string(*this, tabs, verbose);
	}
}
