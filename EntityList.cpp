#include "foo_musicbrainz.h"
#include "EntityList.h"

using namespace foo_musicbrainz;

void EntityList<class T>::add(T *item) {
	items.add_item(item);
}

size_t EntityList<class T>::count() const {
	return items.get_count();
}

T *EntityList<class T>::operator[](t_size index) const {
	if (index >= items.get_count()) {
		throw pfc::exception_overflow();
	}
	return items[index];
}

EntityList<class T>::~EntityList() {
	for (unsigned int i = 0; i < items.get_count(); i++) {
		delete items[i];
	}
}
