#define GETTER(type, name) \
	type get_##name() const { \
		return this->name; \
	}

#define SETTER(type, name) \
	void set_##name(type value) { \
		this->name = value; \
	}

#define GETTER_BY_REFERENCE(type, name) \
	type &get##name() const { \
		return this->name; \
	}

#define SETTER_BY_REFERENCE(type, name) \
	void set##name(type &value) \
		d->name = value; \
	}

#define ACCESSOR(type, name) \
	GETTER(type, name) \
	SETTER(type, name)

#define ACCESSOR_BY_REFERENCE(type, name) \
	GETTER(type, name) \
	SETTER(type, name)

#define MEMBER_BY_REFERENCE(type, name) \
	protected: \
		type name; \
	public: \
	ACCESSOR_BY_REFERENCE(type, name)
