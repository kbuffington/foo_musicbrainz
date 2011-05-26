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
	const type &get_##name() const { \
		return *this->name; \
	}

#define POINTER_MEMBER_DESTRUCTOR(name) \
	if (name != nullptr) { \
		delete name; \
	}
