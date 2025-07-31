/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdlib>
#include <limits>
#include <cctype>

#include "popt.h"

using namespace popt;

namespace {

inline
bool
suffix (
	const char * text,
	unsigned long & multiplier
) {
	switch (*text) {
		case 'G':
			++text;
			if ('i' == *text) {
				++text;
				multiplier = 1024UL * 1024UL * 1024UL;
			} else
				multiplier = 1000000000UL;
			break;
		case 'M':
			++text;
			if ('i' == *text) {
				++text;
				multiplier = 1024UL * 1024UL;
			} else
				multiplier = 1000000UL;
			break;
		case 'k':
			++text;
			multiplier = 1000UL;
			break;
		case 'K':
			++text;
			if ('i' == *text) {
				++text;
				multiplier = 1024UL;
			} else
				return false;
			break;
		default:
			multiplier = 1UL;
			break;
	}
	return !*text;
}

}

size_definition::~size_definition() {}
void size_definition::action(processor &, const char * text)
{
	const char * old(text);
	value = std::strtoul(text, const_cast<char **>(&text), base);
	if (text == old)
		throw error(old, "not a number");
	unsigned long multiplier = 1UL;
	if (*text && !suffix(text, multiplier))
		throw error(text, "not an IEC suffix");
	if (std::numeric_limits<unsigned long>::max() / multiplier <= value)
		throw error(old, "size too big");
	value *= multiplier;
	set = true;
}
