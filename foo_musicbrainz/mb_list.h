#pragma once

template <class T>
class mb_list
{
public:
	mb_list() {};
	~mb_list()
	{
		items.remove_all();
	}

	void add(T* item) {
		items.add_item(item);
	};

	t_size count() const{
		return items.get_count();
	}

	T* operator[](t_size index) const {
		if (index >= items.get_count()) {
			throw pfc::exception_overflow();
		}
		return items[index];
	}

	inline T* get(t_size index) const {
		return (*this)[index];
	}

protected:
	pfc::list_t<T*> items;
};
