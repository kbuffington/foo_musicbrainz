#pragma once

#include "foo_musicbrainz.h"

namespace foo_musicbrainz {
	class Date {
	public:
		Date();
		Date(pfc::string8);
		
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
