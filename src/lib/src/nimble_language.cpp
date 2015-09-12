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
		">&", ">>&", ">&!", ">!", ";",
		};

	#define SYMBOL_STRING(_TYPE_) \
		((_TYPE_) > SYMBOL_MAX ? UNKNOWN : \
		CHK_STR(SYMBOL_STR[_TYPE_]))

	static const std::string TOKEN_STR[] = {
		"BEGIN", "END", "IDENTIFIER", "IMMEDIATE", 
		"LITERAL", "SYMBOL",
		};

	#define TOKEN_STRING(_TYPE_) \
		((_TYPE_) > TOKEN_MAX ? UNKNOWN : \
		CHK_STR(TOKEN_STR[_TYPE_]))

	static const std::string *TOKEN_STR_PTR[] = {
		NULL, NULL, NULL, NULL, NULL, SYMBOL_STR,
		};

	#define TOKEN_STRING_POINTER(_TYPE_) \
		((_TYPE_) > TOKEN_MAX ? NULL : \
		TOKEN_STR_PTR[_TYPE_])

	static const size_t TOKEN_STR_LEN[] = {
		0, 0, 0, 0, 0, (SYMBOL_MAX + 1),
		};

	#define TOKEN_STRING_LENGTH(_TYPE_) \
		((_TYPE_) > TOKEN_MAX ? 0 : \
		TOKEN_STR_LEN[_TYPE_])

	double 
	_nimble_language::_as_value(
		__in char ch,
		__in int position,
		__in base_t base
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
			THROW_NIMBLE_LANGUAGE_EXCEPTION_MESSAGE(
				NIMBLE_LANGUAGE_EXCEPTION_INVALID_BASE,
				"%lu", base);
		}

		result = val * std::pow((double) base, (double) (position + 
			((position >= 0) ? -1 : 0)));

		return result;
	}

	double 
	_nimble_language::as_value(
		__in const std::string &text,
		__in base_t base
		)
	{
		char ch;
		int iter, frac;	
		double result = 0.0;
		bool has_frac = false;

		for(iter = 0; iter < text.size(); ++iter) {

			ch = text.at(iter);
			if(ch == CHAR_DECIMAL) {

				if(base != BASE_DECIMAL) {
					THROW_NIMBLE_LANGUAGE_EXCEPTION_MESSAGE(
						NIMBLE_LANGUAGE_EXCEPTION_INVALID_BASE,
						"%lu", base);
				}

				if(has_frac) {
					THROW_NIMBLE_LANGUAGE_EXCEPTION_MESSAGE(
						NIMBLE_LANGUAGE_EXCEPTION_MALFORMED_VALUE,
						"%s", CHK_STR(text));
				}

				has_frac = true;
				frac = iter;
			} else if(!std::isxdigit(ch)) {
				THROW_NIMBLE_LANGUAGE_EXCEPTION_MESSAGE(
					NIMBLE_LANGUAGE_EXCEPTION_INVALID_DIGIT,
					"%s[%lu] -> \'%c\' (%02x)", CHK_STR(text), 
					iter, std::isprint(ch) ? ch : ' ', ch);
			}
		}

		for(iter = 0; iter < (has_frac ? frac : text.size()); 
				++iter) {
			result += _as_value(text.at(iter), 
				(has_frac ? frac : text.size()) - iter, 
				base);
		}

		if(has_frac) {

			for(iter = (frac + 1); iter < text.size(); 
					++iter) {
				result += _as_value(text.at(iter), 
					frac - iter, base);
			}
		}

		return result;
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
}
