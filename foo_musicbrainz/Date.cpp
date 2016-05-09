#include <regex>
#include "foo_musicbrainz.h"
#include "Date.h"

using namespace std::tr1;
using namespace pfc;
using namespace foo_musicbrainz;

#define STRING_TO_DATE_PART(part, index) \
	if (matches[index].length() > 0) { \
		part = atoi(matches[index].str().data()); \
	} else { \
		part = 0; \
	}

Date::Date() {
	year = month = day = 0;
}

Date::Date(short year, short month, short day) {
	this->year = this->month = this->day = 0;
	if (year == 0) return;
	this->year = year;
	if (month == 0) return;
	this->month = month;
	if (day == 0) return;
	this->day = day;
}

Date::Date(string8 str) {
	static regex rx("^\\s*(?:([12][0-9]{3})(?:-(?:([01]?[0-9])(?:-([0123]?[0-9])?)?)?)?)?\\s*$");
	cmatch matches;
	if (regex_search(str.get_ptr(), matches, rx)) {
		STRING_TO_DATE_PART(year, 1)
		STRING_TO_DATE_PART(month, 2)
		STRING_TO_DATE_PART(day, 3)
	} else {
		year = month = day = 0;
	}
}

#define COMPARE_DATE_PARTS(part) \
	if (left.part != right.part) { \
		if (left.part == 0) { \
			return 1; \
		} else if (right.part == 0) { \
			return -1; \
		} else { \
			return left.part > right.part ? 1 : -1; \
		} \
	}

int Date::compare(const Date &left, const Date &right) {
	COMPARE_DATE_PARTS(year)
	COMPARE_DATE_PARTS(month)
	COMPARE_DATE_PARTS(day)
	return 0;
}

bool Date::operator>(const Date &other) const {
	return compare(*this, other) == 1;
}

bool Date::operator<(const Date &other) const {
	return compare(*this, other) == -1;
}

bool Date::operator>=(const Date &other) const {
	return compare(*this, other) != -1;
}

bool Date::operator<=(const Date &other) const {
	return compare(*this, other) != 1;
}

bool Date::operator==(const Date &other) const {
	return compare(*this, other) == 0;
}

bool Date::operator!=(const Date &other) const {
	return compare(*this, other) != 0;
}

Date::operator pfc::string8() const {
	pfc::string8 str;

	if (year != 0) str << year;
	if (month != 0) {
		str << "-";
		if (month < 10) {
			str << "0";
		}
		str << month;
	}
	if (day != 0) {
		str << "-";
		if (day < 10) {
			str << "0";
		}
		str << day;
	}

	return str;
}
