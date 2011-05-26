#pragma once

#include "foo_musicbrainz.h"

namespace foo_musicbrainz {
	class Date {
	public:
		Date();
		Date(pfc::string8);

		static int compare(const Date &left, const Date&right);
		inline friend bool operator>(const Date &left, const Date &right);
		inline friend bool operator<(const Date &left, const Date &right);
		inline friend bool operator>=(const Date &left, const Date &right);
		inline friend bool operator<=(const Date &left, const Date &right);
		inline friend bool operator==(const Date &left, const Date &right);
		inline friend bool operator!=(const Date &left, const Date &right);

		operator pfc::string8() const;

	private:
		short year;
		short month;
		short day;
	};
}
