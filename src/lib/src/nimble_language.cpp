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
		"=", ")", "&", "$", "(", "|", "<", ">", 
		">>", ">&", ">>&", ">&!", ">!", ";", "!",
		};

	#define SYMBOL_STRING(_TYPE_) \
		((_TYPE_) > SYMBOL_MAX ? UNKNOWN : \
		CHK_STR(SYMBOL_STR[_TYPE_]))

	static const std::set<std::string> SYMBOL_SET = {
		SYMBOL_STR, SYMBOL_STR + (SYMBOL_MAX + 1)
		};

	#define IS_SYMBOL(_STR_) (SYMBOL_SET.find(_STR_) != SYMBOL_SET.end())

	static const std::set<nimble_tok_t> CONTROL_TOKEN_SET = {
		TOKEN_ARGUMENT, TOKEN_ASSIGNMENT, TOKEN_BEGIN, TOKEN_CALL,
		TOKEN_CALL_LIST, TOKEN_COMMAND, TOKEN_END, TOKEN_STATEMENT,
		};

	#define IS_CONTROL_TOKEN(_TYPE_) \
		(CONTROL_TOKEN_SET.find(_TYPE_) != CONTROL_TOKEN_SET.end())

	static const std::string TOKEN_STR[] = {
		"ARGUMENT", "ASSIGNMENT", "BEGIN", "CALL", "CALL_LIST", "COMMAND", 
		"END", "IMMEDIATE", "LITERAL", "STATEMENT", "SYMBOL",
		};

	#define TOKEN_STRING(_TYPE_) \
		((_TYPE_) > TOKEN_MAX ? UNKNOWN : \
		CHK_STR(TOKEN_STR[_TYPE_]))

	static const std::string *TOKEN_STR_PTR[] = {
		NULL, NULL, NULL, NULL, NULL, NULL, 
		NULL, NULL, NULL, NULL, SYMBOL_STR,
		};

	#define TOKEN_STRING_POINTER(_TYPE_) \
		((_TYPE_) > TOKEN_MAX ? NULL : \
		TOKEN_STR_PTR[_TYPE_])

	static const size_t TOKEN_STR_LEN[] = {
		0, 0, 0, 0, 0, 0, 
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

		TRACE_ENTRY(TRACE_VERBOSE);

		for(; val < base; ++val) {

			if(ch == BASE_CHAR_VAL[val]) {
				break;
			}
		}

		if(val >= base) {
			TRACE_MESSAGE(TRACE_ERROR, "%s, expecting base (%lu)\n%s", NIMBLE_LANGUAGE_EXCEPTION_STRING(
				NIMBLE_LANGUAGE_EXCEPTION_INVALID_BASE), base,
				CHK_STR(nimble_token_meta::as_string(meta, 0, true)));
			THROW_NIMBLE_LANGUAGE_EXCEPTION_MESSAGE(NIMBLE_LANGUAGE_EXCEPTION_INVALID_BASE,
				"Expecting base (%lu)\n%s", base,
				CHK_STR(nimble_token_meta::as_string(meta, 0, true)));
		}

		result = val * std::pow((double) base, (double) (position + 
			((position >= 0) ? -1 : 0)));

		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %.8f", result);
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

		TRACE_ENTRY(TRACE_VERBOSE);

		for(iter = 0; iter < text.size(); ++iter, ++info.column()) {

			ch = text.at(iter);
			if(ch == CHAR_DECIMAL) {

				if(base != BASE_DECIMAL) {
					TRACE_MESSAGE(TRACE_ERROR, "%s, expecting base (%lu)\n%s", NIMBLE_LANGUAGE_EXCEPTION_STRING(
						NIMBLE_LANGUAGE_EXCEPTION_INVALID_BASE), base,
						CHK_STR(nimble_token_meta::as_string(meta, 0, true)));
					THROW_NIMBLE_LANGUAGE_EXCEPTION_MESSAGE(
						NIMBLE_LANGUAGE_EXCEPTION_INVALID_BASE,
						"Expecting base (%lu)\n%s", base,
						CHK_STR(nimble_token_meta::as_string(info, 0, true)));
				}

				if(has_frac) {
					TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", NIMBLE_LANGUAGE_EXCEPTION_STRING(
						NIMBLE_LANGUAGE_EXCEPTION_MALFORMED_VALUE),
						CHK_STR(nimble_token_meta::as_string(meta, 0, true)));
					THROW_NIMBLE_LANGUAGE_EXCEPTION_MESSAGE(
						NIMBLE_LANGUAGE_EXCEPTION_MALFORMED_VALUE,
						"\n%s", CHK_STR(nimble_token_meta::as_string(info, 0, true)));
				}

				has_frac = true;
				frac = iter;
			} else if(!std::isxdigit(ch)) {
				TRACE_MESSAGE(TRACE_ERROR, "%s\n%s", NIMBLE_LANGUAGE_EXCEPTION_STRING(
					NIMBLE_LANGUAGE_EXCEPTION_INVALID_DIGIT),
					CHK_STR(nimble_token_meta::as_string(meta, 0, true)));
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

		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %.8f", result);
		return result;
	}

	bool 
	_nimble_language::is_control_token(
		__in nimble_tok_t type
		)
	{
		bool result;

		TRACE_ENTRY(TRACE_VERBOSE);

		result = IS_CONTROL_TOKEN(type);

		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", result);
		return result;
	}

	bool 
	_nimble_language::is_symbol(
		__in const std::string &text
		)
	{
		bool result;

		TRACE_ENTRY(TRACE_VERBOSE);

		result = IS_SYMBOL(text);

		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", result);
		return result;
	}

	nimble_subtok_t 
	_nimble_language::subtype(
		__in const std::string &text,
		__in nimble_tok_t type
		)
	{
		size_t iter = 0, len;
		nimble_subtok_t result = TOKSUB_INVALID;

		TRACE_ENTRY(TRACE_VERBOSE);

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

		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s (0x%x)", 
			CHK_STR(nimble_language::subtype_as_string(type, result)), result);
		return result;
	}

	std::string 
	_nimble_language::subtype_as_string(
		__in nimble_tok_t type,
		__in nimble_subtok_t subtype
		)
	{
		std::string result = UNKNOWN;

		TRACE_ENTRY(TRACE_VERBOSE);

		if(subtype < TOKEN_STRING_LENGTH(type)) {

			const std::string *tok = TOKEN_STRING_POINTER(type);
			if(tok) {
				result = tok[subtype];
			}
		}

		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result));
		return CHK_STR(result);
	}

	std::string 
	_nimble_language::type_as_string(
		__in nimble_tok_t type
		)
	{
		std::string result;

		TRACE_ENTRY(TRACE_VERBOSE);

		result = TOKEN_STRING(type);

		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result));
		return CHK_STR(result);
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
		TRACE_ENTRY(TRACE_VERBOSE);
		TRACE_EXIT(TRACE_VERBOSE);
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
		TRACE_ENTRY(TRACE_VERBOSE);
		TRACE_EXIT(TRACE_VERBOSE);
	}

	_nimble_token_meta::~_nimble_token_meta(void)
	{
		TRACE_ENTRY(TRACE_VERBOSE);
		TRACE_EXIT(TRACE_VERBOSE);
	}

	_nimble_token_meta &
	_nimble_token_meta::operator=(
		__in const _nimble_token_meta &other
		)
	{
		TRACE_ENTRY(TRACE_VERBOSE);
		SERIALIZE_CALL_RECUR(m_lock);

		if(this != &other) {
			m_column = other.m_column;
			m_column_offset = other.m_column_offset;
			m_line = other.m_line;
			m_path = other.m_path;
			m_row = other.m_row;
			m_row_offset = other.m_row_offset;
		}
		
		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "ptr. 0x%p", this);
		return *this;
	}

	std::string 
	_nimble_token_meta::as_string(
		__in const _nimble_token_meta &meta,
		__in_opt size_t tabs,
		__in_opt bool verbose
		)
	{
		std::string result;

		TRACE_ENTRY(TRACE_VERBOSE);

		result = nimble_lexer_base::character_exception(meta.m_line, meta.m_path, 
			meta.m_column, meta.m_column_offset, meta.m_row, meta.m_row_offset, 
			tabs, verbose);

		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result));
		return CHK_STR(result);
	}

	size_t &
	_nimble_token_meta::column(void)
	{
		TRACE_ENTRY(TRACE_VERBOSE);
		SERIALIZE_CALL_RECUR(m_lock);
		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", m_column);
		return m_column;
	}

	size_t &
	_nimble_token_meta::column_offset(void)
	{
		TRACE_ENTRY(TRACE_VERBOSE);
		SERIALIZE_CALL_RECUR(m_lock);
		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", m_column_offset);
		return m_column_offset;
	}

	std::string &
	_nimble_token_meta::line(void)
	{
		TRACE_ENTRY(TRACE_VERBOSE);
		SERIALIZE_CALL_RECUR(m_lock);
		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(m_line));
		return m_line;
	}

	std::string &
	_nimble_token_meta::path(void)
	{
		TRACE_ENTRY(TRACE_VERBOSE);
		SERIALIZE_CALL_RECUR(m_lock);
		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(m_path));
		return m_path;
	}

	size_t &
	_nimble_token_meta::row(void)
	{
		TRACE_ENTRY(TRACE_VERBOSE);
		SERIALIZE_CALL_RECUR(m_lock);
		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", m_row);
		return m_row;
	}

	size_t &
	_nimble_token_meta::row_offset(void)
	{
		TRACE_ENTRY(TRACE_VERBOSE);
		SERIALIZE_CALL_RECUR(m_lock);
		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %lu", m_row_offset);
		return m_row_offset;
	}

	std::string 
	_nimble_token_meta::to_string(
		__in_opt size_t tabs,
		__in_opt bool verbose
		)
	{
		std::string result;

		TRACE_ENTRY(TRACE_VERBOSE);
		SERIALIZE_CALL_RECUR(m_lock);

		result = nimble_token_meta::as_string(*this, tabs, verbose);;

		TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. %s", CHK_STR(result));
		return CHK_STR(result);
	}
}
