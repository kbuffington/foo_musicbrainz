#pragma once

#include "foo_musicbrainz.h"

namespace foo_musicbrainz {
	class Date {
	public:
		Date();
		Date(short year, short month = 0, short day = 0);
		Date(pfc::string8);
		
		short get_year() { return year; }
		short get_month() { return month; }
		short get_day() { return day; }
		
		static int compare(const Date &left, const Date &right);
		bool inline operator>(const Date &) const;
		bool inline operator<(const Date &) const;
		bool inline operator>=(const Date &) const;
		bool inline operator<=(const Date &) const;
		bool inline operator==(const Date &) const;
		bool inline operator!=(const Date &) const;

		operator pfc::string8() const;

	private:
		short year;
		short month;
		short day;
	};
}
