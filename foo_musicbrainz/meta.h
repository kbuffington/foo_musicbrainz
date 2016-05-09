#pragma once

#define GETTER(type, name) \
	type get_##name() const { \
		return this->name; \
	}

#define SETTER(type, name) \
	void set_##name(type value) { \
		this->name = value; \
	}

#define SETTER_BY_POINTER(type, name) \
	void set_##name(type value) { \
		this->name = *value; \
	}

#define GETTER_BY_REFERENCE(type, name) \
	const type &get_##name() const { \
		return this->name; \
	}

#define SETTER_BY_REFERENCE(type, name) \
	void set_##name(type &value) { \
		this->name = value; \
	}

#define ACCESSOR(type, name) \
	GETTER(type, name) \
	SETTER(type, name)

#define ACCESSOR_BY_REFERENCE(type, name) \
	GETTER_BY_REFERENCE(type, name) \
	SETTER_BY_REFERENCE(type, name)

#define MEMBER(type, name) \
	protected: \
		type name; \
	public: \
	ACCESSOR(type, name)

#define MEMBER_BY_REFERENCE(type, name) \
	protected: \
		type name; \
	public: \
	ACCESSOR_BY_REFERENCE(type, name)

#define STRING_MEMBER(name) \
	MEMBER_BY_REFERENCE(pfc::string8, name) \
	SETTER(const char *, name)

#define POINTER_MEMBER(type, name) \
	protected: \
		type *name; \
	public: \
	void set_##name(type *value) { \
		if (name != nullptr) { \
			delete name; \
		} \
		name = value; \
	} \
	type *get_##name() const { \
		return this->name; \
	} \
	type *extract_##name() { \
		type *tmp = this->name; \
		this->name = nullptr; \
		return tmp; \
	}

#define POINTER_MEMBER_DESTRUCTOR(name) \
	if (name != nullptr) { \
		delete name; \
	}

#define STRING_LIST(name, name_plural, count) \
	public: \
	void set_##name(size_t i) { \
		if (i < count) { \
			name = i; \
		} else { \
			name = 0; \
		} \
	} \
	void set_##name(const char *new_##name) { \
		for (size_t i = 0; i < count; i++) { \
			if (_stricmp(new_##name, name_plural[i]) == 0) { \
				name = i; \
				return; \
			} \
		} \
		name = 0; \
	} \
	pfc::string8 get_##name() const { \
		return pfc::string8(name_plural[name]); \
	} \
	size_t get_##name##_index() const { \
		return name; \
	} \
		static const size_t name##_count = count; \
		static const char *name_plural[]; \
	private: \
		size_t name;

#define COLLECTION(T, name) \
	public: \
		void add_##name(T *item) { \
			name##_items.add_item(item); \
		}; \
		size_t name##_count() const{ \
			return name##_items.get_count(); \
		} \
		T *get_##name(t_size index) const { \
			if (index >= name##_items.get_count()) { \
				throw pfc::exception_overflow(); \
			} \
			return name##_items[index]; \
		} \
		T *extract_##name(t_size index) { \
			T *item = get_##name(index); \
			name##_items.remove_by_idx(index); \
			return item; \
		} \
		void remove_##name(t_size index) { \
			delete extract_##name(index); \
		} \
	protected: \
		pfc::list_t<T *> name##_items;

#define COLLECTION_DESTRUCTOR(name) \
	for (size_t i = 0; i <  name##_items.get_count(); i++) { \
		delete name##_items[i]; \
	}