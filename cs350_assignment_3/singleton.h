#ifndef SINGLETON_H
#define SINGLETON_H

#define SINGLETON(TClass)						\
												\
	private:									\
	TClass() = default;							\
	TClass(const TClass &) = delete;			\
	void operator = (const TClass &) = delete;	\
	public:										\
	static TClass & instance() {				\
		static TClass INSTANCE;					\
		return INSTANCE;						\
	}

#define PARENT_SINGLETON(TClass)				\
												\
	protected:									\
	TClass() = default;							\
	TClass(const TClass &) = delete;			\
	void operator = (const TClass &) = delete;	\
	public:										\
	static TClass & instance() {				\
		static TClass INSTANCE;					\
		return INSTANCE;						\
	}

#define CHILD_SINGLETON(TClass)					\
	public:										\
	static TClass & instance() {				\
		static TClass INSTANCE;					\
		return INSTANCE;						\
	}
#endif