#pragma once

#include "foo_musicbrainz.h"

namespace foo_musicbrainz {
	template <class T>
	class EntityList {
	public:
		void add(T *item) {
			items.add_item(item);
		};

		size_t count() const{
			return items.get_count();
		}

		T *operator[](t_size index) const {
			if (index >= items.get_count()) {
				throw pfc::exception_overflow();
			}
			return items[index];
		}

		inline T *get(t_size index) const {
			return (*this)[index];
		}

		T *extract(t_size index) {
			T *item = (*this)[index];
			items.remove_by_idx(index);
			return item;
		}

		void remove(t_size index) {
			delete extract(index);
		}

		EntityList() {};
		~EntityList() {
			for (unsigned int i = 0; i < items.get_count(); i++) {
				delete items[i];
			}
		}

	protected:
		pfc::list_t<T *> items;
	};
}
