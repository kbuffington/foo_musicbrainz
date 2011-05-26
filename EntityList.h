#pragma once

#include "foo_musicbrainz.h"

namespace foo_musicbrainz {
	template <class T>
	class EntityList {
	public:
		void add(T *item);
		size_t count() const;
		T *operator[](t_size index) const ;

		EntityList() {};
		~EntityList() {};

	protected:
		pfc::list_t<T *> items;
	};
}
