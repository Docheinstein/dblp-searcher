#ifndef TYPE_H
#define TYPE_H

template <class T> class Type {
public:

	Type() = default;
	Type(T t) : mType(t) {}

	T type() { return mType; }

	bool operator == (Type<T> t) { return mType == t.mType; }
	bool operator != (Type<T> t) { return mType == t.mType; }

protected:
	T mType;
};

#endif // TYPE_H
